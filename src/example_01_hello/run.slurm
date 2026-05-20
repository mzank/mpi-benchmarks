#!/bin/bash
#SBATCH --job-name=mpi_hello_world
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --exclusive
#SBATCH --time=00:05:00
#SBATCH --output=logs/%x_%j.out
#SBATCH --error=logs/%x_%j.err

set -euo pipefail

PROJECT_ROOT="${SLURM_SUBMIT_DIR}"
BINARY="${PROJECT_ROOT}/build/bin/example_01_hello"

mkdir -p "${PROJECT_ROOT}/logs"

if [ -f "${PROJECT_ROOT}/config/cluster.conf" ]; then
    source "${PROJECT_ROOT}/config/cluster.conf"
fi

if [ ! -x "${BINARY}" ]; then
    echo "Error: Binary not found. Run build step first."
    exit 1
fi

echo "Running on host: $(hostname)"
echo "Tasks: ${SLURM_NTASKS}"
echo "Nodes: ${SLURM_JOB_NUM_NODES}"
echo "Binary: ${BINARY}"

srun "${BINARY}"
