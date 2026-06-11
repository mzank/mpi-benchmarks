#!/bin/bash
#SBATCH --job-name=ring_1node_scaling
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=00:05:00
#SBATCH --output=logs/%x_%j.out
#SBATCH --error=logs/%x_%j.err

set -euo pipefail

PROJECT_ROOT="${SLURM_SUBMIT_DIR}"
BINARY="${PROJECT_ROOT}/build/bin/example_03_ring"

mkdir -p "${PROJECT_ROOT}/logs"

if [ -f "${PROJECT_ROOT}/config/cluster.conf" ]; then
    source "${PROJECT_ROOT}/config/cluster.conf"
fi

if [ ! -x "${BINARY}" ]; then
    echo "Error: Binary not found. Run build step first."
    exit 1
fi

PHYSICAL_CORES=$((SLURM_CPUS_ON_NODE / 2))
NUMA_DOMAINS=$(srun -N1 -n1 numactl --hardware | awk '/available:/ {print $2}')
CORES_PER_NUMA=$((PHYSICAL_CORES / NUMA_DOMAINS))

RANKS_LIST=()
ranks=2
while [ "${ranks}" -le "${CORES_PER_NUMA}" ]
do
    RANKS_LIST+=("${ranks}")
    ranks=$((ranks * 2))
done

for ((n=1; n<=NUMA_DOMAINS; n++))
do
    ranks=$((n * CORES_PER_NUMA))
    
    if [ "${ranks}" -gt "${RANKS_LIST[-1]}" ]
    then
        RANKS_LIST+=("${ranks}")
    fi
done

echo "Nodes               : ${SLURM_JOB_NUM_NODES}"
echo "NUMA domains        : ${NUMA_DOMAINS}"
echo "Physical cores      : ${PHYSICAL_CORES}"
echo "Physical cores/NUMA : ${CORES_PER_NUMA}"
echo "RANKS_LIST          : ${RANKS_LIST[*]}"
echo "Binary              : ${BINARY}"

echo "=== NUMA topology ==="
srun -N 1 -n 1 bash -c 'hostname; numactl --hardware'

for RANKS in "${RANKS_LIST[@]}"
do
    echo
    echo "===================================================="
    echo " MPI Ranks: ${RANKS}"
    echo "===================================================="
    
    srun -n "${RANKS}" --cpu-bind=verbose --hint=nomultithread --mem-bind=local "${BINARY}"
    
done

echo ""
echo "========================================"
echo " DONE"
echo "========================================"
