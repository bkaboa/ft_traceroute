#include "traceroute.h"

static t_traceroute_data traceroute_data;
static Logger *logger;

static void init_traceroute() {
    traceroute_data.hostname = NULL;
    traceroute_data.option = E_IPV4 | E_UDP;
    traceroute_data.ttl = TRACEROUTE_DEFAULT_MIN_TTL;
    traceroute_data.port_number = TRACEROUTE_DEFAULT_PORT;
    traceroute_data.max_ttl = TRACEROUTE_DEFAULT_MAX_TTL;
    traceroute_data.seq = 0;
}

t_traceroute_data get_traceroute_data() {
    return (traceroute_data);
}

static void *main_opt_func(const char *arg) {
    if (traceroute_data.hostname != NULL) {
        logger->log(E_LOG_ERROR, true, "hostname already set: %s\n", traceroute_data.hostname);
        return ((void *)OPT_ERROR);
    }
    traceroute_data.hostname = (char *)arg;
    return ((void *)OPT_SUCCESS);
}

static void *port_opt_func(const char *arg) {
    if (ft_isnumber(arg) == false) {
        logger->log(E_LOG_ERROR, true, "port must be a number\n");
        return ((void *)OPT_ERROR);
    }
    int port = ft_atoi(arg);
    if (port < 0 || port > 65535) {
        logger->log(E_LOG_ERROR, true, "port must be between 0 and 65535\n");
        return ((void *)OPT_ERROR);
    }
    traceroute_data.port_number = (u_int16_t)port;
    return ((void *)OPT_SUCCESS);
}

static void *ipv6_opt_func(const char *arg) {
    (void)arg;
    traceroute_data.option |= E_IPV6;
    traceroute_data.option &= ~E_IPV4;
    return ((void *)OPT_SUCCESS);
}

static void *icmp_func(const char *arg) {
    (void)arg;
    traceroute_data.option |= E_ICMP;
    traceroute_data.option &= ~E_UDP;
    return ((void *)OPT_SUCCESS);
}

static void *first_ttl(const char *arg)
{
    if (ft_isnumber(arg) == false)
    {
        logger->log(E_LOG_ERROR, true, "ttl must be a number\n");
        return ((void *)OPT_ERROR);
    }
    int ttl = ft_atoi(arg);
    if (ttl < 0 || ttl > 255)
    {
        logger->log(E_LOG_ERROR, true, "ttl must be between 0 and 255\n");
        return ((void *)OPT_ERROR);
    }
    traceroute_data.ttl = (u_int8_t)ttl;
    return ((void *)OPT_SUCCESS);
}

static void *max_ttl(const char *arg)
{
    if (ft_isnumber(arg) == false)
    {
        logger->log(E_LOG_ERROR, true, "ttl must be a number\n");
        return ((void *)OPT_ERROR);
    }
    int ttl = ft_atoi(arg);
    if (ttl < 0 || ttl > 255)
    {
        logger->log(E_LOG_ERROR, true, "ttl must be between 0 and 255\n");
        return ((void *)OPT_ERROR);
    }
    traceroute_data.max_ttl = (u_int8_t)ttl;
    return ((void *)OPT_SUCCESS);
}

int32_t create_option(t_opt_list *opt_lists) {
    logger = get_logger_instance();
    init_traceroute();
    if (opt_set_main(opt_lists, "traceroute: print the route packets take to network host", &main_opt_func) == OPT_ERROR)
        return (TRACEROUTE_ERROR);
    if (opt_add_new('p', "port", "take NUMBER use destination PORT port (default: 33434)", false, &port_opt_func, true, opt_lists) == OPT_ERROR)
        return (TRACEROUTE_ERROR);
    if (opt_add_new('6', "ipv6", "use IPv6", false, &ipv6_opt_func, false, opt_lists) == OPT_ERROR)
        return (TRACEROUTE_ERROR);
    if (opt_add_new('I', "icmp", "use ICMP ECHO instead of UDP SYN (root)", false, &icmp_func, false, opt_lists) == OPT_ERROR)
        return (TRACEROUTE_ERROR);
    if (opt_add_new('f', "first-ttl", "take NUMBER set the first ttl", false, &first_ttl, true, opt_lists) == OPT_ERROR)
        return (TRACEROUTE_ERROR);
    if (opt_add_new('m', "max-ttl", "take NUMBER set the max ttl", false, &max_ttl, true, opt_lists) == OPT_ERROR)
        return (TRACEROUTE_ERROR);
    // debug_opt_list(opt_lists);
    return (TRACEROUTE_SUCCESS);
}

void debug_tracerooute_data(t_traceroute_data *traceroute) {
    logger->log(E_LOG_DEBUG, true, "hostname: %s\n", traceroute->hostname);
    logger->log(E_LOG_DEBUG, true, "ip: %s\n", traceroute->ip);
    logger->log(E_LOG_DEBUG, true, "option: %d\n", traceroute->option);
    logger->log(E_LOG_DEBUG, true, "ip_proto: %d\n", traceroute->ip_proto);
    logger->log(E_LOG_DEBUG, true, "ttl: %d\n", traceroute->ttl);
    logger->log(E_LOG_DEBUG, true, "max_ttl: %d\n", traceroute->max_ttl);
    logger->log(E_LOG_DEBUG, true, "port_number: %d\n", traceroute->port_number);
    logger->log(E_LOG_DEBUG, true, "ICMP: %s\n", traceroute->option & E_ICMP ? "true" : "false");
    logger->log(E_LOG_DEBUG, true, "IPv6: %s\n", traceroute->option & E_IPV6 ? "true" : "false");
}

void print_byte_as_bits(char val) {
  for (int i = 7; 0 <= i; i--) {
    printf("%c", (val & (1 << i)) ? '1' : '0');
  }
}

void print_bits(char * ty, char * val, unsigned char * bytes, size_t num_bytes) {
  printf("(%*s) %*s = [ ", 15, ty, 16, val);
  for (size_t i = 0; i < num_bytes; i++) {
    print_byte_as_bits(bytes[i]);
    printf(" ");
  }
  printf("]\n");
}
