cmake_minimum_required(VERSION 3.10)
project(OS_Lab3_FileMapping_Windows)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(os_lib
    src/os_win.cpp
)
target_include_directories(os_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(parent src/parent.cpp)
add_executable(child1 src/child1.cpp)
add_executable(child2 src/child2.cpp)

target_link_libraries(parent PRIVATE os_lib)
target_link_libraries(child1 PRIVATE os_lib)
target_link_libraries(child2 PRIVATE os_lib)
