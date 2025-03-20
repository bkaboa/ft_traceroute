#include "traceroute.h"

static t_traceroute_data traceroute;
static Logger *logger;

static void init_traceroute() {
    traceroute.hostname = NULL;
    traceroute.ip = NULL;
    traceroute.option = 0;
    traceroute.ttl = 0;
}

t_traceroute_data get_traceroute_data() {
    return (traceroute);
}

static void *main_opt_func(const char *arg) {
    if (traceroute.hostname != NULL) {
        logger->log(E_LOG_ERROR, true, "hostname already set: %s\n", traceroute.hostname);
        return ((void *)OPT_ERROR);
    }
    traceroute.hostname = (char *)arg;
}

static void *port_opt_func(const char *arg) {
    if (ft_isnumber(arg) == false) {
        logger->log(E_LOG_ERROR, true, "port must be a number\n");
        return ((void *)OPT_ERROR);
    }
    traceroute.option = ft_atoi(arg);
}

int32_t create_option(t_opt_list *opt_lists) {
    logger = get_logger_instance();
    init_traceroute();
    if (opt_set_main(opt_lists, "traceroute: print the route packets take to network host", &main_opt_func) == OPT_ERROR)
        return (TRACEROUTE_ERROR);
    if (opt_add_new('p', "port", "use destination PORT port (default: 33434)", false, NULL, true, *opt_lists) == OPT_ERROR)
        return (TRACEROUTE_ERROR);
}