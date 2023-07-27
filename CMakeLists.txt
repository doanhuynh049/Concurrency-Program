cmake_minimum_required(VERSION 3.8)

project(Concurrency CXX)

# Set the C/C++ standard to C11 C++17
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(my_executable
    src/main.cpp
    src/session1/fork.cpp
    src/session1/named-pipe.cpp
    src/session1/condition_var.cpp
    src/session1/mutex_multithreading.cpp
    src/session1/recursive_mutex.cpp
    src/session1/multithreading.cpp
    src/session2/multiprocessing.cpp)
target_include_directories(my_executable PRIVATE ${CMAKE_CUTTENT_SOURCE_DIR}/include)

target_sources(my_executable PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/session1/fork.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/session1/multithreading.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/session1/mutex_multithreading.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/session1/named-pipe.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/session1/recursive_mutex.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/session1/condition_var.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/session2/multiprocessing.h)

set_target_properties(my_executable PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

# add custom commadn to run the executable after building it
add_custom_command(TARGET my_executable POST_BUILD
    COMMENT "Running my_executable"
    COMMAND ${PROJECT_SOURCE_DIR}/build/bin/my_executable
    COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_SOURCE_DIR}/build/bin/my_executable
)