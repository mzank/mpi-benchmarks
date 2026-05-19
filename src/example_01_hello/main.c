#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

static noreturn void die(const char *msg, int err)
{
    fprintf(stderr, "%s (MPI error code: %d)\n", msg, err);
    MPI_Abort(MPI_COMM_WORLD, err);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS)
    {
        fprintf(stderr, "MPI_Init failed\n");
        return EXIT_FAILURE;
    }

    int rank = -1;
    int size = -1;

    err = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (err != MPI_SUCCESS)
        die("MPI_Comm_rank failed", err);

    err = MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (err != MPI_SUCCESS)
        die("MPI_Comm_size failed", err);

    printf("[rank %d/%d] Hello from MPI process.\n", rank, size);

    fflush(stdout);

    err = MPI_Finalize();
    if (err != MPI_SUCCESS)
    {
        fprintf(stderr, "MPI_Finalize failed\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
