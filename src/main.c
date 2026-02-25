#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>

#include "wifi.h"
#include "wifi_pswd.h"
#include "zephyr/net/dns_resolve.h"

#define HTTP_HOST "example.com"
#define HTTP_URL "/"

static char resonse[4096];

// print the results of a DNS lookup
void print_addr_info(struct zsock_addrinfo **results)
{
    char ipv4[INET_ADDRSTRLEN];
    char ipv6[INET6_ADDRSTRLEN];
    struct sockaddr_in *sa;
    struct sockaddr_in6 *sa6;
    struct zsock_addrinfo *rp;

    // iterate through the results
    for (rp = *results; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_addr->sa_family ==AF_INET) {
            sa = (struct sockaddr_in *)rp->ai_addr;
            zsock_inet_ntop(AF_INET, &sa->sin_addr, ipv4, INET_ADDRSTRLEN);
            printk("IPv4: %s\n", ipv4);
        }
        if (rp->ai_addr->sa_family == AF_INET6) {
            sa6 = (struct sockaddr_in6 *)rp->ai_addr;
            zsock_inet_ntop(AF_INET6, &sa6->sin6_addr, ipv6, INET6_ADDRSTRLEN);
            printk("IPv6: %s\n", ipv6);
        }
    }

}

int main(void) 
{
    struct zsock_addrinfo hints;
    struct zsock_addrinfo *res;
    char http_request[512];
    int sock;
    int len;
    uint32_t rx_total;
    int ret;

    printk("HTTP DEMO\n");

    // initialize the WiFi
    my_wifi_init();

    printk("Connecting to WiFi...\n");

    // Connect to the WiFi network
    // ret = wifi_connect(BITCRAZE_SSID, BITCRAZE_PASSWORD);
    if (wifi_connect(BITCRAZE_SSID, BITCRAZE_PASSWORD)) {
        printk("Failed to connect to WiFi\n");
        return -1;
    }

    // Wait to obtain an IP address
    wifi_wait_for_ip_addr();

    // Construct HTTP request
    snprintf(http_request, sizeof(http_request), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", HTTP_URL, HTTP_HOST);

    // Clear and set address infor
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;  // TCP socket

    // Perform DNS lookup and print
    printk("Performing DNS lookup for %s...\n", HTTP_HOST);
    ret = zsock_getaddrinfo(HTTP_HOST, "80", &hints, &res);
    if (ret != 0) {
        printk("DNS lookup failed: %d\n", ret);
        return -1;
    }
    print_addr_info(&res);

    // Create socket
    sock = zsock_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        printk("Error (%d): Could not create socket\n", errno);
        return -1;
    }

    // Connect the socket
    ret = zsock_connect(sock, res->ai_addr, res->ai_addrlen);
    if (ret < 0) {
        printk("Error (%d): Could not connect to server\n", errno);
        return -1;
    }

    // Send the request
    printk("Sending HTTP request: ... \n");
    ret = zsock_send(sock, http_request, strlen(http_request), 0);
    if (ret < 0) {
        printk("Error (%d): Failed to send HTTP request\n", errno);
        return -1;
    }

    // print the response
    printk("Received response:\n");
    rx_total = 0;
    while (1) {
        // Receive data -1, to reserve space for null terminator
        len = zsock_recv(sock, resonse, sizeof(resonse) - 1, 0);
        
        if (len < 0) {
            printk("Error (%d): %s\n", errno, strerror(errno));
            return -1;
        } else if (len == 0) {
            // No more data
            break;
        }

        resonse[len] = '\0';  // Null-terminate the response
        printk("%s", resonse);  // Print the response chunk
        rx_total += len;
        break;
    }

    printk("\nTotal bytes received: %u\n", rx_total);

    // Close the socket
    zsock_close(sock);

    wifi_disconnect();

    return 0;

}

// int main(void) 
// {
//     printk("Starting WiFi test...\n");

//     my_wifi_init();

//     // Connect to the WiFi network
//     if (wifi_connect(BITCRAZE_SSID, BITCRAZE_PASSWORD) != 0) {
//         printk("Failed to connect to WiFi\n");
//         return -1;
//     }

//     // Wait for an IP address to be obtained
//     wifi_wait_for_ip_addr();

//     k_sleep(K_SECONDS(60));

//     // Disconnect from the WiFi network
//     if (wifi_disconnect() != 0) {
//         printk("Failed to disconnect from WiFi\n");
//         return -1;
//     }

//     return 0;
// }