#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>

#define PORT        8080
#define BUFFER_SIZE 1024

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[BUFFER_SIZE];

    /* Create TCP socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Allow address reuse so we can restart quickly */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* Bind to all interfaces on PORT */
    memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* Listen for incoming connections */
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* Print all LAN IPv4 addresses so the client knows where to connect */
    {
        struct ifaddrs *ifaddr, *ifa;
        char ip_str[INET_ADDRSTRLEN];
        if (getifaddrs(&ifaddr) == 0) {
            printf("[Server] Listening on port %d — reachable at:\n", PORT);
            for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                if (ifa->ifa_addr == NULL) continue;
                if (ifa->ifa_addr->sa_family != AF_INET) continue;
                if (ifa->ifa_flags & IFF_LOOPBACK) continue;
                inet_ntop(AF_INET,
                          &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr,
                          ip_str, sizeof(ip_str));
                printf("[Server]   %s  (iface: %s)\n", ip_str, ifa->ifa_name);
            }
            freeifaddrs(ifaddr);
        } else {
            printf("[Server] Listening on port %d...\n", PORT);
        }
    }

    /* Accept a single client */
    client_fd = accept(server_fd, (struct sockaddr *)&address, &addr_len);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &address.sin_addr, client_ip, sizeof(client_ip));
    printf("[Server] Client connected from %s\n", client_ip);

    /* Exchange messages in a loop */
    for (;;) {
        /* Receive message from client */
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) {
            if (bytes == 0)
                printf("[Server] Client disconnected.\n");
            else
                perror("recv");
            break;
        }
        printf("[Server] Received: %s\n", buffer);

        /* Echo back with a prefix */
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "Echo: %s", buffer);
        if (send(client_fd, response, strlen(response), 0) < 0) {
            perror("send");
            break;
        }
        printf("[Server] Sent:     %s\n", response);
    }

    close(client_fd);
    close(server_fd);
    printf("[Server] Closed.\n");
    return 0;
}
