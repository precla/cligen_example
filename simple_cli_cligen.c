/*
 * Simple CLI using real CLIgen functions
 * Uses clispec_parse_file() and cligen_loop() for proper CLIgen integration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cligen/cligen.h>

typedef struct {
    char name[256];
    char status[256];
    char ip[INET_ADDRSTRLEN];
} interface_info;

/* Global interface list */
static interface_info g_interfaces[100];
static int g_interface_count = 0;

/* Get list of interfaces dynamically from OS */
int get_interfaces(interface_info *interfaces, int max_count)
{
    struct ifaddrs *ifaddr, *ifa;
    int count = 0;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    /* First pass: add all interfaces */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_name == NULL || count >= max_count)
            continue;

        int already_added = 0;
        for (int i = 0; i < count; i++) {
            if (strcmp(interfaces[i].name, ifa->ifa_name) == 0) {
                already_added = 1;
                break;
            }
        }
        if (already_added)
            continue;

        strncpy(interfaces[count].name, ifa->ifa_name, sizeof(interfaces[count].name) - 1);

        if (ifa->ifa_flags & IFF_UP) {
            strcpy(interfaces[count].status, "UP");
        } else {
            strcpy(interfaces[count].status, "DOWN");
        }

        strcpy(interfaces[count].ip, "N/A");
        count++;
    }

    /* Second pass: populate IP addresses */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_name == NULL || ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sin = (struct sockaddr_in *)ifa->ifa_addr;

            for (int i = 0; i < count; i++) {
                if (strcmp(interfaces[i].name, ifa->ifa_name) == 0) {
                    inet_ntop(AF_INET, &sin->sin_addr, interfaces[i].ip, INET_ADDRSTRLEN);
                    break;
                }
            }
        }
    }

    freeifaddrs(ifaddr);
    return count;
}

/* ===== EXPAND CALLBACKS ===== */

/* Expand function for interface names */
int expand_interface(cligen_handle h,
                     const char   *fn_str,
                     cvec         *cvv,
                     cvec         *argv,
                     cvec         *commands,
                     cvec         *helptexts)
{
    /* Load interfaces dynamically */
    g_interface_count = get_interfaces(g_interfaces, 100);
    if (g_interface_count < 0)
        return -1;

    /* Add each interface to completion list */
    for (int i = 0; i < g_interface_count; i++) {
        cvec_add_string(commands, NULL, g_interfaces[i].name);

        char help[512];
        snprintf(help, sizeof(help), "%s (%s) %s",
                 g_interfaces[i].name,
                 g_interfaces[i].status,
                 g_interfaces[i].ip);
        cvec_add_string(helptexts, NULL, help);
    }

    return 0;
}

/* ===== COMMAND CALLBACKS ===== */

/* Show all interfaces */
int show_interfaces(cligen_handle h, cvec *cvv, cvec *argv)
{
    cligen_output(stdout, "\n");
    cligen_output(stdout, "╔════════════════════╦════════════╦═══════════════════════╗\n");
    cligen_output(stdout, "║ Interface          ║ Status     ║ IP Address            ║\n");
    cligen_output(stdout, "╠════════════════════╬════════════╬═══════════════════════╣\n");

    for (int i = 0; i < g_interface_count; i++) {
        cligen_output(stdout, "║ %-18s ║ %-10s ║ %-21s ║\n",
                   g_interfaces[i].name,
                   g_interfaces[i].status,
                   g_interfaces[i].ip);
    }

    cligen_output(stdout, "╚════════════════════╩════════════╩═══════════════════════╝\n");
    cligen_output(stdout, "\n");

    return 0;
}

/* Show specific interface */
int show_interface(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv;
    char *ifname;

    /* Get interface name from parsed variables */
    cv = cvec_find(cvv, "ifname");
    if (!cv) {
        cligen_output(stderr, "Error: interface name not found\n");
        return -1;
    }

    ifname = cv_string_get(cv);

    cligen_output(stdout, "\n");
    cligen_output(stdout, "╔════════════════════════════════════╗\n");
    cligen_output(stdout, "║ Interface: %-24s ║\n", ifname);

    /* Find and display interface info */
    for (int i = 0; i < g_interface_count; i++) {
        if (strcmp(g_interfaces[i].name, ifname) == 0) {
            cligen_output(stdout, "║ Status: %-27s ║\n", g_interfaces[i].status);
            cligen_output(stdout, "║ IP Address: %-23s ║\n", g_interfaces[i].ip);
            break;
        }
    }

    cligen_output(stdout, "╚════════════════════════════════════╝\n\n");

    return 0;
}

/* Set interface IP address */
int set_interface_ip(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv;
    char *ifname;
    struct in_addr *ipaddr;

    /* Get interface name */
    cv = cvec_find(cvv, "ifname");
    if (!cv) {
        cligen_output(stderr, "Error: interface name not found\n");
        return -1;
    }
    ifname = cv_string_get(cv);

    /* Get IP address - CLIgen validated it as ipv4addr type */
    cv = cvec_find(cvv, "ipaddr");
    if (!cv) {
        cligen_output(stderr, "Error: IP address not found\n");
        return -1;
    }
    ipaddr = cv_ipv4addr_get(cv);

    cligen_output(stdout, "\n✓ IP Configuration\n");
    cligen_output(stdout, "  Interface: %s\n", ifname);
    cligen_output(stdout, "  IP Address: %s\n", inet_ntoa(*ipaddr));
    cligen_output(stdout, "  Status: Would be applied\n\n");
    cligen_output(stdout, "  [Note: This is a simulation. In a real system,\n");
    cligen_output(stdout, "   this would run: ip addr add %s/24 dev %s]\n\n", inet_ntoa(*ipaddr), ifname);

    return 0;
}

/* Remove interface IP address */
int remove_interface_ip(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv;
    char *ifname;

    cv = cvec_find(cvv, "ifname");
    if (!cv) {
        cligen_output(stderr, "Error: interface name not found\n");
        return -1;
    }
    ifname = cv_string_get(cv);

    cligen_output(stdout, "\n✓ IP Removed\n");
    cligen_output(stdout, "  Interface: %s\n", ifname);
    cligen_output(stdout, "  Status: IP address would be removed\n\n");
    cligen_output(stdout, "  [Note: This would run: ip addr del <current-ip> dev %s]\n\n", ifname);

    return 0;
}

/* Help command */
int help_cmd(cligen_handle h, cvec *cvv, cvec *argv)
{
    cligen_output(stdout, "\nAvailable commands:\n");
    cligen_output(stdout, "  show interfaces             - Show all network interfaces\n");
    cligen_output(stdout, "  show interface <name>       - Show specific interface details\n");
    cligen_output(stdout, "  config interface <name> ip <addr> - Set IP address on interface\n");
    cligen_output(stdout, "  config interface <name> no ip    - Remove IP from interface\n");
    cligen_output(stdout, "  help                        - Show this help message\n");
    cligen_output(stdout, "  quit                        - Exit the CLI\n\n");
    cligen_output(stdout, "Try: config interface <TAB>  to see available interfaces!\n\n");

    return 0;
}

/* Quit command */
int quit_cmd(cligen_handle h, cvec *cvv, cvec *argv)
{
    cligen_exiting_set(h, 1);
    return 0;
}

/* ===== CALLBACK REGISTRATION ===== */

/* Map function names to callback functions */
cgv_fnstype_t *str2fn(const char *name, void *arg, char **error)
{
    *error = NULL;

    if (strcmp(name, "show_interfaces") == 0)
        return show_interfaces;
    if (strcmp(name, "show_interface") == 0)
        return show_interface;
    if (strcmp(name, "set_interface_ip") == 0)
        return set_interface_ip;
    if (strcmp(name, "remove_interface_ip") == 0)
        return remove_interface_ip;
    if (strcmp(name, "help_cmd") == 0)
        return help_cmd;
    if (strcmp(name, "quit_cmd") == 0)
        return quit_cmd;

    cligen_output(stderr, "Unknown callback function: %s\n", name);
    return NULL;
}

/* Map expand function names to expand callbacks */
expand_cb *str2fn_expand(const char *name, void *arg, char **error)
{
    *error = NULL;

    if (strcmp(name, "interface") == 0)
        return expand_interface;

    return NULL;
}

/* ===== MAIN PROGRAM ===== */

int main()
{
    cligen_handle h;
    parse_tree *pt;
    pt_head *ph;
    FILE *f;
    cvec *globals;

    cligen_output(stdout, "\n");
    cligen_output(stdout, "╔════════════════════════════════════════════════════════════╗\n");
    cligen_output(stdout, "║  Network Configuration CLI - Using CLIgen                  ║\n");
    cligen_output(stdout, "║                                                            ║\n");
    cligen_output(stdout, "║  Features:                                                 ║\n");
    cligen_output(stdout, "║  • Tab completion for interface names (cliread + expand)   ║\n");
    cligen_output(stdout, "║  • Type validation (IPv4 addresses via ipv4addr type)      ║\n");
    cligen_output(stdout, "║  • Command history (arrow keys)                            ║\n");
    cligen_output(stdout, "║  • Dynamic interface discovery (getifaddrs)                ║\n");
    cligen_output(stdout, "║                                                            ║\n");
    cligen_output(stdout, "║  Type 'help' for available commands                        ║\n");
    cligen_output(stdout, "╚════════════════════════════════════════════════════════════╝\n\n");

    /* Initialize CLIgen */
    h = cligen_init();
    if (!h) {
        cligen_output(stderr, "Failed to initialize CLIgen\n");
        return 1;
    }

    /* Load CLI specification from file */
    f = fopen("network.cli", "r");
    if (!f) {
        cligen_output(stderr, "Error: Cannot open network.cli\n");
        cligen_output(stderr, "Make sure you're in /home/sartura/aprcela/cligen_example/\n");
        cligen_exit(h);
        return 1;
    }

    /* Parse CLI specification */
    globals = cvec_new(0);
    if (clispec_parse_file(h, f, "network.cli", NULL, NULL, globals) < 0) {
        cligen_output(stderr, "Error: Failed to parse network.cli\n");
        fclose(f);
        cvec_free(globals);
        cligen_exit(h);
        return 1;
    }
    fclose(f);

    /* Register callbacks and expands for all parse trees */
    ph = NULL;
    while ((ph = cligen_ph_each(h, ph)) != NULL) {
        pt = cligen_ph_parsetree_get(ph);

        if (cligen_callbackv_str2fn(pt, str2fn, NULL) < 0) {
            cligen_output(stderr, "Error: Failed to register callbacks\n");
            cvec_free(globals);
            cligen_exit(h);
            return 1;
        }

        if (cligen_expand_str2fn(pt, str2fn_expand, NULL) < 0) {
            cligen_output(stderr, "Error: Failed to register expand functions\n");
            cvec_free(globals);
            cligen_exit(h);
            return 1;
        }
    }

    cvec_free(globals);

    /* Load interfaces at startup */
    g_interface_count = get_interfaces(g_interfaces, 100);
    if (g_interface_count < 0) {
        cligen_output(stderr, "Warning: Failed to load interfaces\n");
    }

    /* Run the CLI - this handles cliread, tab completion, history, everything! */
    if (cligen_loop(h) < 0) {
        cligen_output(stderr, "Error in CLI loop\n");
        cligen_exit(h);
        return 1;
    }

    cligen_output(stdout, "Goodbye!\n\n");

    /* Cleanup */
    cligen_exit(h);

    return 0;
}
