cmake_minimum_required(VERSION 3.20.0)

set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/modules/wifi_utilities")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(demo_wifi)

target_include_directories(app PRIVATE secret)
target_include_directories(app PRIVATE src/communication)


target_sources(app PRIVATE 
    src/main.c
    src/communication/tcp_socket.c
)