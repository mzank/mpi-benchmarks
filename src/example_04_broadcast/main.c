/**
 * @file main.c
 * @brief MPI Broadcast benchmark measuring broadcast time and effective bandwidth.
 *
 * This benchmark measures the performance of a broadcast communication pattern
 * where rank 0 broadcasts data to all other ranks using MPI_Bcast.
 *
 * Build example (manual):
 * @code
 * mpicc -std=c11 -Wall -Wextra -Wpedantic -O3 -march=native -flto main.c -o example_04_broadcast
 * @endcode
 *
 * @note The CMake build system places the binary in `build/bin/example_04_broadcast`.
 *
 * Run example (manual):
 * @code
 * mpirun -n 4 ./example_04_broadcast
 * @endcode
 *
 * Expected output:
 * @code
 * # MPI Broadcast Benchmark
 * ...
 * # Metrics
 * #   Broadcast        : broadcast time
 * #   EffectiveBW      : effective bandwidth across all processes
 * #
 * # Size(Bytes)   Broadcast(us)    EffectiveBW(MiB/s)
 *        1 bytes      1.20 us        3.18 MiB/s
 *        2 bytes      1.25 us        6.10 MiB/s
 * ...
 * @endcode
 *
 * @author Marco Zank
 * @date 2026
 * @version 0.1
 */

#define _GNU_SOURCE

#include <inttypes.h>
#include <limits.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

/** @brief Maximum message size (16 MiB). */
#define MAX_MSG_SIZE ((size_t)1 << 24)

/** @brief Number of iterations for small messages (<= 8 KiB). */
#define ITER_SMALL 1000

/** @brief Number of iterations for large messages (> 8 KiB). */
#define ITER_LARGE 100

/** @brief Number of warmup iterations. */
#define WARMUP 20

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
 * @brief Prints the CPU affinity mask and current CPU of the calling process.
 *
 * @param[in] rank The MPI rank of the calling process.
 */
static void print_affinity(const int rank)
{
    cpu_set_t mask;

    CPU_ZERO(&mask);

    if (sched_getaffinity(0, sizeof(mask), &mask) != 0)
    {
        perror("sched_getaffinity");
        return;
    }

    printf("Rank %d affinity mask: ", rank);

    for (int cpu = 0; cpu < CPU_SETSIZE; cpu++)
    {
        if (CPU_ISSET(cpu, &mask))
        {
            printf("%d ", cpu);
        }
    }

    printf("\n");

    int current_cpu = sched_getcpu();

    if (current_cpu >= 0)
    {
        printf("Rank %d current logical CPU: %d\n",
               rank,
               current_cpu);
    }
}

/**
 * @brief Prints SLURM-specific environment variables for the process.
 *
 * @param[in] rank The MPI rank of the calling process.
 */
static void print_slurm_info(const int rank)
{
    const char *procid = getenv("SLURM_PROCID");
    const char *localid = getenv("SLURM_LOCALID");
    const char *nodeid = getenv("SLURM_NODEID");

    printf("Rank %d SLURM info: "
           "PROCID=%s LOCALID=%s NODEID=%s\n",
           rank,
           procid ? procid : "N/A",
           localid ? localid : "N/A",
           nodeid ? nodeid : "N/A");
}

/**
 * @brief Performs a broadcast operation of data from the root process.
 *
 * Broadcasts data from the specified root rank to all processes in the communicator.
 *
 * @param[in,out] buffer Pointer to the data buffer.
 * @param[in] count Number of bytes to broadcast.
 * @param[in] root Rank of the root process.
 */
static void broadcast_exchange(void *buffer,
                               const int count,
                               const int root)
{
    MPI_CHECK(MPI_Bcast(buffer,
                        count,
                        MPI_BYTE,
                        root,
                        MPI_COMM_WORLD));
}

/**
 * @brief Calculates a simple additive checksum for verification.
 *
 * @param[in] buf Pointer to the buffer.
 * @param[in] n Number of bytes in the buffer.
 * @return The calculated checksum.
 */
static uint64_t checksum(const unsigned char *buf, size_t n)
{
    uint64_t s = 0;
    for (size_t i = 0; i < n; i++)
        s += (uint64_t)buf[i];
    return s;
}

/**
 * @brief Entry point of the MPI application.
 *
 * Initializes the MPI runtime, validates the process count, performs the warmup
 * and measurement loops for increasing message sizes in a broadcast pattern,
 * verifies the data integrity, and finalizes MPI cleanly.
 *
 * @param[in] argc Argument count from the command line.
 * @param[in] argv Argument vector from the command line.
 *
 * @retval EXIT_SUCCESS Program completed successfully.
 * @retval EXIT_FAILURE MPI initialization or finalization failed.
 */
int main(int argc, char *argv[])
{
    int rank = 0;
    int size = 0;

    MPI_CHECK(MPI_Init(&argc, &argv));
    MPI_CHECK(MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN));

    MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));

    if (size < 2)
    {
        if (rank == 0)
        {
            fprintf(stderr,
                    "This benchmark requires at least 2 MPI ranks.\n");
        }

        MPI_CHECK(MPI_Finalize());

        return EXIT_FAILURE;
    }

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int processor_name_len = 0;

    MPI_CHECK(MPI_Get_processor_name(processor_name,
                                     &processor_name_len));

    for (int r = 0; r < size; r++)
    {
        MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

        if (rank == r)
        {
            printf("Rank %d running on %s\n",
                   rank,
                   processor_name);

            print_slurm_info(rank);
            print_affinity(rank);

            fflush(stdout);
        }
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

    unsigned char *buffer = malloc(MAX_MSG_SIZE);

    if (!buffer)
    {
        fprintf(stderr, "malloc failed\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    memset(buffer, rank == 0 ? 0xAA : 0, MAX_MSG_SIZE);

    double *all_bw = NULL;
    uint64_t *all_checksum = NULL;

    if (rank == 0)
    {
        all_bw = malloc(size * sizeof(double));
        all_checksum = malloc(size * sizeof(uint64_t));

        if (!all_bw || !all_checksum)
        {
            fprintf(stderr, "malloc failed\n");
            free(all_bw);
            free(all_checksum);
            free(buffer);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    if (rank == 0)
    {
        printf("\n");
        printf("# MPI Broadcast Benchmark\n");
        printf("#\n");
        printf("# Configuration\n");
        printf("#   MPI ranks        : %d\n", size);
        printf("#   Max message size : %zu bytes\n", MAX_MSG_SIZE);
        printf("#   Warmup iterations: %d\n", WARMUP);
        printf("#   Small iterations : %d\n", ITER_SMALL);
        printf("#   Large iterations : %d\n", ITER_LARGE);
        printf("#   MPI_Wtick        : %.9e seconds\n", MPI_Wtick());
        printf("#\n");
        printf("# Metrics\n");
        printf("#   Broadcast        : broadcast time\n");
        printf("#   EffectiveBW      : effective bandwidth across all processes\n");
        printf("#\n");
        printf("# Size(Bytes)\tBroadcast(us)\tEffectiveBW(MiB/s)\n");
    }

    for (size_t msg_size = 1;
         msg_size <= MAX_MSG_SIZE;
         msg_size *= 2)
    {
        if (msg_size > (size_t)INT_MAX)
            break;

        const int count = (int)msg_size;
        const int iters = (msg_size <= 8192) ? ITER_SMALL : ITER_LARGE;

        for (int i = 0; i < WARMUP; i++)
        {
            MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
            broadcast_exchange(buffer, count, 0);
        }

        /* Clear receive buffer after warmup so that the timed loop
         * must actively transfer data to pass checksum verification. */
        if (rank != 0)
        {
            memset(buffer, 0, msg_size);
        }

        MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

        double local_elapsed = 0.0;

        for (int i = 0; i < iters; i++)
        {
            MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

            const double start = MPI_Wtime();
            broadcast_exchange(buffer, count, 0);
            const double end = MPI_Wtime();

            local_elapsed += (end - start);
        }

        const double local_bcast_time =
            local_elapsed / (double)iters;

        double max_bcast_time = 0.0;

        MPI_CHECK(MPI_Reduce(&local_bcast_time,
                             &max_bcast_time,
                             1,
                             MPI_DOUBLE,
                             MPI_MAX,
                             0,
                             MPI_COMM_WORLD));

        const double bcast_us = max_bcast_time * 1e6;

        const double bw_MiBs =
            ((double)msg_size * (size - 1)) /
            max_bcast_time /
            (1024.0 * 1024.0);

        const double local_bw_MiBs =
            ((double)msg_size * (size - 1)) /
            local_bcast_time /
            (1024.0 * 1024.0);

        const uint64_t csum =
            checksum(buffer, msg_size);

        MPI_CHECK(MPI_Gather(&local_bw_MiBs,
                             1,
                             MPI_DOUBLE,
                             all_bw,
                             1,
                             MPI_DOUBLE,
                             0,
                             MPI_COMM_WORLD));

        MPI_CHECK(MPI_Gather(&csum,
                             1,
                             MPI_UINT64_T,
                             all_checksum,
                             1,
                             MPI_UINT64_T,
                             0,
                             MPI_COMM_WORLD));

        if (rank == 0)
        {
            printf("%8zu bytes  %8.2f us  %8.2f MiB/s\n",
                   msg_size,
                   bcast_us,
                   bw_MiBs);

            int ok = 1;

            for (int r = 0; r < size; r++)
            {
                const uint64_t expected =
                    (uint64_t)0xAA * (uint64_t)msg_size;

                if (all_checksum[r] != expected)
                {
                    ok = 0;

                    fprintf(stderr,
                            "Verification failed: rank %d expected %" PRIu64 " got %" PRIu64 "\n",
                            r,
                            expected,
                            all_checksum[r]);
                }
            }

            if (msg_size == MAX_MSG_SIZE)
            {
                printf("  Verification data:\n");

                for (int r = 0; r < size; r++)
                {
                    const uint64_t expected_checksum =
                        (uint64_t)0xAA * (uint64_t)msg_size;

                    printf("    rank %d : "
                           "bw=%9.3f MiB/s  "
                           "checksum=%12" PRIu64 "  "
                           "expected=%12" PRIu64 "\n",
                           r,
                           all_bw[r],
                           all_checksum[r],
                           expected_checksum);
                }
            }

            if (!ok)
            {
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }

    free(buffer);

    if (rank == 0)
    {
        free(all_bw);
        free(all_checksum);
    }

    MPI_CHECK(MPI_Finalize());

    return EXIT_SUCCESS;
}
