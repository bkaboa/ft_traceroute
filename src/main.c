#include "traceroute.h"

int main(int argc, char *argv[]) {
    t_opt_list opt_lists = {0};

    if (create_option(&opt_lists) == OPT_ERROR) {
        dprintf(2, "Error: create_option\n");
        return (EXIT_FAILURE);
    }

    if (ft_getopt((const char **)argv, argc, opt_lists) == OPT_ERROR)
        return (EXIT_FAILURE);
}