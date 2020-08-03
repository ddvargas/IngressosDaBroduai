cmake_minimum_required(VERSION 3.16)
project(Trab2SistemasOperacionais C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(ingressos main.c)

target_link_libraries(ingressos PRIVATE Threads::Threads)