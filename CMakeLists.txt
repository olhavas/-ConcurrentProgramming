cmake_minimum_required(VERSION 3.15)
project(ConcurrentProgramming)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(ConcurrentProgramming ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} Threads::Threads)
