#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <cligen/cligen.h>

/* Callback: dynamically expand available interfaces */
int
expand_interface(cligen_handle h,
                 const char   *fn_str,
                 cvec         *cvv,
                 cvec         *argv,
                 cvec         *commands,
                 cvec         *helptexts)
{
    struct ifaddrs *ifaddr, *ifa;
    char help[256];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    /* Iterate through all interfaces */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_name == NULL)
            continue;

        /* Skip duplicates (each interface has multiple address families) */
        if (commands && cvec_find_str(commands, NULL, ifa->ifa_name) != NULL)
            continue;

        /* Add interface name and help text */
        cvec_add_string(commands, NULL, ifa->ifa_name);

        /* Generate help based on interface flags */
        if (ifa->ifa_flags & IFF_UP) {
            snprintf(help, sizeof(help), "%s (UP)", ifa->ifa_name);
        } else {
            snprintf(help, sizeof(help), "%s (DOWN)", ifa->ifa_name);
        }
        cvec_add_string(helptexts, NULL, help);
    }

    freeifaddrs(ifaddr);
    return 0;
}

/* Callback: show interface info */
int
show_interface(cligen_handle h,
               cvec         *cvv,
               cvec         *argv)
{
    char *ifname;
    struct ifaddrs *ifaddr, *ifa;
    char ip_str[INET_ADDRSTRLEN];

    /* Get interface name from parsed input */
    ifname = cvec_find_str(cvv, "ifname");
    if (ifname == NULL) {
        fprintf(stderr, "Error: interface name not found\n");
        return -1;
    }

    printf("\n=== Interface: %s ===\n", ifname);

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    /* Find and display info for this interface */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, ifname) == 0) {
            /* Display flags */
            printf("Flags: ");
            if (ifa->ifa_flags & IFF_UP) printf("UP ");
            if (ifa->ifa_flags & IFF_RUNNING) printf("RUNNING ");
            if (ifa->ifa_flags & IFF_LOOPBACK) printf("LOOPBACK ");
            printf("\n");

            /* Display IP address if IPv4 */
            if (ifa->ifa_addr->sa_family == AF_INET) {
                struct sockaddr_in *sin = (struct sockaddr_in *)ifa->ifa_addr;
                inet_ntop(AF_INET, &sin->sin_addr, ip_str, INET_ADDRSTRLEN);
                printf("IP Address: %s\n", ip_str);
            }
        }
    }

    freeifaddrs(ifaddr);
    printf("\n");
    return 0;
}

/* Callback: set MTU */
int
set_mtu(cligen_handle h,
        cvec         *cvv,
        cvec         *argv)
{
    char *ifname;
    int mtu;
    cg_var *cv;

    ifname = cvec_find_str(cvv, "ifname");
    cv = cvec_find(cvv, "size");
    if (ifname == NULL || cv == NULL) {
        fprintf(stderr, "Error: interface or MTU not found\n");
        return -1;
    }

    mtu = cv_int32_get(cv);
    printf("Setting %s MTU to %d\n", ifname, mtu);
    printf("(This would call: ip link set %s mtu %d)\n\n", ifname, mtu);

    return 0;
}

/* Map function strings to actual C callbacks */
cgv_fnstype_t *
str2fn(const char *name, void *arg, char **error)
{
    *error = NULL;
    if (strcmp(name, "show_interface") == 0)
        return show_interface;
    if (strcmp(name, "set_mtu") == 0)
        return set_mtu;
    fprintf(stderr, "Unknown function: %s\n", name);
    return NULL;
}

/* Map expand function strings */
expand_cb *
str2fn_exp(const char *name, void *arg, char **error)
{
    if (strcmp(name, "interface") == 0)
        return expand_interface;
    return NULL;
}

int
main(int argc, char **argv)
{
    cligen_handle h;
    parse_tree *pt;
    FILE *f;

    /* Create CLIgen handle */
    h = cligen_init();
    if (h == NULL) {
        fprintf(stderr, "Failed to initialize CLIgen\n");
        return -1;
    }

    /* Set callback functions */
    cligen_callback_register(h, str2fn, NULL);
    cligen_expand_register(h, str2fn_exp, NULL);

    /* Read CLI specification */
    f = fopen("ifconfig_cli.cli", "r");
    if (f == NULL) {
        fprintf(stderr, "Failed to open ifconfig_cli.cli\n");
        cligen_exit(h);
        return -1;
    }

    /* Parse CLI spec into parse tree */
    if (cligen_parse_file(h, f, "ifconfig_cli.cli") < 0) {
        fprintf(stderr, "Failed to parse CLI spec\n");
        fclose(f);
        cligen_exit(h);
        return -1;
    }
    fclose(f);

    /* Get the parse tree and run interactive CLI */
    pt = cligen_ph_parse_tree(h, "tutorial");
    if (pt == NULL) {
        fprintf(stderr, "Failed to get parse tree\n");
        cligen_exit(h);
        return -1;
    }

    printf("Interface Configuration CLI\n");
    printf("Type 'show interface <TAB>' to see available interfaces\n");
    printf("Type 'help' for commands\n\n");

    /* Run the interactive CLI */
    cligen_loop(h, pt);

    /* Cleanup */
    cligen_exit(h);
    return 0;
}
