A repo demonstrating the WiFi capabilities of the IRIS board.

## Installation
Follow: https://docs.zephyrproject.org/latest/develop/getting_started/index.html

Add this line to ~/.bashrc:

```bash
export ZEPHYR_BASE=~/zephyrproject/zephyr
```

### VSCode
Follow: https://docs.zephyrproject.org/latest/develop/tools/vscode.html

For automatic source of the zephyr venv, do the following:
1. crtl + shift + P
2. Type Python: Select Interpreter
3. Enter ~/zephyrproject/.venv/bin/python

## Building
```bash
west build -p auto -b ubx_evk_iris_w1@fidelix .
```

## Flashing
```bash
west flash
```

## Debugging
```bash
west attatch
```

## Wifi utilities
Wifi utilities, like connecting and disconnecting to a Wifi can be found in [`modules/wifi_utilities`](./modules/wifi_utilities).

## TCP Socket Demo
It is based on: https://www.youtube.com/watch?v=0ONIU4JRnHE. The code dan be found in  [`modules/tcp_socket_demo`](./modules/tcp_socket_demo). It can be enabled by setting CONFIG_TCP_SOCKET_DEMO=y in prj.conf. It runs in a seperate thread. Add a folder secret to modules/tcp_socket_demo with makros:

```C
#define BITCRAZE_SSID "my_ssid"
#define BITCRAZE_PASSWORD "my_password"
```

Compile PC_Site:
```bash
cd PC_Site
cmake -B build
cmake --build build
```

Run it
```bash
./PC_Site/build/tcp_socket_server
```

Compile and flash the board. After some time (~30s), the LED on the board should turn from red to blue (wifi connected) green (IP resolved) yellow (socket established) and tcp_socket_server should printout messages received from the board. You can also connect to the serial output of the board (baudrate 115200) and see log messages.


## UDP Socket Demo
In this demo, the board opens up a server with a UDP socket.

The code dan be found in  [`modules/udp_socket_demo`](./modules/udp_socket_demo). It can be enabled by setting CONFIG_TCP_SOCKET_DEMO=y in prj.conf. It runs in a seperate thread. Add a folder secret to modules/udp_socket_demo with makros:

```C
#define BITCRAZE_SSID "my_ssid"
#define BITCRAZE_PASSWORD "my_password"
```

Compile PC_Site:
```bash
cd PC_Site
cmake -B build
cmake --build build
```

Run it
```bash
./PC_Site/build/udp_socket_client
```

Compile and flash the board. After some time (~30s), the LED on the board should turn from red to blue (wifi connected) green (IP resolved) yellow (socket established) and tcp_socket_server should printout messages received from the board. You can also connect to the serial output of the board (baudrate 115200) and see log messages.