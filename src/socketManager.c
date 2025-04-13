#include "traceroute.h"

int init_socket4(t_traceroute *traceroute) {
    struct addrinfo hints = {0};
    struct addrinfo *res = NULL;
    const char *service = NULL;
    Logger *logger = get_logger_instance();

    if (traceroute->traceroute_data.option & E_ICMP) {
        if (getuid() != 0) {
            logger->log(E_LOG_ERROR, true, "You must be root to use ICMP\n");
            return (TRACEROUTE_ERROR);
        }
        hints.ai_socktype = SOCK_RAW;
        hints.ai_protocol = IPPROTO_ICMP;
        logger->log(E_LOG_DEBUG, true, "use ICMP\n");
    } else {
        logger->log(E_LOG_DEBUG, true, "use UDP\n");
        service = traceroute->traceroute_data.port_number == 0 ? TRACEROUTE_SRC_PORT : ft_itoa(traceroute->traceroute_data.port_number);
        hints.ai_protocol = IPPROTO_UDP;
        hints.ai_socktype = SOCK_DGRAM;
    }

    hints.ai_family = AF_INET;

    if (getaddrinfo(traceroute->traceroute_data.hostname, service, &hints, &res) != 0) {
        logger->log(E_LOG_ERROR, true, "getaddrinfo: %s: %s\n", traceroute->traceroute_data.hostname, gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    traceroute->traceroute_socket.addr = res;

    if ((traceroute->traceroute_socket.socket = socket(AF_INET, res->ai_socktype, res->ai_protocol)) < 0) {
        freeaddrinfo(res);
        logger->log(E_LOG_ERROR, true, "socket: %s\n", gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    if (setsockopt(traceroute->traceroute_socket.socket, IPPROTO_IP, IP_TTL, &traceroute->traceroute_data.ttl, sizeof(traceroute->traceroute_data.ttl)) < 0) {
        close(traceroute->traceroute_socket.socket);
        freeaddrinfo(res);
        logger->log(E_LOG_ERROR, true, "setsockopt: %d\n", errno);
        logger->log(E_LOG_ERROR, true, "setsockopt: %s\n", gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    if (setsockopt(traceroute->traceroute_socket.socket, SOL_SOCKET, SO_RCVTIMEO, &(struct timeval){TRACEROUTE_DEFAULT_TIMEOUT, 0}, sizeof(struct timeval)) < 0) {
        close(traceroute->traceroute_socket.socket);
        freeaddrinfo(res);
        logger->log(E_LOG_ERROR, true, "setsockopt: %d\n", errno);
        logger->log(E_LOG_ERROR, true, "setsockopt: %s\n", gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    if (inet_ntop(AF_INET, &((struct sockaddr_in *)res->ai_addr)->sin_addr, traceroute->traceroute_data.ip, INET_ADDRSTRLEN) == NULL) {
        close(traceroute->traceroute_socket.socket);
        freeaddrinfo(res);
        logger->log(E_LOG_ERROR, true, "inet_ntop: %s\n", gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    freeaddrinfo(res);
    return (TRACEROUTE_SUCCESS);
}

int init_socket6(t_traceroute *traceroute) {
    struct addrinfo hints = {0};
    struct addrinfo *res = NULL;
    const char *service = NULL;
    Logger *logger = get_logger_instance();

    if (traceroute->traceroute_data.option & E_ICMP) {
        if (getuid() != 0) {
            logger->log(E_LOG_ERROR, true, "You must be root to use ICMP\n");
            return (TRACEROUTE_ERROR);
        }
        hints.ai_socktype = SOCK_RAW;
        hints.ai_protocol = IPPROTO_ICMPV6;
    } else {
        logger->log(E_LOG_DEBUG, true, "use UDP\n");
        service = traceroute->traceroute_data.port_number == 0 ? TRACEROUTE_SRC_PORT : ft_itoa(traceroute->traceroute_data.port_number);
        hints.ai_protocol = IPPROTO_UDP;
        hints.ai_socktype = SOCK_DGRAM;
    }

    hints.ai_family = AF_INET6;

    if (getaddrinfo(traceroute->traceroute_data.hostname, service, &hints, &res) != 0) {
        logger->log(E_LOG_ERROR, true, "getaddrinfo: %s: %s\n", traceroute->traceroute_data.hostname, gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    traceroute->traceroute_socket.addr = res;

    if ((traceroute->traceroute_socket.socket = socket(AF_INET6, res->ai_socktype, res->ai_protocol)) < 0) {
        freeaddrinfo(res);
        logger->log(E_LOG_ERROR, true, "socket: %s\n", gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    if (setsockopt(traceroute->traceroute_socket.socket, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &traceroute->traceroute_data.ttl, sizeof(traceroute->traceroute_data.ttl)) < 0) {
        freeaddrinfo(res);
        close(traceroute->traceroute_socket.socket);
        logger->log(E_LOG_ERROR, true, "setsockopt: %d\n", errno);
        logger->log(E_LOG_ERROR, true, "setsockopt: %s\n", gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    if (setsockopt(traceroute->traceroute_socket.socket, SOL_SOCKET, SO_RCVTIMEO, &(struct timeval){TRACEROUTE_DEFAULT_TIMEOUT, 0}, sizeof(struct timeval)) < 0) {
        freeaddrinfo(res);
        close(traceroute->traceroute_socket.socket);
        logger->log(E_LOG_DEBUG, true, "hostname: %s\n", traceroute->traceroute_data.hostname);
        logger->log(E_LOG_ERROR, true, "setsockopt: %d\n", errno);
        logger->log(E_LOG_ERROR, true, "setsockopt: %s\n", gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    if (inet_ntop(AF_INET6, &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr, traceroute->traceroute_data.ip, INET6_ADDRSTRLEN) == NULL) {
        freeaddrinfo(res);
        close(traceroute->traceroute_socket.socket);
        logger->log(E_LOG_ERROR, true, "inet_ntop: %s\n", gai_strerror(errno));
        return (TRACEROUTE_ERROR);
    }

    return (TRACEROUTE_SUCCESS);
}

int increase_ttl(t_traceroute *traceroute)
{
    traceroute->traceroute_data.ttl++;
    if (traceroute->traceroute_data.option & E_IPV4) {
        if (setsockopt(traceroute->traceroute_socket.socket, IPPROTO_IP, IP_TTL, &traceroute->traceroute_data.ttl, sizeof(traceroute->traceroute_data.ttl)) < 0) {
            return (TRACEROUTE_ERROR);
        }
    } else if (traceroute->traceroute_data.option & E_IPV6) {
        if (setsockopt(traceroute->traceroute_socket.socket, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &traceroute->traceroute_data.ttl, sizeof(traceroute->traceroute_data.ttl)) < 0) {
            return (TRACEROUTE_ERROR);
        }
    }
    return (TRACEROUTE_SUCCESS);
}

void increase_port(t_traceroute *traceroute)
{
    traceroute->traceroute_data.port_number++;
    if (traceroute->traceroute_data.port_number >= 65535) {
        traceroute->traceroute_data.port_number = TRACEROUTE_DEFAULT_PORT;
    }
    if (traceroute->traceroute_data.option & E_IPV4) {
        struct sockaddr_in *addr = (struct sockaddr_in *)traceroute->traceroute_socket.addr->ai_addr;
        addr->sin_port = htons(traceroute->traceroute_data.port_number);
    }
    else if (traceroute->traceroute_data.option & E_IPV6) {
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *)traceroute->traceroute_socket.addr->ai_addr;
        addr->sin6_port = htons(traceroute->traceroute_data.port_number);
    }
}
