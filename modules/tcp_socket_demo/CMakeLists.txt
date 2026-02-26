if(CONFIG_TCP_SOCKET_DEMO)

    zephyr_include_directories(.)

    zephyr_library_sources(tcp_socket.c)

endif()
