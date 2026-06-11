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

## Running with SLURM

Multiple SLURM scripts are provided to test different communication scenarios. Logs and error reports are automatically saved in the `logs/` directory at the project root.

### 1. Intra-node (Single Node)

Measure communication performance within a single physical node.

**Standard Bindings:**
```bash
sbatch src/example_03_ring/run_1node.slurm
```
This script tests various CPU binding options (Default, Core, Socket, NUMA LDOM, Rank-aware NUMA) to show the impact of process placement on shared-memory ring performance.

**Scaling Test:**
```bash
sbatch src/example_03_ring/run_1node_scaling.slurm
```
This script evaluates how the aggregate ring bandwidth scales as more ranks are added on a single node.

### 2. Inter-node (Multi-Node)

Measure aggregate network performance across multiple physical nodes.

**Two Nodes:**
```bash
sbatch src/example_03_ring/run_2nodes.slurm
```

**Four Nodes:**
```bash
sbatch src/example_03_ring/run_4nodes.slurm
```

These scripts ensure tasks are distributed across nodes to evaluate the cluster interconnect performance in a ring topology.

## Expected Output

The benchmark outputs a table with the following columns:
- **Size(Bytes)**: The message size in bytes.
- **Exchange(us)**: The average time for one full ring exchange in microseconds.
- **AggregateRingBW(MiB/s)**: The achieved aggregate bandwidth across the ring in MiB/s.

## Verification

At the end of each message size iteration, rank 0 gathers checksums from all ranks to verify that the data was correctly received from the respective predecessors. For the maximum message size, detailed verification data per rank is printed.

## Example Results

Benchmark results for the AMD Instinct MI300A architecture are available in the `logs/` directory:

- **[1-Node Results](logs/1node/)**: Intra-node performance using standard CPU binding strategies (Cores, Sockets, NUMA) on a single node with 4 AMD MI300A.
- **[1-Node Scaling Results](logs/1node_scaling/)**: Scaling behavior on a single node as the number of ranks increases.
- **[2-Node Results](logs/2nodes/)**: Inter-node performance across two nodes with 4 AMD MI300A.
- **[4-Node Results](logs/4nodes/)**: Inter-node performance across four nodes with 4 AMD MI300A.
