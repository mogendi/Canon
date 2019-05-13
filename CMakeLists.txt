cmake_minimum_required(VERSION 3.13)
project(canon)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_library(engine ./core/Engine/canon_tpool.c)

include_directories(./core)
include_directories(./HTTP)
add_executable(canon ./HTTP/HTTPmain.c core/DataStructures/bst.h)

target_link_libraries(canon engine)
