#define _GNU_SOURCE

#include <limits.h>
#include <mpi.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MSG_SIZE ((size_t)1 << 24) /* 16 MiB */

#define ITER_SMALL 1000
#define ITER_LARGE 100
#define WARMUP 20

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

int main(int argc, char *argv[])
{
    int rank = 0;
    int size = 0;

    MPI_CHECK(MPI_Init(&argc, &argv));

    MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));

    if (size != 2)
    {
        if (rank == 0)
        {
            fprintf(stderr,
                    "This benchmark requires exactly 2 MPI ranks.\n");
        }

        MPI_CHECK(MPI_Finalize());

        return EXIT_FAILURE;
    }

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int processor_name_len = 0;

    MPI_CHECK(
        MPI_Get_processor_name(processor_name,
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

    unsigned char *sendbuf = malloc(MAX_MSG_SIZE);
    unsigned char *recvbuf = malloc(MAX_MSG_SIZE);

    if (!sendbuf || !recvbuf)
    {
        fprintf(stderr, "malloc failed\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    memset(sendbuf, rank, MAX_MSG_SIZE);
    memset(recvbuf, 0, MAX_MSG_SIZE);

    if (rank == 0)
    {
        printf("\n");
        printf("# MPI Ping-Pong Benchmark\n");
        printf("#\n");
        printf("# Configuration\n");
        printf("#   Max message size : %zu bytes\n",
               MAX_MSG_SIZE);
        printf("#   Warmup iterations: %d\n",
               WARMUP);
        printf("#   Small iterations : %d\n",
               ITER_SMALL);
        printf("#   Large iterations : %d\n",
               ITER_LARGE);
        printf("#   MPI_Wtick        : %.9e seconds\n",
               MPI_Wtick());
        printf("#\n");
        printf("# Metrics\n");
        printf("#   Latency          : one-way latency\n");
        printf("#   Throughput       : round-trip throughput\n");
        printf("#\n");
        printf("# Size(Bytes)\tLatency(us)\tEffectiveBW(MiB/s)\n");
    }

    const int peer = (rank == 0) ? 1 : 0;

    for (size_t msg_size = 1; msg_size <= MAX_MSG_SIZE; msg_size *= 2)
    {
        if (msg_size > (size_t)INT_MAX)
            break;

        int count = (int)msg_size;

        int iters = (msg_size <= 8192) ? ITER_SMALL : ITER_LARGE;

        for (int i = 0; i < WARMUP; i++)
        {
            MPI_CHECK(MPI_Sendrecv(sendbuf, count, MPI_BYTE, peer, 0,
                                   recvbuf, count, MPI_BYTE, peer, 0,
                                   MPI_COMM_WORLD, MPI_STATUS_IGNORE));
        }

        MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

        double start = MPI_Wtime();

        for (int i = 0; i < iters; i++)
        {
            MPI_CHECK(MPI_Sendrecv(sendbuf, count, MPI_BYTE, peer, 0,
                                   recvbuf, count, MPI_BYTE, peer, 0,
                                   MPI_COMM_WORLD, MPI_STATUS_IGNORE));
        }

        double end = MPI_Wtime();

        if (rank == 0)
        {
            double rtt = (end - start) / iters;
            double latency_us = (rtt / 2.0) * 1e6;
            double bw = (2.0 * msg_size) / rtt / (1024.0 * 1024.0);

            printf("%8zu bytes  %8.2f us  %8.2f MiB/s\n",
                   msg_size, latency_us, bw);
        }
    }

    free(sendbuf);
    free(recvbuf);

    MPI_CHECK(MPI_Finalize());

    return EXIT_SUCCESS;
}
