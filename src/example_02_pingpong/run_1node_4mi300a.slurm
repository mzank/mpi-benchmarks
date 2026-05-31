#!/bin/bash
#SBATCH --job-name=pingpong_1node_4mi300a
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
echo " 1) Same socket, different cores"
echo "=============================="
srun -n 2 --cpu-bind=verbose,map_cpu:0,1 --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 2) Same socket, SMT siblings"
echo "=============================="
srun -n 2 --cpu-bind=verbose,map_cpu:0,96 --hint=multithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 3) Different NUMA domains: NUMA 0 <-> NUMA 1"
echo "=============================="
srun -n 2 --cpu-bind=verbose,map_cpu:0,24 --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 4) Different NUMA domains: NUMA 0 <-> NUMA 2"
echo "=============================="
srun -n 2 --cpu-bind=verbose,map_cpu:0,48 --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 5) Different NUMA domains: NUMA 0 <-> NUMA 3"
echo "=============================="
srun -n 2 --cpu-bind=verbose,map_cpu:0,72 --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 6) Different NUMA domains: NUMA 1 <-> NUMA 2"
echo "=============================="
srun -n 2 --cpu-bind=verbose,map_cpu:24,48 --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 7) Different NUMA domains: NUMA 1 <-> NUMA 3"
echo "=============================="
srun -n 2 --cpu-bind=verbose,map_cpu:24,72 --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "=============================="
echo " 8) Different NUMA domains: NUMA 2 <-> NUMA 3"
echo "=============================="
srun -n 2 --cpu-bind=verbose,map_cpu:48,72 --hint=nomultithread --mem-bind=local "${BINARY}"

echo ""
echo "========================================"
echo " DONE"
echo "========================================"
