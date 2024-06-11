#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int connect_to_server(const char *host_ip, uint16_t port);

int next_image(int sock, char *rx_buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif


#endif
