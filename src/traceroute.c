#include "traceroute.h"

void print_recv(t_traceroute *traceroute, char *recv_buffer, int hops, int recv, struct timeval send_time)
{
    Logger *logger = get_logger_instance();
    char   ip[INET6_ADDRSTRLEN] = {0};
    static u_int32_t    inet[3][4] = {0};
    int                 index = hops % 3;
    bool                is_recv = true;
    struct timeval      recv_time = {0};

    gettimeofday(&recv_time, NULL);
    if (hops == 0) {
        logger->log(E_LOG_INFO, false, "%d: ", traceroute->traceroute_data.ttl);
    }
    else if (index == 0) {
        ft_memset(inet, 0, sizeof(inet));
        logger->log(E_LOG_INFO, false, "\n%d: ", traceroute->traceroute_data.ttl);
    }
    if (recv < 0) {
        logger->log(E_LOG_INFO, false, "* ");
    }
    else {
        if (traceroute->traceroute_data.option & E_IPV6) {
            t_traceroute_icmp_packet_recv_6 icmp_recv = {0};
            ft_memcpy(&icmp_recv, recv_buffer, sizeof(t_traceroute_icmp_packet_recv_6));
            if (index) {
                inet[index][0] = icmp_recv.ip.ip6_dst.__in6_u.__u6_addr32[0];
                inet[index][1] = icmp_recv.ip.ip6_dst.__in6_u.__u6_addr32[1];
                inet[index][2] = icmp_recv.ip.ip6_dst.__in6_u.__u6_addr32[2];
                inet[index][3] = icmp_recv.ip.ip6_dst.__in6_u.__u6_addr32[3];
            }
            else if (index == 0) {
                is_recv = false;
            }
            else if (index == 1 && \
                (inet[0][0] ^ inet[1][0] || \
                inet[0][1] ^ inet[1][1] || \
                inet[0][2] ^ inet[1][2] || \
                inet[0][3] ^ inet[1][3])) {
                    is_recv = false;
            }
            else if (index == 2 && \
                ((inet[0][0] ^ inet[2][0] || \
                inet[0][1] ^ inet[2][1] || \
                inet[0][2] ^ inet[2][2] || \
                inet[0][3] ^ inet[2][3] || \
                inet[1][0] ^ inet[2][0]) && \
                (inet[1][1] ^ inet[2][1] || \
                inet[1][2] ^ inet[2][2] || \
                inet[1][3] ^ inet[2][3]))) {
                    is_recv = false;
            }
            if (is_recv == false) {
                inet_ntop(AF_INET6, &icmp_recv.ip.ip6_dst, ip, INET6_ADDRSTRLEN);
                logger->log(E_LOG_INFO, false, "%s ", ip);
            }
        }
        else {
            t_traceroute_icmp_packet_recv_4 icmp_recv = {0};
            ft_memcpy(&icmp_recv, recv_buffer, sizeof(t_traceroute_icmp_packet_recv_4));
            if (index) {
                inet[index][0] = icmp_recv.ip.saddr;
            }
            else if (index == 0) {
                is_recv = false;
            }
            else if (index == 1 && \
                (inet[0][0] ^ inet[1][0])) {
                    is_recv = false;
            }
            else if (index == 2 && \
                (inet[0][0] ^ inet[2][0] && \
                inet[1][0] ^ inet[2][0])) {
                    is_recv = false;
            }
            if (is_recv == false) {
                inet_ntop(AF_INET, &icmp_recv.ip.saddr, ip, INET_ADDRSTRLEN);
                logger->log(E_LOG_INFO, false, "%s ", ip);
            }
        }
        // SHOW(uint32_t, inet[index][0]);
        logger->log(E_LOG_INFO, false, "(%d ms) ", \
            (int)((recv_time.tv_sec - send_time.tv_sec) * 1000 + \
            (recv_time.tv_usec - send_time.tv_usec) / 1000));
    }
}

int traceroute_routine(t_traceroute *traceroute)
{
    bool                    done = true;
    int                     hops = 0;
    socklen_t              addr_size = 0;
    t_traceroute_routine    routine = {0};
    Logger                  *logger = get_logger_instance();
    char                    recv_buffer[200] = {0};
    struct timeval          send_time = {0};

    addr_size = init_packet(traceroute, &routine);

    logger->log(E_LOG_DEBUG, true, "traceroute sockaddrin6 size: %lu\n", sizeof(struct sockaddr_in6));
    logger->log(E_LOG_DEBUG, true, "addr_size: %lu\n", addr_size);

    logger->log(E_LOG_DEBUG, true, "traceroute_routine: %s\n", traceroute->traceroute_data.hostname);
    logger->log(E_LOG_DEBUG, true, "traceroute socket addr: %p\n", traceroute->traceroute_socket.addr);

    logger->log(E_LOG_INFO, false, "traceroute to %s (%s), %d hops max\n", traceroute->traceroute_data.hostname, traceroute->traceroute_data.ip, traceroute->traceroute_data.max_ttl);

    while(traceroute->traceroute_data.ttl < traceroute->traceroute_data.max_ttl && done) {
        if (hops % 3 == 0 && hops > 0) {
            increase_ttl(traceroute);
        }
        routine.forge_packet(traceroute);
        gettimeofday(&send_time, NULL);
        sendto(traceroute->traceroute_socket.socket, traceroute->packet_send, routine.size, 0, traceroute->traceroute_socket.addr->ai_addr, addr_size);
        if (hops > 40) {
            done = false;
        }
        int recv = recvfrom(traceroute->traceroute_socket.socket, recv_buffer, 200, 0, NULL, NULL);
        logger->log(E_LOG_DEBUG, true, "recv: %d\n", recv);
        logger->log(E_LOG_ERROR, true, "gai_strerror: %s\n", strerror(errno));
        print_recv(traceroute, recv_buffer, hops, recv, send_time);
        if (traceroute->traceroute_data.option & E_UDP) {
            increase_port(traceroute);
        }
        hops++;
        ft_memset(recv_buffer, 0, sizeof(recv_buffer));
    }
    return (TRACEROUTE_SUCCESS);
}