# MPI Hello World

## Overview

A minimal MPI example demonstrating process initialization, rank identification, and basic error handling.

## Requirements

- One or more MPI ranks.

## Local Execution

To run the example locally with, e.g., 4 processes:

```bash
mpirun -n 4 ./build/bin/example_01_hello
```

## Running with SLURM

A SLURM script is provided to run the example on a cluster node. Logs and error reports are automatically saved in the `logs/` directory at the project root.

```bash
sbatch src/example_01_hello/run.slurm
```

The script is configured by default to use 1 node and 4 tasks.

## Expected Output

Format:
```text
[rank 0/4] Hello from MPI process.
[rank 1/4] Hello from MPI process.
[rank 2/4] Hello from MPI process.
[rank 3/4] Hello from MPI process.
```
