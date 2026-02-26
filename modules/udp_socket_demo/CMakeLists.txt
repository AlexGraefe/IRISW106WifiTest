if(CONFIG_UDP_SOCKET_DEMO)

    zephyr_include_directories(.)

    zephyr_library_sources(udp_socket.c)

endif()
