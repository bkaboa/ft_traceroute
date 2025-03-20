#ifndef TRACEROUTE_H
# define TRACEROUTE_H

# include <signal.h>

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <errno.h>
# include <sys/time.h>

# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip6.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <netdb.h>

# include <netinet/tcp.h>

# include "optLib/optlib.h"
# include "libft/libft.h"
# include "logger/ft_logger.h"
# include "logger/ft_color.h"

# define TRACEROUTE_ERROR 1
# define TRACEROUTE_SUCCESS 0


# define TRACEROUTE_DEFAULT_IP 0x4
# define TRACEROUTE_DEFAULT_PACKET_PROTOCOL IPPROTO_TCP
# define TRACEROUTE_DEFAULT_PACKET_INTERVAL 1
# define TRACEROUTE_DEFAULT_PACKET_COUNT 3
# define TRACEROUTE_DEFAULT_PACKET_SIZE 60
# define TRACEROUTE_DEFAULT_TIMEOUT 5
# define TRACEROUTE_DEFAULT_TTL 64
# define TRACEROUTE_DEFAULT_PORT 33434

typedef int32_t socket_t;

enum e_traceroute_option {
    E_TCP = 0x1,
    E_ICMP = 0x2,
    E_IPV4 = 0x4,
    E_IPV6 = 0x8,
};

typedef struct {
    char *hostname;
    char *ip;
    int32_t option;
    int32_t ip_proto;
    int8_t ttl;
    int8_t max_ttl;
    int16_t port_number;
} t_traceroute_data;

typedef struct {
    socket_t socket;
    struct sockaddr_in addr;
} t_traceroute_socket;

typedef struct {
    union
    {
        struct icmphdr icmp;
        struct tcphdr tcp;
    } u_header;
    struct timeval tv;
    char data[64];
} t_traceroute_packet;

typedef struct __attribute__((packed)) {
    struct iphdr ip;
    t_traceroute_packet packet;
} t_traceroute_packet_recv_4;

typedef struct __attribute__((packed)) {
    struct ip6_hdr ip;
    t_traceroute_packet packet;
} t_traceroute_packet_recv_6;

typedef struct {
    t_traceroute_socket traceroute_socket;
    t_traceroute_data traceroute_data;
    t_traceroute_packet traceroute_packet;
} t_traceroute;

int32_t create_option(t_opt_list *opt_lists);

#endif // TRACEROUTE_H