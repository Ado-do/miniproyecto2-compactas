add_executable(test_components test_components.cpp)
target_link_libraries(test_components PRIVATE miniproyecto2)

add_test(
    NAME components_datasets
    COMMAND test_components "${CMAKE_SOURCE_DIR}"
)
