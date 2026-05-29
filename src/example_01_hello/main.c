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

#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

/**
 * @brief Macro for MPI error checking.
 *
 * If the MPI call fails, it prints an error message and aborts the application.
 */
#define MPI_CHECK(call)                              \
    do                                               \
    {                                                \
        int err__ = (call);                          \
        if (err__ != MPI_SUCCESS)                    \
        {                                            \
            char errstr[MPI_MAX_ERROR_STRING];       \
            int len__ = 0;                           \
            MPI_Error_string(err__, errstr, &len__); \
            fprintf(stderr,                          \
                    "MPI error at %s:%d -> %s\n",    \
                    __FILE__,                        \
                    __LINE__,                        \
                    errstr);                         \
            MPI_Abort(MPI_COMM_WORLD, err__);        \
        }                                            \
    } while (0)

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
int main(int argc, char *argv[])
{
    MPI_CHECK(MPI_Init(&argc, &argv));

    int rank = -1;
    int size = -1;

    MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));

    printf("[rank %d/%d] Hello from MPI process.\n", rank, size);

    /*
     * Flush stdout to reduce output buffering effects when multiple
     * ranks write concurrently.
     */
    fflush(stdout);

    MPI_CHECK(MPI_Finalize());

    return EXIT_SUCCESS;
}
