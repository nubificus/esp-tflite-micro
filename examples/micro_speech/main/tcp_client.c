#include "tcp_client.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"

static const char *TAG = "tcp_client";
static const char request_byte = '1';

int connect_to_server(const char *host_ip, uint16_t port) {
    int sock = -1;
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return -1;
    }

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(sock);
        return -1;
    }

    ESP_LOGI(TAG, "Successfully connected to %s:%d", host_ip, port);
    return sock;
}

int next_chunk(int sock, char *rx_buffer, size_t buffer_size) {
    memset(rx_buffer, 0, buffer_size);

    int err = send(sock, &request_byte, 1, 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        return -1;
    }

    /* ESP_LOGI(TAG, "Request sent"); */

    int len = recv(sock, rx_buffer, buffer_size, 0);
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        return -1;
    }

    /* ESP_LOGI(TAG, "Received %d bytes from server", len); */
    return len;
}
