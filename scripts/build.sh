#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

if [ -f "${PROJECT_ROOT}/config/cluster.conf" ]; then
    source "${PROJECT_ROOT}/config/cluster.conf"
fi

cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}" --parallel

mkdir -p "${PROJECT_ROOT}/logs"
