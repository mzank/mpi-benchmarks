#!/bin/bash
#SBATCH --job-name=pingpong_1node
#SBATCH --nodes=1
#SBATCH --ntasks=2
#SBATCH --exclusive
#SBATCH --time=00:05:00
#SBATCH --output=logs/%x_%j.out
#SBATCH --error=logs/%x_%j.err

set -euo pipefail

PROJECT_ROOT="${SLURM_SUBMIT_DIR}"
BINARY="${PROJECT_ROOT}/build/bin/example_02_pingpong"

mkdir -p "${PROJECT_ROOT}/logs"

if [ -f "${PROJECT_ROOT}/config/cluster.conf" ]; then
    source "${PROJECT_ROOT}/config/cluster.conf"
fi

if [ ! -x "${BINARY}" ]; then
    echo "Error: Binary not found. Run build step first."
    exit 1
fi

echo "Tasks: ${SLURM_NTASKS}"
echo "Nodes: ${SLURM_JOB_NUM_NODES}"
echo "Binary: ${BINARY}"

echo "=== NUMA topology ==="
srun -N 1 -n 1 bash -c 'hostname; numactl --hardware'

echo ""
echo "=============================="
echo " 1) DEFAULT"
echo "=============================="
srun -n 2 --cpu-bind=verbose --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 2) CORE binding"
echo "=============================="
srun -n 2 --cpu-bind=verbose,cores --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 3) SOCKET binding"
echo "=============================="
srun -n 2 --cpu-bind=verbose,sockets --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 4) NUMA (LDOM) binding"
echo "=============================="
srun -n 2 --cpu-bind=verbose,ldoms --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 5) RANK-AWARE NUMA"
echo "=============================="
srun -n 2 --cpu-bind=verbose,rank_ldom --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "========================================"
echo " DONE"
echo "========================================"
