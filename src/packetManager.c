#include "traceroute.h"

static u_int16_t checksum(void *b, int len)
{
    u_int16_t *buf = b;
    u_int32_t sum = 0;
    u_int16_t result;

    for (sum = 0; len > 1; len -= 2)
    {
        sum += *buf++;
    }
    if (len == 1)
    {
        sum += *(u_int8_t *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return (result);
}

void forge_udp_packet(t_traceroute *traceroute)
{
    t_traceroute_udp_packet_send *packet = (t_traceroute_udp_packet_send *)traceroute->packet_send;
    struct timeval *tv = &packet->tv;

    gettimeofday(tv, NULL);
}

void forge_icmp4_packet(t_traceroute *traceroute)
{
    t_traceroute_icmp_packet_send_4 *packet = (t_traceroute_icmp_packet_send_4 *)traceroute->packet_send;
    struct timeval *tv = &packet->tv;

    packet->icmp.un.echo.sequence = htons(traceroute->traceroute_data.seq);
    traceroute->traceroute_data.seq++;
    gettimeofday(tv, NULL);
    packet->icmp.checksum = 0;
    packet->icmp.checksum = checksum((unsigned short *)&packet->icmp, sizeof(t_traceroute_icmp_packet_send_4));
}

void forge_icmp6_packet(t_traceroute *traceroute)
{
    t_traceroute_icmp_packet_send_6 *packet = (t_traceroute_icmp_packet_send_6*)traceroute->packet_send;
    struct timeval *tv = &packet->tv;

    packet->icmp.icmp6_seq = htons(traceroute->traceroute_data.seq);
    gettimeofday(tv, NULL);
    packet->icmp.icmp6_cksum = 0;
    packet->icmp.icmp6_cksum = checksum((unsigned short *)&packet->icmp, sizeof(t_traceroute_icmp_packet_send_6));
    traceroute->traceroute_data.seq++;
}

socklen_t init_packet(t_traceroute *traceroute, t_traceroute_routine *routine)
{
    if (traceroute->traceroute_data.option & E_IPV6 && traceroute->traceroute_data.option & E_ICMP) {
        t_traceroute_icmp_packet_send_6 packet = {0};
        packet.icmp.icmp6_cksum = 0;
        packet.icmp.icmp6_type = ICMP6_ECHO_REQUEST;
        packet.icmp.icmp6_code = 0;
        routine->forge_packet = forge_icmp6_packet;
        ft_memset(packet.data, 'A', TRACEROUTE_DEFAULT_PACKET_SIZE);
        ft_memcpy(traceroute->packet_send, &packet, sizeof(t_traceroute_icmp_packet_send_6));
        routine->size = sizeof(t_traceroute_icmp_packet_send_6);
    }
    else if (traceroute->traceroute_data.option & E_ICMP) {
        t_traceroute_icmp_packet_send_4 packet = {0};
        packet.icmp.checksum = 0;
        packet.icmp.type = ICMP_ECHO;
        packet.icmp.code = 0;
        packet.icmp.un.echo.id = htons(getpid() & 0xFFFF);
        packet.icmp.un.echo.sequence = 0;
        routine->forge_packet = forge_icmp4_packet;
        ft_memset(packet.data, 'A', TRACEROUTE_DEFAULT_PACKET_SIZE);
        ft_memcpy(traceroute->packet_send, &packet, sizeof(t_traceroute_icmp_packet_send_4));
        routine->size = sizeof(t_traceroute_icmp_packet_send_4);
    }
    else if (traceroute->traceroute_data.option & E_UDP) {
        t_traceroute_udp_packet_send packet = {0};
        routine->forge_packet = forge_udp_packet;
        ft_memset(packet.data, 'A', TRACEROUTE_DEFAULT_PACKET_SIZE);
        gettimeofday(&packet.tv, NULL);
        ft_memcpy(traceroute->packet_send, &packet, sizeof(t_traceroute_udp_packet_send));
        routine->size = sizeof(t_traceroute_udp_packet_send);
    }
    if (traceroute->traceroute_data.option & E_IPV6) {
        return sizeof(struct sockaddr_in6);
    }
    return sizeof(struct sockaddr_in);
}

