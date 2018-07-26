/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 * Application:    Distributed Algorithm for Simulation of Delta Networks  *
 *                                                                         *
 * File Name:      762m.c --- Master Program                               *
 *                                                                         *
 * Direction:      Slave program in "762s.c"			                    	   *
 *                 Process Group File "procgroup" 		 	                   *
 *                                                                         *
 * Language:       C Language + P4                                         *
 * Author:         Jordon Lin                                              *
 * Date:           April 30, 1993                                          *
 *                                                                         *
 * Note: this program must be linked with "p4_cmain.o" in lib directory    *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <limits.h>
#include <math.h>
#include "p4.h"
#include "762.h"
/*------------------------------------------------------------------------*/
/*   q-shuffle of qr objects                                              */
/*------------------------------------------------------------------------*/
int shuffle(q, qr, i)
int q, qr, i;
{
    int index;
    return ((i == (index = qr - 1)) ? i : q * i % index);
}
/*------------------------------------------------------------------------*/
/*   routing                                                              */
/*------------------------------------------------------------------------*/
int routing(/*b, n, */address, k)
/*int b, n,*/
int k, address;
{
    char *s;
    int i, d, bit;

    d = address;
    s = (char *) p4_malloc((n+1) * sizeof (char));
    for (i = 0; i < n; i++)
    {
	s[i] = d % b + '0';
	d /= b;
    }
    s[i] = '\0';

    bit = (int) s[n - 1 - k] - '0';
    p4_free(s);

    return bit;
}
/*------------------------------------------------------------------------*/
/*   Switching Element                                                    */
/*------------------------------------------------------------------------*/
int *switching(/*a, b, n,*/ p, k, num)
/*int a, b, n;*/
int k, *p, num;
{
    int **queue;
    /* num : numbers of switching elements */
    int *q;
    int size; /* numbers of outputs */
    int i, j;
    int output, input,from,type;
    int size2;
    int size3;
    int *incoming, nslaves, my_id;

    /* size = (int) pow((double) a, (double) n - k - 1)
               * pow((double) b, (double) k + 1); */
    size = N;
    q = (int *) p4_malloc(size * sizeof(int));
    for (i = 0; i < N; i++)
         q[i] =EMPTY;

    queue = (int **) p4_malloc(b * sizeof(*queue));
    for (i = 0; i < b; i++)
    {
	queue[i] = (int *) p4_malloc((a+1)* sizeof(*queue));
	memset(queue[i], 0, sizeof(int) * (a + 1));
    }

    size3 = N/P*sizeof(int);
    /* repeat for numbers of switching elements */
    for (j = 1; j < P; j++)
    {
        p4_sendr(DATA,j, (p+j*4),size3); 
    }
    
    for (j = 0; j < N/P/a; j++)
    {
        for (i = 0; i < a; i++)
        {
            input = i + j * a;
            if (p[input] != EMPTY)
            {
                output = routing(/*b, n, */p[input], k);
		queue[output][0] += 1;
		queue[output][queue[output][0]] = input;
            }
        }
        for (i = 0; i < b; i++)
	{
	    if (queue[i][0] > 0)
	    {
		if (queue[i][0] == 1)
		    q[i+j*b] = p[queue[i][1]];
		else
		{
		    input = (int)((double) rand() / (double) INT_MAX) * queue[i][0] + 1;
		    output = i + j * b;
		    q[output] = p[queue[i][input]];
		}
		queue[i][0] = 0;
	    }
	}
    }
    /* receive data from slave */
    type = DATA;
    incoming = NULL;
    for (from = 1; from < P; from++)
    {
        p4_recv(&type,&from,&incoming,&size3);
        
        for (i = 0; i < N/P; i++)
            q[N/P * from + i] = *(incoming+i);
        p4_msg_free(incoming);
    }
    p4_free(p);
    for (i = 0; i < b; i++)
	p4_free(queue[i]);
    p4_free(queue);
    
    return q;
}
/*------------------------------------------------------------------------*/
/*   Simulate a^n x b^n Delta Network                                     */
/*------------------------------------------------------------------------*/
int simulate(/*a, b, n, */m, nslaves)
/*int a, b, n;*/ 
float m; int nslaves;
{
    int *p, *q;   /* q: input buffer; p: output buffer */
    int i, j;
    int size, size2, BW;

    /* initialize 
    size = (int) pow((double) a, (double) n);
    size2 = (int) pow((double) b, (double) n);*/
    size = N;
    size2 = N;
    /* allocate memory for a^n inputs */
    q = (int *) p4_malloc(size * sizeof(int));
    memset(q, EMPTY, size);
    /* generate input requests */
    for (i = 0; i < size; i++)      
       if ((float) rand() / (float) INT_MAX < m)
            q[i] = (int) ((float) rand() / (float) INT_MAX * size2);
    /* go through crossbar switch */
    p = switching(/*a, b, n, */q, 0, nslaves);
/*
    printf("\nOutput of stage %i:\n",0);
    for (i = 0; i < size; i++)
        printf("%5i%c", p[i], ((i+1)%a == 0) ? '\n' : ' ');
    puts("-----------------------------------------");
*/
    for (j = 1; j < n ; j++)
    {
        /* allocate memory for next stage */
        size = size * b / a;
        q = (int *)p4_malloc(size*sizeof(int));
        /* shuffle */
        for (i = 0; i < size; i++)
            q[shuffle(a, size, i)] = p[i];
        p4_free(p);
        /* go through crossbar switch */
        p = switching(/*a, b, n, */q, j, nslaves);
/*
        printf("\nOutput of stage %i:\n",j-1);
        for (i = 0; i < size; i++)
             printf("%5i%c", q[i], ((i+1)%a == 0) ? '\n' : ' ');
        puts("-----------------------------------------");
*/
    }
    BW = 0;
    for (i = 0; i < size2; i++)
        if (p[i] != EMPTY)
            BW++;
    p4_free(p);

    return BW;
}
  
void main(argc,argv)
int argc;
char **argv;
{
/*    int a, b, n;*/
    float m;
    int i, j, scale, times, BW;
    double sum, m0, mi;
    int nslaves, type, size, id, to, from, my_id;
    int pN, pP;
    time_t t1, t2, t3, t4;
 
    time(&t1);
    /* input variables */
    /*fprintf(stdout,"Please input simulation times = ");
    fscanf(stdin,"%i",&times);
    fflush(stdin);*/
    times = TIMES;
/*    a = 4;
    b = 4;
    n = 3;
*/    m = 0.9;
    BW = 0;
    sum = 0.0;
    pN = N;
    pP = P;
    
    time(&t1);
    p4_initenv(&argc,argv);
    p4_create_procgroup();
    /*nslaves = p4_num_total_ids() - 1;
    printf("Number of Slaves = %d\n",nslaves);*/
    
    size = 1;
/*
    for (j = 1; j < P; j++)
    {
        p4_sendr(DATA,j, &times, size);
    }
*/
    scale = (times > 50) ? times / 50 : 1;
    for (i = 0; i < times / scale; i++)
        putchar('.');
    for (i = 0; i < times / scale; i++)
        putchar('\b');

    for (i = 1; i <= times; i++)
    {
        BW = simulate(m, nslaves);
        sum = sum + BW;
        if (i % scale == 0)
            putchar('o');
    }
    p4_wait_for_end();
    sum = sum / times;
    fprintf(stdout,"\nBandwidth of %i^%i x %i^%i delta network(%0.3f)",
            a, n, b, n, m);
    fprintf(stdout,"\nSimulation Bandwidth = %lf", sum);
   
    m0 = (double) m;
    for (i = 1; i <= n; i++)
    {
        mi = 1 - pow(1 - (m0 / b), (double) a);
        m0 = mi;
    }
    sum = pow((double) b, (double) n) * m0;
    fprintf(stdout,"\nAnalytical Bandwidth = %lf\n", sum);
    time(&t2);
    t3 = t2 - t1;
    printf("begin at %s\n", ctime(&t1));
    printf("  end at %s\n", ctime(&t2));
  }
