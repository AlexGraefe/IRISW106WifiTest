#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#define SERVER_IP   "192.168.5.29"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

#include <stdbool.h>

#include <zephyr/net/socket.h>

#define SOCKET_THREAD_PRIORITY 10

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)

typedef enum {
	COMM_WIFI_CONNECTING,
	COMM_WAITING_FOR_IP,
	COMM_CONNECTING_TO_SERVER,
	COMM_SENDING_MESSAGES,
	COMM_FAILURE,
	COMM_CLEANUP,
	COMM_DONE,
} communication_state_t;

typedef struct {
	struct sockaddr_in server_addr;
	char buffer[BUFFER_SIZE];
	int sock_fd;
	bool wifi_connected;
	bool socket_open;
	int exit_code;
	communication_state_t failure_from_state;
} communication_context_t;

int run_tcp_socket_demo(void);

#endif /* TCP_SOCKET_H */
