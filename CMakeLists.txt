cmake_minimum_required(VERSION 3.15)
project(miniproyecto2 VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# k2-tree Basic (C)
add_subdirectory(material-base/k2tree_basic_v0.1)

# sdsl-lite-turan (C++) — pemb.hpp / Graph.hpp
set(GENERATE_DOC OFF CACHE BOOL "" FORCE)
set(CODE_COVERAGE OFF CACHE BOOL "" FORCE)
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory(material-base/sdsl-lite-turan)

# Project library (MP2 deliverable code)
add_library(miniproyecto2 STATIC
    src/adjacency_list.cpp
    src/graph_io.cpp
)

target_include_directories(miniproyecto2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(miniproyecto2 PUBLIC sdsl k2tree_basic)

add_executable(mp2_smoke src/smoke_main.cpp)
target_link_libraries(mp2_smoke PRIVATE miniproyecto2)

option(BUILD_MP2_TESTS "Build MP2 tests" ON)
if(BUILD_MP2_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

option(BUILD_MP2_BENCHMARKS "Build MP2 benchmarks" ON)
if(BUILD_MP2_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
