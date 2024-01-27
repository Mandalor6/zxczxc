#ifndef NETWORK_V_SOCKET_H
#define NETWORK_V_SOCKET_H

/* Define what a socket descriptor is based on platform */
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>

#define VENOK_SOCKET_DESCRIPTOR SOCKET
#else
#define VENOK_SOCKET_DESCRIPTOR int
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct v_socket;
struct v_socket_context;
struct v_loop;
struct v_poll;
struct v_timer;
struct v_udp_socket;
struct v_udp_packet_buffer;


#ifdef __cplusplus
}
#endif

#endif //NETWORK_V_SOCKET_H
