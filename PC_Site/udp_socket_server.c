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
    struct sockaddr_in si_me, si_other;
    int slen = sizeof(si_other);
    char buffer[BUFFER_SIZE];

    /* Create TCP socket */
    server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // SOCK_STREAM for TCP
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
    memset(&si_me, 0, sizeof(si_me));
    si_me.sin_family      = AF_INET;
    si_me.sin_addr.s_addr = INADDR_ANY;
    si_me.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&si_me, sizeof(si_me)) < 0) {
        perror("bind");
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


    /* Exchange messages in a loop */
    for (;;) {
        /* Receive message from client */
        memset(buffer, 0, BUFFER_SIZE);
        //try to receive some data, this is a blocking call
        ssize_t bytes = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *) &si_other, &slen);
		if (bytes == -1)
		{
			printf("recvfrom() failed\n");
            break;
		}
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Received: %s\n", buffer);

        /* Echo back with a prefix */
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "Echo: %s", buffer);
        if (sendto(server_fd, response, strlen(response), 0, (struct sockaddr*) &si_other, slen) == -1)
		{
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
