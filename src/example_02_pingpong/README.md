# MPI Ping-Pong Benchmark

This example implements a standard MPI ping-pong benchmark to measure point-to-point communication performance between two MPI ranks.

## Overview

The benchmark performs the following steps:
1.  **Environment Check**: Prints CPU affinity and SLURM environment information for each rank.
2.  **Warmup**: Executes 20 iterations of `MPI_Sendrecv` to prime the network and libraries.
3.  **Measurement**: Iterates through message sizes from 1 byte to 16 MiB (powers of 2).
    *   Small messages (≤ 8 KiB) use 1,000 iterations.
    *   Large messages (> 8 KiB) use 100 iterations.
4.  **Reporting**: Calculates and displays the average one-way latency (μs) and effective bandwidth (MiB/s).

## Requirements

- Exactly 2 MPI ranks are required.

## Local Execution

To run the benchmark locally on your machine using 2 processes:

```bash
mpirun -n 2 ./build/bin/example_02_pingpong
```

## Running with SLURM

Two SLURM scripts are provided to test different communication scenarios. Logs and error reports are automatically saved in the `logs/` directory at the project root.

### 1. Intra-node (Single Node)

Measure communication performance between two processes running on the same physical node.

```bash
sbatch src/example_02_pingpong/run_1node.slurm
```

This script tests various CPU binding options (Default, Core, Socket, NUMA LDOM, Rank-aware NUMA) to show the impact of process placement on shared-memory performance.

### 2. Inter-node (Two Nodes)

Measure network performance between two processes running on different physical nodes.

```bash
sbatch src/example_02_pingpong/run_2nodes.slurm
```

This script ensures one task is placed per node to force communication across the cluster interconnect.

## Output

The benchmark outputs a table with the following columns:
- **Size(Bytes)**: The message size in bytes.
- **Latency(us)**: The average one-way latency in microseconds.
- **EffectiveBW(MiB/s)**: The achieved bandwidth in MiB/s.
