#include "traceroute.h"

int main(int argc, char *argv[]) {
    t_opt_list opt_lists = {0};
    t_traceroute traceroute = {0};
    Logger *logger = get_logger_instance();

    if (create_option(&opt_lists) == OPT_ERROR) {
        logger->log(E_LOG_ERROR, true, "create_option: %s\n", gai_strerror(errno));
        return (EXIT_FAILURE);
    }
    if (ft_getopt((const char **)argv, argc, opt_lists) == OPT_ERROR) {
        return (EXIT_FAILURE);
    }
    opt_destroy(opt_lists);
    traceroute.traceroute_data = get_traceroute_data();
    if (traceroute.traceroute_data.option & E_IPV4) {
        if (init_socket4(&traceroute) == TRACEROUTE_ERROR) {
            return (EXIT_FAILURE);
        }
    } else {
        if (init_socket6(&traceroute) == TRACEROUTE_ERROR) {
            return (EXIT_FAILURE);
        }
    }
    traceroute_routine(&traceroute);
    // freeaddrinfo(traceroute.traceroute_socket.addr);
}