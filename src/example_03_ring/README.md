# MPI Ring Benchmark

## Overview

This example implements an MPI ring benchmark to measure aggregate communication performance in a ring pattern. Each rank sends data to its successor and receives from its predecessor.

The benchmark performs the following steps:
1.  **Environment Check**: Prints CPU affinity and SLURM environment information for each rank.
2.  **Warmup**: Executes 20 iterations of the ring exchange to prime the network and libraries.
3.  **Measurement**: Iterates through message sizes from 1 byte to 16 MiB (powers of 2).
    *   Small messages (≤ 8 KiB) use 1,000 iterations.
    *   Large messages (> 8 KiB) use 100 iterations.
4.  **Verification**: Calculates a checksum of the received data to ensure integrity.
5.  **Reporting**: Calculates and displays the average exchange time (μs) and aggregate ring bandwidth (MiB/s).

## Requirements

- At least 2 MPI ranks are required.

## Local Execution

To run the benchmark locally on your machine using 4 processes:

```bash
mpirun -n 4 ./build/bin/example_03_ring
```

## Expected Output

The benchmark outputs a table with the following columns:
- **Size(Bytes)**: The message size in bytes.
- **Exchange(us)**: The average time for one full ring exchange in microseconds.
- **AggregateRingBW(MiB/s)**: The achieved aggregate bandwidth across the ring in MiB/s.

## Verification

At the end of each message size iteration, rank 0 gathers checksums from all ranks to verify that the data was correctly received from the respective predecessors. For the maximum message size, detailed verification data per rank is printed.
