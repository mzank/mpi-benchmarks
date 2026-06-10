add_executable(example_03_ring main.c)

target_link_libraries(example_03_ring PRIVATE MPI::MPI_C)
