add_executable(mp2_bench bench_quick.cpp)
target_link_libraries(mp2_bench PRIVATE miniproyecto2)
set_target_properties(mp2_bench PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_executable(mp2_bench_k2_tune bench_k2_tune.cpp)
target_link_libraries(mp2_bench_k2_tune PRIVATE miniproyecto2)
set_target_properties(mp2_bench_k2_tune PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
