# MPI Benchmarks

A collection of MPI benchmark and example applications implemented in C11. This project focuses on developing, building, documenting, and executing MPI applications on both local systems and HPC clusters using SLURM.

---

## Included Examples

### MPI Hello World

Path:

```text
src/example_01_hello/main.c
```

A minimal MPI example demonstrating process initialization, rank identification, and basic error handling. See the [Hello World README](src/example_01_hello/README.md) for detailed instructions.

### MPI Ping-Pong Benchmark

Path:

```text
src/example_02_pingpong/main.c
```

A point-to-point communication benchmark measuring latency and bandwidth between two MPI ranks. See the [Ping-Pong README](src/example_02_pingpong/README.md) for detailed instructions.

---

## Requirements

To build and run this project on Linux, you need:

- **C Compiler**
  - GCC, Clang, or another C11-compatible compiler

- **MPI Implementation**
  - OpenMPI
  - MPICH
  - Intel MPI
  - or compatible implementation

- **Build System**
  - CMake 3.20 or newer

- **Documentation Generator** (optional)
  - Doxygen

---

## Building the Project

### Using CMake Directly

Configure the project:

```bash
cmake -S . -B build
```

Build all targets:

```bash
cmake --build build --parallel
```

After building, the compiled binaries are placed in the `build/bin` directory.

### Using the Helper Script

A build script is provided:

```bash
./scripts/build.sh
```

---

## Documentation

If Doxygen is installed, HTML documentation can be generated with:

```bash
cmake -S . -B build
cmake --build build --target docs
```

The generated documentation is located at:

```text
build/docs/html/index.html
```

---

## Running the Examples

In place of all examples, it is shown how to run the Hello World example locally and on an HPC cluster. See the example's README for detailed instructions.

### Local Execution

Run the Hello World example locally with, e.g., four MPI processes:

```bash
mpirun -n 4 ./build/bin/example_01_hello
```

Depending on your MPI implementation, `mpiexec` may also be used:

```bash
mpiexec -n 4 ./build/bin/example_01_hello
```

---

## Running on an HPC Cluster (SLURM)

A SLURM submission script is provided for each example. Logs and error reports are automatically saved in the `logs/` directory at the project root.

Submit the Hello World example with:

```bash
sbatch src/example_01_hello/run.slurm
```

For the Ping-Pong benchmark, multiple scripts are provided for different scenarios (intra-node vs inter-node). See the [Ping-Pong README](src/example_02_pingpong/README.md) for details.

Adjust the SLURM resource parameters in the script to match your cluster environment, such as:

- partition
- number of nodes
- number of tasks
- wall time
- memory limits

---

## Cluster Configuration

### Environment Modules

Many HPC systems require loading compiler and MPI modules before building or running, e.g.:

```bash
module load gcc openmpi
```

Adjust the modules according to your cluster environment.

---

## Cluster Configuration File

A template for cluster-specific settings is provided:

```text
config/cluster.conf.example
```

Create your local cluster configuration with:

```bash
cp config/cluster.conf.example config/cluster.conf
```

This configuration may contain:

- module load commands
- environment variables
- compiler settings
- MPI runtime configuration

If `config/cluster.conf` exists, helper scripts and SLURM scripts will automatically source it.

---

## License

This project is licensed under the Apache License 2.0. See [LICENSE](LICENSE) for details.

You can also view the license here: https://www.apache.org/licenses/LICENSE-2.0

---