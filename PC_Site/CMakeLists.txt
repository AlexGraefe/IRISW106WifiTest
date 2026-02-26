cmake_minimum_required(VERSION 3.10)
project(SocketDemo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Server executable
add_executable(tcp_socket_server tcp_socket_server.c)

# Server executable
add_executable(udp_socket_server udp_socket_server.c)

# Client executable
add_executable(client client.c)

# Client executable
add_executable(udp_client udp_socket_client.c)
