/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 * Application:    Distributed Algorithm for Simulation of Delta Networks  *
 *                                                                         *
 * File Name:      762s.c --- Slave Program                                *
 *                                                                         *
 * Direction:      Master program in  "762m.c"                             *
 *                 Process Group File "procgroup"                          *
 *                                                                         *
 * Language:       C Language + P4                                         *
 * Author:         Jordon Lin                                              *
 * Date:           April 30, 1993                                          *
 *                                                                         *
 * Note that this program must be linked with "p4_cmain.o" in lib dir.     *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "p4.h"
#include <limits.h>
#include "762.h"
/*------------------------------------------------------------------------*/
/*   routing                                                              */
/*------------------------------------------------------------------------*/
int route(/*b, n, */address, k)
/*int b, n, */
int k, address;
{
    char *s;
    int i, d, bit;

    d = address;
    s = (char *) p4_malloc((n+1)*sizeof (char));
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
/*   Slave Process                                                        */
/*------------------------------------------------------------------------*/
slave()
{
    int i, j, k, l, type, to, from, size, my_id;
    int *p, *q, output, input;
    int **queue;

    my_id = p4_get_my_id();
    type = DATA;

    size = -1;
    from = 0;  /* master process */

    for (l = 1; l <= TIMES; l++)
        for (k = 0; k < n; k++)
        {
        /* receive the data from master */
        size = -1;
        from = 0;  /* master process */
        p = NULL;
        p4_recv(&type, &from, &p, &size);
        p4_msg_free(p);
/*
        printf("\n slave %d: Requests = \n",my_id);
        for (i = 0; i < N/P; i++)
            printf("%5d%c", *(p+i), ((i+1)%a == 0) ? '\n' : ' ');
*/
        q = (int *) p4_malloc(N/P * sizeof(int));
        memset(q, EMPTY, sizeof(int) * N/P);
    
        queue = (int **) p4_malloc(b * sizeof(*queue));
        for (i = 0; i < b; i++)
        {
	    queue[i] = (int *) p4_malloc((a+1)* sizeof(*queue));
            memset(queue[i], 0, sizeof(int) * (a + 1));
        }

        /* compute the allocated data */

        for (j = 0; j < N/P/a; j++)
        {
            for (i = 0; i < a; i++)
            {
                input = i + j * a;
                if (p[input] != EMPTY)
                {
                    output = route(/*b, n, */p[input], k);
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
        for (i = 0; i < b; i++)
	    p4_free(queue[i]);
        p4_free(queue);
        to = 0;   /* master */
        p4_sendr(DATA, to, q, size);
    }
} /* end of slave */
