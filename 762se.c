/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 * Application:    Distributed Algorithm for Simulation of Delta Networks  *
 *                                                                         *
 * File Name:      762se.c --- Slave Program                               *
 *                                                                         *
 * Direction:      Master program in  "762me.c"                            *
 *                 Process Group File "empty.pg"                           *
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
#include "762e.h"

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

        to = 0;   /* master */
        p4_sendr(DATA, to, q, size);
    }
} /* end of slave */
