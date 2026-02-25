#ifndef WIFI_H
#define WIFI_H

void my_wifi_init(void); // rename, currently wifi_init has a name clash with nxp library
int wifi_connect(char *ssid, char *psk);
void wifi_wait_for_ip_addr(void);
int wifi_disconnect(void);

#endif /* WIFI_H */