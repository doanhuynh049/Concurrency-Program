cmake_minimum_required(VERSION 3.8)

project(Concurrency CXX)

# Set the C/C++ standard to C11 C++17
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(my_executable
    src/main.cpp
    src/fork.cpp
    src/named-pipe.cpp
    src/mutex_multithreading.cpp
    src/multithreading.cpp)
target_include_directories(my_executable PRIVATE ${CMAKE_CUTTENT_SOURCE_DIR}/include)

target_sources(my_executable PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fork.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/multithreading.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mutex_multithreading.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/named-pipe.h)

set_target_properties(my_executable PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

# add custom commadn to run the executable after building it
add_custom_command(TARGET my_executable POST_BUILD
    COMMENT "Running my_executable"
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/build/bin/my_executable
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/build/bin/my_executable
)