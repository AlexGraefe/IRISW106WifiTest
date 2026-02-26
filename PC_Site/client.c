#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT        8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *server_ip = argv[1];
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    /* Create TCP socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Build server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    /* Connect to server */
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Client] Connected to %s:%d\n", server_ip, PORT);

    /* Send a few messages and wait for the echo */
    const char *messages[] = {
        "Hello, Server!",
        "How are you?",
        "Socket demo working.",
        "Goodbye!",
        NULL
    };

    for (int i = 0; messages[i] != NULL; i++) {
        /* Send */
        if (send(sock_fd, messages[i], strlen(messages[i]), 0) < 0) {
            perror("send");
            break;
        }
        printf("[Client] Sent:     %s\n", messages[i]);

        /* Receive echo */
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) {
            if (bytes == 0)
                printf("[Client] Server closed the connection.\n");
            else
                perror("recv");
            break;
        }
        printf("[Client] Received: %s\n", buffer);
    }

    close(sock_fd);
    printf("[Client] Closed.\n");
    return 0;
}
