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

## Running with SLURM

Multiple SLURM scripts are provided to test different communication scenarios. Logs and error reports are automatically saved in the `logs/` directory at the project root.

### 1. Intra-node (Single Node)

Measure communication performance within a single physical node.

**Standard Bindings:**
```bash
sbatch src/example_04_broadcast/run_1node.slurm
```
This script tests various CPU binding options (Default, Core, Socket, NUMA LDOM, Rank-aware NUMA) to show the impact of process placement on shared-memory broadcast performance.

**Scaling Test:**
```bash
sbatch src/example_04_broadcast/run_1node_scaling.slurm
```
This script evaluates how the broadcast performance scales as more ranks are added on a single node.

### 2. Inter-node (Multi-Node)

Measure network performance across multiple physical nodes.

**Two Nodes:**
```bash
sbatch src/example_04_broadcast/run_2nodes.slurm
```

**Four Nodes:**
```bash
sbatch src/example_04_broadcast/run_4nodes.slurm
```

These scripts ensure tasks are distributed across nodes to evaluate the cluster interconnect performance for broadcast communication.

## Expected Output

The benchmark outputs a table with the following columns:
- **Size(Bytes)**: The message size in bytes.
- **Broadcast(us)**: The average per-iteration time of the slowest MPI rank in microseconds.
- **EffectiveBW(MiB/s)**: The aggregate data volume delivered to non-root ranks per unit time in MiB/s.

## Verification

At the end of each message size iteration, rank 0 gathers checksums from all ranks to verify that the data was correctly received from rank 0. For the maximum message size, detailed verification data per rank is printed.

## Example Results

Benchmark results for the AMD Instinct MI300A architecture are available in the `logs/` directory:

- **[1-Node Results](logs/1node/)**: Intra-node performance using standard CPU binding strategies (Cores, Sockets, NUMA) on a single node with 4 AMD MI300A.
- **[1-Node Scaling Results](logs/1node_scaling/)**: Scaling behavior on a single node with 4 AMD MI300A as the number of ranks increases.
- **[2-Node Results](logs/2nodes/)**: Inter-node performance across two nodes with 4 AMD MI300A.
- **[4-Node Results](logs/4nodes/)**: Inter-node performance across four nodes with 4 AMD MI300A.
