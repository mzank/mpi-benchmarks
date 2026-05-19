cmake_minimum_required(VERSION 3.20)

project(mpi_benchmarks VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(MPI REQUIRED)

option(ENABLE_DOCS "Build documentation" ON)

if(ENABLE_DOCS)
    find_package(Doxygen REQUIRED)

    set(DOXYGEN_PROJECT_NAME "MPI Benchmarks")
    set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs")

    set(DOXYGEN_INPUT_DIRS
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/README.md"
    )

    set(DOXYGEN_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")

    include(cmake/Doxygen.cmake)
endif()

add_subdirectory(src/example_01_hello)
