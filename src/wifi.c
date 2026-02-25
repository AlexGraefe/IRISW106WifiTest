#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>

#include "wifi.h"

// Event callbacks
static struct net_mgmt_event_callback wifi_cb;
static struct net_mgmt_event_callback ipv4_cb;

// Semaphores
static K_SEM_DEFINE(sem_wifi, 0, 1);
static K_SEM_DEFINE(sem_ipv4, 0, 1);

// called when the WiFi is connected
static void on_wifi_connection_event(struct net_mgmt_event_callback *cb, 
                                     uint64_t mgmt_event, 
                                     struct net_if *iface)
{
    const struct wifi_status *status = (const struct wifi_status *)cb->info;

    if (mgmt_event == NET_EVENT_WIFI_CONNECT_RESULT) {
        if (status->status) {
            printk("WiFi connection failed with status: %d\n", status->status);
        } else {
            printk("Connected!\n");
            k_sem_give(&sem_wifi);
        }
    } else if (mgmt_event == NET_EVENT_WIFI_DISCONNECT_RESULT) {
        if (status->status) {
            printk("WiFi disconnection failed with status: %d\n", status->status);
        }
        else {
            printk("Disconnected\n");
            k_sem_take(&sem_wifi, K_NO_WAIT);
        }
    }
}

// event handler for WiFi management events
static void on_ipv4_obtained(struct net_mgmt_event_callback *cb, 
                             uint64_t mgmt_event, 
                             struct net_if *iface)
{
    // Signal that the IP address has been obtained (for ipv6, change accordingly)
    if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD) {
        k_sem_give(&sem_ipv4);
    }
}


// initialize the WIFi event callbacks
void my_wifi_init(void)
{
    // Initialize the event callback
    net_mgmt_init_event_callback(&wifi_cb, 
                        on_wifi_connection_event, 
                NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT);
    net_mgmt_init_event_callback(&ipv4_cb,
                        on_ipv4_obtained,
                NET_EVENT_IPV4_ADDR_ADD);

    // Add the event callback
    net_mgmt_add_event_callback(&wifi_cb);
    net_mgmt_add_event_callback(&ipv4_cb);
    
}

// connect to WiFi (blocking)
int wifi_connect(char *ssid, char *psk)
{
    // printk("h");
    int ret;
    struct net_if *iface;
    struct wifi_connect_req_params params = {};

    // Get the default network interface
    iface = net_if_get_default();  // might need to change this, if the IRIS' wifi is not the default interface

    // Fill in the connection request parameters
    params.ssid = (const uint8_t *)ssid;
    params.ssid_length = strlen(ssid);
    params.psk = (const uint8_t *)psk;
    params.psk_length = strlen(psk);
    params.security = WIFI_SECURITY_TYPE_PSK;  // WPA2-PSK security
    params.band = WIFI_FREQ_BAND_UNKNOWN;  // Auto-select the band
    params.channel = WIFI_CHANNEL_ANY;  // Auto-select the channel
    params.mfp = WIFI_MFP_OPTIONAL;

    // Connect to the WiFi network
    ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, 
                  iface,
             &params,
               sizeof(struct wifi_connect_req_params));

    // Wait for the connection to complete
    k_sem_take(&sem_wifi, K_FOREVER);

    return ret;
}

// Wait for an IP address to be obtained (blocking)
void wifi_wait_for_ip_addr(void) 
{
    struct wifi_iface_status status;
    struct net_if *iface;
    char ip_addr[NET_IPV4_ADDR_LEN];
    char gw_addr[NET_IPV4_ADDR_LEN];

    // Get interface
    iface = net_if_get_default();  // might need to change this, if the IRIS' wifi is not the default interface

    // Wait for an IPv4 address to be obtained
    k_sem_take(&sem_ipv4, K_FOREVER);

    // Get the WiFi status
    if (net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS,
                 iface, 
                 &status,
                 sizeof(struct wifi_iface_status)))
    {
        printk("Failed to get WiFi status\n");
    }

    // Get the IP address
    memset(ip_addr, 0, sizeof(ip_addr));  // Clear the buffer
    if (net_addr_ntop(AF_INET,
                &iface->config.ip.ipv4->unicast[0].ipv4.address.in_addr,
                ip_addr, 
                sizeof(ip_addr)) == NULL) {
        printk("Failed to convert IP address to string\n");
    } 

    // Get the gateway address
    memset(gw_addr, 0, sizeof(gw_addr));  // Clear the buffer
    if (net_addr_ntop(AF_INET,
                 &iface->config.ip.ipv4->gw,
                 gw_addr,
                 sizeof(gw_addr)) == NULL) {
        printk("Failed to convert gateway address to string\n");
    }

    // Print the WiFi status
    printk("WiFi status:\n");
    if (status.state >= WIFI_STATE_ASSOCIATED) {
        printk("  SSID: %s\n", status.ssid);
        printk("  Band: %s\n", wifi_band_txt(status.band));
        printk("  Channel: %d\n", status.channel);
        printk("  Security: %s\n", wifi_security_txt(status.security));
        printk("  RSSI: %d dBm\n", status.rssi);
        printk("  IP Address: %s\n", ip_addr);
        printk("  Gateway: %s\n", gw_addr);
    }
}

// Disconnect fomr the WiFi network
int wifi_disconnect(void)
{
    int ret;
    struct net_if *iface;

    // Get the default network interface
    iface = net_if_get_default();  // might need to change this, if the IRIS' wifi is not the default interface

    // Disconnect from the WiFi network
    ret = net_mgmt(NET_REQUEST_WIFI_DISCONNECT, 
                  iface,
               NULL,
               0);

    return ret;
}
