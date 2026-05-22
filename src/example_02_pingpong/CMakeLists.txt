add_executable(example_02_pingpong main.c)

target_link_libraries(example_02_pingpong PRIVATE MPI::MPI_C)
