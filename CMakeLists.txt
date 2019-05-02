cmake_minimum_required(VERSION 3.13)
project(canon)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

include_directories(./core)
include_directories(./HTTP)
add_executable(canon core/Connection.c core/mutex.h)