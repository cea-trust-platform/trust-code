/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2006 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   /* For memset */

int main( int argc, char *argv[] )
{
    MPI_Request r[1];
    MPI_Status  s[1];
    int *buf0;
    int rank, size, src, dest, flag, errs = 0;
    int n0;
    MPI_Comm comm;

    MPI_Init( &argc, &argv );

    MPI_Comm_size( MPI_COMM_WORLD, &size );
    if (size < 2) {
	fprintf( stderr, "Must run with at least 2 processes\n" );
	MPI_Abort( MPI_COMM_WORLD, 1 );
    }
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    dest = 0;
    src  = 1;
    comm = MPI_COMM_WORLD;

    n0 = 65536;
    buf0 = (int *)malloc( n0 * sizeof(int) );
    if (!buf0)  {
	fprintf( stderr, "Unable to allocate buffers of size %d\n", 
		 n0 * (int)sizeof(int) );
	MPI_Abort( MPI_COMM_WORLD, 1 );
    }
    memset( buf0, -1, n0 * sizeof(int) );
 
    if (rank == dest) {
	MPI_Irecv( buf0, n0, MPI_INT, src, 4, comm, &r[0] );
        MPI_Waitall( 1, r, s );

    }
    if (rank == src) {
	int tflag;
	MPI_Isend( buf0, n0, MPI_INT, dest, 4, comm, &r[0] );
        MPI_Waitall( 1, r, s );
    }

    free(buf0);
    MPI_Barrier( comm );
    MPI_Finalize();

    return 0;
}
