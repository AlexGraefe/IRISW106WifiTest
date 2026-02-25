#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>

#include "wifi.h"

int main(void) 
{
    printk("Starting WiFi test...\n");

    my_wifi_init();

    // Connect to the WiFi network
    if (wifi_connect() != 0) {
        printk("Failed to connect to WiFi\n");
        return -1;
    }

    // Wait for an IP address to be obtained
    wifi_wait_for_ip_addr();

    k_sleep(K_SECONDS(60));

    // Disconnect from the WiFi network
    if (wifi_disconnect() != 0) {
        printk("Failed to disconnect from WiFi\n");
        return -1;
    }

    return 0;
}