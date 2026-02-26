cmake_minimum_required(VERSION 3.10)
project(SocketDemo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Server executable
add_executable(server server.c)

# Client executable
add_executable(client client.c)
