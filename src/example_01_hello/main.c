/**
 * @file main.c
 * @brief Minimal MPI "Hello World" example with error handling.
 *
 * Build example:
 * @code
 * mpicc -std=c11 -Wall -Wextra -Wpedantic -O3 main.c -o example_01_hello
 * @endcode
 *
 * Run example:
 * @code
 * mpirun -n 4 ./example_01_hello
 * @endcode
 *
 * Expected output:
 * @code
 * [rank 0/4] Hello from MPI process.
 * [rank 1/4] Hello from MPI process.
 * [rank 2/4] Hello from MPI process.
 * [rank 3/4] Hello from MPI process.
 * @endcode
 *
 * @author Marco Zank
 * @date 2026
 * @version 0.1
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

/**
 * @brief Abort the MPI program after reporting an error.
 *
 * Prints a diagnostic message to stderr and terminates all processes
 * in the communicator using MPI_Abort().
 *
 * @param[in] msg error message.
 * @param[in] err MPI error code returned by an MPI routine.
 *
 * @warning
 * MPI_Abort() terminates all ranks in the communicator immediately.
 *
 * @post
 * This function never returns.
 */
static noreturn void die(const char *msg, int err)
{
    fprintf(stderr, "%s (MPI error code: %d)\n", msg, err);

    MPI_Abort(MPI_COMM_WORLD, err);

    /* Defensive fallback in case MPI_Abort returns unexpectedly. */
    exit(EXIT_FAILURE);
}

/**
 * @brief Entry point of the MPI application.
 *
 * Initializes the MPI runtime, queries the rank and communicator size,
 * prints a greeting from each process, and finalizes MPI cleanly.
 *
 * @param[in] argc Argument count from the command line.
 * @param[in] argv Argument vector from the command line.
 *
 * @retval EXIT_SUCCESS Program completed successfully.
 * @retval EXIT_FAILURE MPI initialization or finalization failed.
 */
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
    {
        die("MPI_Comm_rank failed", err);
    }

    err = MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (err != MPI_SUCCESS)
    {
        die("MPI_Comm_size failed", err);
    }

    printf("[rank %d/%d] Hello from MPI process.\n", rank, size);

    /*
     * Flush stdout to reduce output buffering effects when multiple
     * ranks write concurrently.
     */
    fflush(stdout);

    err = MPI_Finalize();

    if (err != MPI_SUCCESS)
    {
        fprintf(stderr, "MPI_Finalize failed\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
