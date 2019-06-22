cmake_minimum_required(VERSION 3.13)
project(canon)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

file(GLOB SRC1 ./core/DataStructures/*.c)
file(GLOB SRC2 ./core/Engine/*.c)
file(GLOB SRC3 ./core/FileOps/*.c)
file(GLOB SRC4 ./core/cacher/*.c)
file(GLOB SRC5 ./core/*.c)
file(GLOB SRC6 http/*.c)

file(GLOB INC1 ./core/DataStructures/*.h)
file(GLOB INC2 ./core/Engine/*.h)
file(GLOB INC3 ./core/FileOps/*.h)
file(GLOB INC4 ./core/*.h)
file(GLOB INC5 http/*.h)
file(GLOB INC6 ./core/cacher/*.h)

include_directories(./core)
include_directories(http)
add_executable(canon http/HTTPmain.c ${SRC1} ${SRC2} ${SRC3} ${SRC4} ${SRC5} ${SRC6}
                                       ${INC1} ${INC2} ${INC3} ${INC4} ${INC5} ${INC6})


