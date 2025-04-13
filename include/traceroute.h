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
# include <netinet/icmp6.h>
# include <arpa/inet.h>
# include <netdb.h>

# include <netinet/tcp.h>

# include "optLib/optlib.h"
# include "libft/libft.h"
# include "logger/ft_logger.h"
# include "logger/ft_color.h"

# define TRACEROUTE_ERROR 1
# define TRACEROUTE_SUCCESS 0
# define TRACEROUTE_ROUTINE_SUCCESS 2


# define TRACEROUTE_DEFAULT_IP 0x4
# define TRACEROUTE_DEFAULT_PACKET_INTERVAL 1
# define TRACEROUTE_DEFAULT_PACKET_COUNT 3
# define TRACEROUTE_DEFAULT_PACKET_SIZE 64
# define TRACEROUTE_SRC_PORT "80"
# define TRACEROUTE_DEFAULT_TIMEOUT 1
# define TRACEROUTE_DEFAULT_MAX_TTL 64
# define TRACEROUTE_DEFAULT_MIN_TTL 1
# define TRACEROUTE_DEFAULT_PORT 33434

# define MAX_PACKET_SIZE 110


typedef int32_t socket_t;

typedef char* t_data;

enum e_traceroute_option {
    E_UDP  = 0x1,
    E_ICMP = 0x2,
    E_IPV4 = 0x4,
    E_IPV6 = 0x8,
};

typedef struct {
    char *hostname;
    char ip[INET6_ADDRSTRLEN];
    int32_t option;
    int32_t ip_proto;
    u_int32_t ttl;
    u_int8_t max_ttl;
    u_int16_t port_number;
    u_int16_t seq;
} t_traceroute_data;

typedef struct {
    socket_t socket;
    const struct addrinfo *addr;
} t_traceroute_socket;

typedef struct {
    struct icmphdr icmp;
    struct timeval tv;
    char data[TRACEROUTE_DEFAULT_PACKET_SIZE];
} t_traceroute_icmp_packet_send_4;

typedef struct {
    struct icmp6_hdr icmp;
    struct timeval tv;
    char data[TRACEROUTE_DEFAULT_PACKET_SIZE];
} t_traceroute_icmp_packet_send_6;

typedef struct {
    struct timeval tv;
    char data[TRACEROUTE_DEFAULT_PACKET_SIZE];
} t_traceroute_udp_packet_send;

typedef struct __attribute__((packed)) {
    struct iphdr ip;
    struct icmphdr icmp;
} t_traceroute_icmp_packet_recv_4;

typedef struct __attribute__((packed)) {
    struct ip6_hdr ip;
    struct icmp6_hdr icmp;
} t_traceroute_icmp_packet_recv_6;

typedef struct {
    t_traceroute_socket traceroute_socket;
    t_traceroute_data   traceroute_data;
    t_data              packet_send[TRACEROUTE_DEFAULT_PACKET_SIZE];
} t_traceroute;

typedef void (*forge_packet_func)(t_traceroute *traceroute);

typedef struct {
    size_t              size;
    forge_packet_func   forge_packet;
} t_traceroute_routine;

int32_t create_option(t_opt_list *opt_lists);

t_traceroute_data get_traceroute_data();
void debug_tracerooute_data(t_traceroute_data *traceroute);

socklen_t init_packet(t_traceroute *traceroute, t_traceroute_routine *routine);

int traceroute_routine(t_traceroute *traceroute);
int increase_ttl(t_traceroute *traceroute);

int init_socket4(t_traceroute *traceroute);
int init_socket6(t_traceroute *traceroute);

void increase_port(t_traceroute *traceroute);

void print_bits(char * ty, char * val, unsigned char * bytes, size_t num_bytes);
void print_byte_as_bits(char val);

#define SHOW(T,V) do { T x = V; print_bits(#T, #V, (unsigned char*) &x, sizeof(x)); } while(0)

#endif // TRACEROUTE_H