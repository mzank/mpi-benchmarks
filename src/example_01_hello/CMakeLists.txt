add_executable(example_01_hello main.c)

target_link_libraries(example_01_hello PRIVATE MPI::MPI_C)
