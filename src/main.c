#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>

#include "wifi.h"
#include "wifi_pswd.h"

#define SERVER_IP   "192.168.5.29"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

static const char *messages[] = {
    "Hello, Server!",
    "How are you?",
    "Socket demo working.",
    "Goodbye!",
    NULL
};

int main(void)
{
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int sock_fd;
    int ret;

    printk("TCP ECHO CLIENT DEMO\n");

    my_wifi_init();
    printk("Connecting to WiFi...\n");

    if (wifi_connect(BITCRAZE_SSID, BITCRAZE_PASSWORD)) {
        printk("Failed to connect to WiFi\n");
        return -1;
    }

    wifi_wait_for_ip_addr();

    sock_fd = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd < 0) {
        printk("Error (%d): Could not create socket\n", errno);
        wifi_disconnect();
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    ret = zsock_inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    if (ret != 1) {
        printk("Error: Invalid SERVER_IP (%s)\n", SERVER_IP);
        zsock_close(sock_fd);
        wifi_disconnect();
        return -1;
    }

    ret = zsock_connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printk("Error (%d): Could not connect to server\n", errno);
        zsock_close(sock_fd);
        wifi_disconnect();
        return -1;
    }

    printk("[Client] Connected to %s:%d\n", SERVER_IP, SERVER_PORT);

    for (int i = 0; messages[i] != NULL; i++) {
        ret = zsock_send(sock_fd, messages[i], strlen(messages[i]), 0);
        if (ret < 0) {
            printk("Error (%d): send failed\n", errno);
            break;
        }

        printk("[Client] Sent:     %s\n", messages[i]);

        ret = zsock_recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (ret <= 0) {
            if (ret == 0) {
                printk("[Client] Server closed the connection.\n");
            } else {
                printk("Error (%d): recv failed\n", errno);
            }
            break;
        }

        buffer[ret] = '\0';
        printk("[Client] Received: %s\n", buffer);
    }

    zsock_close(sock_fd);
    printk("[Client] Closed.\n");

    wifi_disconnect();
    return 0;
}