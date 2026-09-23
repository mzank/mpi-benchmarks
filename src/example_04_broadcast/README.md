# MPI Broadcast Benchmark

## Overview

This example implements an MPI broadcast benchmark to measure broadcast communication performance where rank 0 broadcasts data to all other ranks using `MPI_Bcast`.

The benchmark performs the following steps:
1.  **Environment Check**: Prints CPU affinity and SLURM environment information for each rank.
2.  **Warmup**: Executes 20 iterations of the broadcast exchange to prime the network and libraries.
3.  **Measurement**: Iterates through message sizes from 1 byte to 16 MiB (powers of 2).
    *   Small messages (≤ 8 KiB) use 1,000 iterations.
    *   Large messages (> 8 KiB) use 100 iterations.
4.  **Verification**: Calculates a checksum of the received data to ensure integrity.
5.  **Reporting**: Calculates and displays the average per-iteration time (μs) of the slowest MPI rank and the effective bandwidth (MiB/s) as aggregate data volume delivered to non-root ranks per unit time.

## Requirements

- At least 2 MPI ranks are required.

## Local Execution

To run the benchmark locally on your machine using 4 processes:

```bash
mpirun -n 4 ./build/bin/example_04_broadcast
```

## Expected Output

The benchmark outputs a table with the following columns:
- **Size(Bytes)**: The message size in bytes.
- **Broadcast(us)**: The average per-iteration time of the slowest MPI rank in microseconds.
- **EffectiveBW(MiB/s)**: The aggregate data volume delivered to non-root ranks per unit time in MiB/s.

## Verification

At the end of each message size iteration, rank 0 gathers checksums from all ranks to verify that the data was correctly received from rank 0. For the maximum message size, detailed verification data per rank is printed.
