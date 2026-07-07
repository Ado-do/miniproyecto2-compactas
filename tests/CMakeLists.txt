add_executable(test_components test_components.cpp)
target_link_libraries(test_components PRIVATE miniproyecto2)

add_test(
    NAME components_datasets
    COMMAND test_components "${CMAKE_SOURCE_DIR}"
)

add_executable(test_k2_graph_io test_k2_graph_io.cpp)
target_link_libraries(test_k2_graph_io PRIVATE miniproyecto2)

add_test(
    NAME k2_graph_io
    COMMAND test_k2_graph_io "${CMAKE_SOURCE_DIR}"
)

add_executable(test_adjacency_list test_adjacency_list.cpp)
target_link_libraries(test_adjacency_list PRIVATE miniproyecto2)

add_test(
    NAME adjacency_list
    COMMAND test_adjacency_list "${CMAKE_SOURCE_DIR}"
)
