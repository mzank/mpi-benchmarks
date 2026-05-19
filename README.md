# MPI Benchmarks
My examples using MPI in C.

## Build Instructions

```bash
# Remove the previous build directory
rm -rf build

# Configure the project with CMake
cmake -S . -B build

# Build all targets
cmake --build build

# Generate the Doxygen documentation
cmake --build build --target docs
```

---
