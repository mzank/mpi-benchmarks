add_executable(example_04_broadcast main.c)

target_link_libraries(example_04_broadcast PRIVATE MPI::MPI_C)
