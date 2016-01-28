static char const cvsid[] = "$Id: main.c,v 1.1 2007/07/09 16:25:17 hammes Exp $";

/*
 * Copyright 2005 SRC Computers, Inc.  All Rights Reserved.
 *
 *	Manufactured in the United States of America.
 *
 * SRC Computers, Inc.
 * 4240 N Nevada Avenue
 * Colorado Springs, CO 80907
 * (v) (719) 262-0213
 * (f) (719) 262-0223
 *
 * No permission has been granted to distribute this software
 * without the express permission of SRC Computers, Inc.
 *
 * This program is distributed WITHOUT ANY WARRANTY OF ANY KIND.
 */

#include <libmap.h>
#include <stdlib.h>

#define SZ 65536

void cpuv (int32_t A[], int64_t C[], int num, int *nret);

void subr (int32_t In[], int64_t Out[], int num, int *nret, int64_t *time, int mapnum);

int main (int argc, char *argv[]) {
    FILE *res_map, *res_cpu;
    int i, num, nret,nretc;
    int32_t *A;
    int64_t *B;
    int64_t *C;
    int64_t tm;
    int mapnum = 0;

    if ((res_map = fopen ("res_map", "w")) == NULL) {
        fprintf (stderr, "failed to open file 'res_map'\n");
        exit (1);
        }

    if ((res_cpu = fopen ("res_cpu", "w")) == NULL) {
        fprintf (stderr, "failed to open file 'res_cpu'\n");
        exit (1);
        }

    if (argc < 2) {
	fprintf (stderr, "need number of elements (up to %d) as arg\n", SZ);
	exit (1);
	}

    if (sscanf (argv[1], "%d", &num) < 1) {
	fprintf (stderr, "need number of elements (up to %d) as arg\n", SZ);
	exit (1);
	}

    if (num > SZ) {
	fprintf (stderr, "need number of elements (up to %d) as arg\n", SZ);
	exit (1);
	}

    A = (int32_t*) Cache_Aligned_Allocate (2*SZ * sizeof (int32_t));
    B = (int64_t*) Cache_Aligned_Allocate (2*SZ * sizeof (int64_t));
    C = (int64_t*) Cache_Aligned_Allocate (2*SZ * sizeof (int64_t));

    srandom (99);

    for (i=0; i<num; i++) {
        A[2*i] = random () & 0xffff;
        A[2*i+1] = A[2*i] + 100000;
	}

    map_allocate (1);

    // call the MAP routine
    subr (A, B, num, &nret, &tm, mapnum);

    printf ("combined DMA and compute time: %lld clocks\n", tm);

    cpuv (A, C, num, &nretc);

    for (i=0; i<nret; i++)
        fprintf (res_map, "i %i %lld %lld\n", i,B[2*i],B[2*i+1]);

    for (i=0; i<nretc; i++)
        fprintf (res_cpu, "i %i %lld %lld\n", i,C[2*i],C[2*i+1]);

 if (nret != nretc)  {
    printf ("ERROR nret %i nretc %i\n",nret,nretc);

    for (i=0; i<nret; i++)  {
        printf ("i %3i MAP %8lld %3lld   ", i,B[2*i],B[2*i+1]);
        printf ("CPU  %8lld %3lld\n", C[2*i],C[2*i+1]);
        }
    }


    map_free (1);

    exit(0);
    }

void cpuv (int32_t A[], int64_t C[], int num, int *nret)
{
    int i,ii;
    int64_t v0;

    ii = 0;
    for (i=0;i<num*2;i++)  {
       v0 = A[i];

       if (v0>256 & v0<10512)  {
       C[2*ii] = v0;
       C[2*ii+1] = i/2;
       ii++;
        }
    }
    *nret = ii;
}

