cmake_minimum_required(VERSION 3.15)

add_library(k2tree_basic STATIC
    basic.c
    bitrankw32int.c
    kTree.c
)

target_include_directories(k2tree_basic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(k2tree_basic PRIVATE -Wall -O2 -DNDEBUG)
target_link_libraries(k2tree_basic PUBLIC m)

add_executable(build_tree buildk2tree.c)
target_link_libraries(build_tree PRIVATE k2tree_basic)
