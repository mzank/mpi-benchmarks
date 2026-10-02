# MPI Ring Benchmark

## Overview

This example implements an MPI ring benchmark to measure aggregate communication performance in a ring pattern. Each rank sends data to its successor and receives from its predecessor using non-blocking communication (`MPI_Isend`/`MPI_Irecv`).

The benchmark performs the following steps:
1.  **Environment Check**: Prints CPU affinity and SLURM environment information for each rank.
2.  **Warmup**: Executes 20 iterations of the ring exchange to prime the network and libraries.
3.  **Measurement**: Iterates through message sizes from 1 byte to 16 MiB (powers of 2).
    *   Small messages (≤ 8 KiB) use 1,000 iterations.
    *   Large messages (> 8 KiB) use 100 iterations.
4.  **Verification**: Calculates a checksum of the received data to ensure integrity.
5.  **Reporting**: Calculates and displays the average exchange time (μs) and aggregate ring bandwidth (MiB/s), timed on rank 0.

## Requirements

- At least 2 MPI ranks are required.

## Local Execution

To run the benchmark locally on your machine using 4 processes:

```bash
mpirun -n 4 ./build/bin/example_03_ring
```

## Running with SLURM

Multiple SLURM scripts are provided to test different communication scenarios. Logs and error reports are written to the `logs/` directory at the project root. This is untracked scratch output: it is ignored by version control and can be deleted at any time.

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

## Configuration Output

Before the result table, the benchmark prints a configuration header, preceded by the per-rank environment information (processor name, SLURM IDs, and CPU affinity):

```text
# MPI Ring Benchmark
#
# Configuration
#   MPI ranks        : 4
#   Max message size : 16777216 bytes
#   Warmup iterations: 20
#   Small iterations : 1000
#   Large iterations : 100
#   MPI_Wtick        : 1.000000000e-09 seconds
#
# Metrics
#   Exchange         : full ring exchange time
#   AggregateRingBW  : aggregate bandwidth across the ring
```

- **MPI ranks**: Number of ranks in `MPI_COMM_WORLD`.
- **Max message size**: Upper bound of the measured message sizes (16 MiB).
- **Warmup / Small / Large iterations**: Values of `WARMUP`, `ITER_SMALL`, and `ITER_LARGE`.
- **MPI_Wtick**: Resolution of the MPI wall-clock timer, useful when interpreting sub-microsecond measurements.

## Expected Output

The benchmark outputs a table with the following columns:
- **Size(Bytes)**: The message size in bytes.
- **Exchange(us)**: The average time for one full ring exchange in microseconds, timed on rank 0. Each exchange completes both the send and the receive via `MPI_Waitall`, so every iteration re-synchronizes all ranks and no rank can run more than one iteration ahead. The timing therefore reflects the ring-wide critical path. Per-iteration scheduler jitter on individual ranks is not captured, as no cross-rank reduction is performed.
- **AggregateRingBW(MiB/s)**: The achieved aggregate bandwidth across the ring in MiB/s, computed as the total volume moved around the ring (`msg_size * size`) divided by the exchange time above.

## Verification

At the end of each message size iteration, rank 0 gathers two values from all ranks: the locally computed bandwidth and a checksum. The checksum confirms that the data was correctly received from the respective predecessors. For the maximum message size, the per-rank bandwidth and checksum are printed as detailed verification data.

## Example Results

Reference results for the AMD Instinct MI300A architecture are committed under `src/example_03_ring/logs/`, one subdirectory per SLURM script. Unlike the scratch output written to the project root `logs/`, these results are tracked in version control and serve as the published baseline for each scenario:

- **[1-Node Results](logs/1node/)**: Intra-node performance using standard CPU binding strategies (Cores, Sockets, NUMA) on a single node with 4 AMD MI300A.
- **[1-Node Scaling Results](logs/1node_scaling/)**: Scaling behavior on a single node with 4 AMD MI300A as the number of ranks increases.
- **[2-Node Results](logs/2nodes/)**: Inter-node performance across two nodes with 4 AMD MI300A.
- **[4-Node Results](logs/4nodes/)**: Inter-node performance across four nodes with 4 AMD MI300A.

Each subdirectory holds the result pair `<jobname>_4mi300a.out`/`.err`, together with one topology capture per node: `<jobname>_topo_<task>_4mi300a.out`/`.err`, recording the hostname and `numactl --hardware` report. The multi-node scripts redirect the topology capture to its own file, while the single-node scripts print it inline in the main log, so topology files appear only in the 2-Node and 4-Node Results directories.
