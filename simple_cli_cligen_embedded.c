/*
 * Simple CLI using CLIgen - with embedded network.cli spec
 * Demonstrates compiling CLI specification into the binary
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cligen/cligen.h>
#ifndef TREE
#include <cligen/cligen_buf.h>
#endif
#include "network_spec.h"

#define TREE 0

#define MAX_INTERFACES 1024

typedef struct {
    char name[256];
    char status[64];
    char ip[INET_ADDRSTRLEN];
    char ip6[INET6_ADDRSTRLEN];
} interface_info;

static cligen_handle g_cli_handle = NULL;
interface_info interfaces[MAX_INTERFACES];
int interface_count = 0;

/* Get list of interfaces dynamically from OS */
int get_interfaces(int max_count)
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

        bool already_added = false;
        for (int i = 0; i < count; i++) {
            if (strcmp(interfaces[i].name, ifa->ifa_name) == 0) {
                already_added = true;
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
        strcpy(interfaces[count].ip6, "N/A");
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
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            for (int i = 0; i < count; i++) {
                if (strcmp(interfaces[i].name, ifa->ifa_name) == 0) {
                    inet_ntop(AF_INET6, &sin6->sin6_addr, interfaces[i].ip6, INET6_ADDRSTRLEN);
                    break;
                }
            }
        }
    }

    freeifaddrs(ifaddr);
    interface_count = count;

    return 0;
}

/* ===== EXPAND CALLBACKS ===== */

int expand_interface(cligen_handle h,
                     const char   *fn_str,
                     cvec         *cvv,
                     cvec         *argv,
                     cvec         *commands,
                     cvec         *helptexts)
{
    int error = 0;
    char help[512];

    error = get_interfaces(MAX_INTERFACES);
    if (error) {
        cligen_output(stderr, "Error getting interfaces\n");
        return -1;
    }

    if (interface_count < 0) {
        cligen_output(stdout, "No interfaces available\n");
        return -1;
    }

    for (int i = 0; i < interface_count; i++) {
        cvec_add_string(commands, NULL, interfaces[i].name);

        snprintf(help, sizeof(help), "%s (%s) '%s' '%s'", interfaces[i].name, interfaces[i].status, interfaces[i].ip, interfaces[i].ip6);
        cvec_add_string(helptexts, NULL, help);
    }

    return 0;
}

/* ===== COMMAND CALLBACKS ===== */

/* Cisco-style mode switch: change the active parse tree and the prompt.
 *   argv[0] = name of the tree to make active (the new "mode")
 *   argv[1] = prompt string to display while in that mode
 */
int mode(cligen_handle h, cvec *cvv, cvec *argv)
{
    char *treename;
    char *prompt;

    treename = cv_string_get(cvec_i(argv, 0));
    prompt = cv_string_get(cvec_i(argv, 1));

    if (cligen_ph_active_set_byname(h, treename) < 0) {
        cligen_output(stderr, "Error: no such mode '%s'\n", treename);
        return -1;
    }

    cligen_prompt_set(h, prompt);

    return 0;
}

int show_interfaces(cligen_handle h, cvec *cvv, cvec *argv)
{
    int error = 0;

    error = get_interfaces(MAX_INTERFACES);
    if (error) {
        cligen_output(stderr, "Error getting interfaces\n");
        goto error_out;
    }

    if (interface_count < 0) {
        cligen_output(stdout, "No interfaces available\n");
        goto error_out;
    }

    cligen_output(stdout, "\nInterface            Status       IP Address\n");

    for (int i = 0; i < interface_count; i++) {
        cligen_output(stdout, "%-18s %-10s %-21s\n", interfaces[i].name, interfaces[i].status, interfaces[i].ip);
    }

    cligen_output(stdout, "\n");

    goto out;

error_out:
    error = -1;

out:
    return error ? -1 : 0;
}

int show_interface(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv;
    char *ifname;

    cv = cvec_find(cvv, "ifname");
    if (!cv) {
        cligen_output(stderr, "Error: interface name not found\n");
        return -1;
    }

    ifname = cv_string_get(cv);

    if (interface_count < 0) {
        cligen_output(stdout, "No interfaces available\n");
        return -1;
    }
    cligen_output(stdout, "\n");
    cligen_output(stdout, "Interface: %-24s\n", ifname);

    for (int i = 0; i < interface_count; i++) {
        if (strcmp(interfaces[i].name, ifname) == 0) {
            cligen_output(stdout, "Status: %-27s\n", interfaces[i].status);
            cligen_output(stdout, "IP Address: %-23s\n", interfaces[i].ip);
            cligen_output(stdout, "IPv6 Address: %-23s\n", interfaces[i].ip6);
            break;
        }
    }

    return 0;
}

int show_history(cligen_handle h, cvec *cvv, cvec *argv)
{
    cligen_hist_file_save(g_cli_handle, stdout);

    return 0;
}


int set_interface_ip(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv;
    char *ifname;
    struct in_addr *ipaddr;

    cv = cvec_find(cvv, "ifname");
    if (!cv) {
        cligen_output(stderr, "Error: interface name not found\n");
        return -1;
    }
    ifname = cv_string_get(cv);

    cv = cvec_find(cvv, "ipaddr");
    if (!cv) {
        cligen_output(stderr, "Error: IP address not found\n");
        return -1;
    }
    ipaddr = cv_ipv4addr_get(cv);

    cligen_output(stdout, "\nIP Configuration\n");
    cligen_output(stdout, "  Interface: %s\n", ifname);
    cligen_output(stdout, "  IP Address: %s\n", inet_ntoa(*ipaddr));
    cligen_output(stdout, "  Status: Would be applied\n\n");
    cligen_output(stdout, "  [Note: This is a simulation. In a real system,\n");
    cligen_output(stdout, "   this would run: ip addr add %s/24 dev %s]\n\n", inet_ntoa(*ipaddr), ifname);

    return 0;
}

int set_interface_ipv6(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv;
    char *ifname;
    struct in6_addr *ipv6addr;
    char buffer[128] = {0};

    cv = cvec_find(cvv, "ifname");
    if (!cv) {
        cligen_output(stderr, "Error: interface name not found\n");
        return -1;
    }
    ifname = cv_string_get(cv);

    cv = cvec_find(cvv, "ipaddr");
    if (!cv) {
        cligen_output(stderr, "Error: IP address not found\n");
        return -1;
    }
    ipv6addr = cv_ipv6addr_get(cv);

    inet_ntop(AF_INET6, ipv6addr, buffer, sizeof(buffer));

    cligen_output(stdout, "\nIP Configuration\n");
    cligen_output(stdout, "  Interface: %s\n", ifname);
    cligen_output(stdout, "  IPv6 Address: %s\n", buffer);
    cligen_output(stdout, "  Status: Would be applied\n");
    cligen_output(stdout, "  [Note: This is a simulation]\n");

    return 0;
}

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

    cligen_output(stdout, "\nIP Removed\n");
    cligen_output(stdout, "  Interface: %s\n", ifname);
    cligen_output(stdout, "  Status: IP address would be removed\n\n");
    cligen_output(stdout, "  [Note: This would run: ip addr del <current-ip> dev %s]\n\n", ifname);

    return 0;
}

int remove_interface_ipv6(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv;
    char *ifname;

    cv = cvec_find(cvv, "ifname");
    if (!cv) {
        cligen_output(stderr, "Error: interface name not found\n");
        return -1;
    }
    ifname = cv_string_get(cv);

    cligen_output(stdout, "\nIP Removed\n");
    cligen_output(stdout, "  Interface: %s\n", ifname);
    cligen_output(stdout, "  Status: IP address would be removed\n\n");
    cligen_output(stdout, "  [Note: This would run: ip addr del <current-ip> dev %s]\n\n", ifname);

    return 0;
}

int set_mtu(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv;
    char *ifname;
    uint16_t mtu = 0;

    cv = cvec_find(cvv, "ifname");
    if (!cv) {
        cligen_output(stderr, "Error: interface name not found\n");
        return -1;
    }
    ifname = cv_string_get(cv);

    cv = cvec_find(cvv, "size");
    if (!cv) {
        cligen_output(stderr, "Error: IP address not found\n");
        return -1;
    }
    mtu = cv_uint16_get(cv);

    cligen_output(stdout, "\nMTU Configuration\n");
    cligen_output(stdout, "  Interface: %s\n", ifname);
    cligen_output(stdout, "  MTU: %d\n", mtu);
    cligen_output(stdout, "  Status: Would be applied\n");
    cligen_output(stdout, "  [Note: This is a simulation]\n");

    return 0;
}

#ifndef TREE
static void print_help_recursive(cbuf *cb, cg_obj *co, int depth)
{
    if (!co || (co->co_flags & CO_FLAGS_HIDE))
        return;

    size_t saved_len = cbuf_len(cb);

    if (depth > 0)
        cprintf(cb, " ");

    if (co->co_type == CO_VARIABLE) {
        const char *typestr = cv_type2str(co->co_vtype);
        cprintf(cb, "<%s:%s>", co->co_command ? co->co_command : "?", typestr ? typestr : "?");
    } else if (co->co_command) {
        cprintf(cb, "%s", co->co_command);
    }

    int is_leaf = (co->co_callbacks != NULL);

    if (is_leaf) {
        const char *cmd = cbuf_get(cb);
        size_t cmd_len = cbuf_len(cb);
        const char *help = co->co_helpstring;

        size_t pad = COLUMN_MIN_WIDTH;
        if (cmd_len > pad)
            pad = ((cmd_len / COLUMN_MIN_WIDTH) + 1) * COLUMN_MIN_WIDTH;

        cligen_output(stdout, "  %-*s%s\n", (int)pad, cmd, help ? help : "");
    }

    if (!is_leaf) {
        for (int i = 0; i < co->co_pt_len; i++) {
            parse_tree *child_pt = co->co_ptvec[i];
            if (!child_pt)
                continue;
            int len = pt_len_get(child_pt);
            for (int j = 0; j < len; j++) {
                cg_obj *child = pt_vec_i_get(child_pt, j);
                if (child)
                    print_help_recursive(cb, child, depth + 1);
            }
        }
    }

    cbuf_trunc(cb, saved_len);
}
#endif

int help_cmd(cligen_handle h, cvec *cvv, cvec *argv)
{
#ifndef TREE
    cbuf *cb = cbuf_new();
    if (!cb)
        return -1;
#endif

    pt_head *head = NULL;
    while ((head = cligen_ph_each(h, head)) != NULL) {
#ifdef TREE
        cligen_help(h, stdout, cligen_ph_parsetree_get(head));
#endif
#ifndef TREE
        parse_tree *pt = cligen_ph_parsetree_get(head);
        if (!pt)
            continue;
        int len = pt_len_get(pt);
        for (int i = 0; i < len; i++) {
            cg_obj *co = pt_vec_i_get(pt, i);
            if (co)
                print_help_recursive(cb, co, 0);
        }
#endif
    }

#ifndef TREE
    cbuf_free(cb);
#endif
    return 0;
}

int quit_cmd(cligen_handle h, cvec *cvv, cvec *argv)
{
    cligen_exiting_set(h, 1);
    return 0;
}

/* ===== CALLBACK REGISTRATION ===== */

cgv_fnstype_t *str2fn(const char *name, void *arg, char **error)
{
    *error = NULL;

    if (strcmp(name, "mode") == 0)
        return mode;
    if (strcmp(name, "show_interfaces") == 0)
        return show_interfaces;
    if (strcmp(name, "show_interface") == 0)
        return show_interface;
    if (strcmp(name, "show_history") == 0)
        return show_history;
    if (strcmp(name, "set_interface_ip") == 0)
        return set_interface_ip;
    if (strcmp(name, "remove_interface_ip") == 0)
        return remove_interface_ip;
    if (strcmp(name, "set_interface_ipv6") == 0)
        return set_interface_ipv6;
    if (strcmp(name, "remove_interface_ipv6") == 0)
        return remove_interface_ipv6;
    if (strcmp(name, "set_mtu") == 0)
        return set_mtu;
    if (strcmp(name, "help_cmd") == 0)
        return help_cmd;
    if (strcmp(name, "quit_cmd") == 0)
        return quit_cmd;

    cligen_output(stderr, "Unknown callback function: %s\n", name);
    return NULL;
}

static expand_cb *str2fn_expand(const char *name, void *arg, char **error)
{
    *error = NULL;

    if (strcmp(name, "interface") == 0)
        return (expand_cb *)expand_interface;

    return NULL;
}

/* ===== SIGNAL HANDLERS ===== */

static void sigint_handler(int sig)
{
    if (g_cli_handle)
        cligen_exit(g_cli_handle);

    cligen_output(stdout, "Goodbye!\n\n");
    exit(0);
}

/* ===== MAIN PROGRAM ===== */

int main(void)
{
    int error = 0;    
    parse_tree *tree = NULL;
    pt_head *head = NULL;
    cvec *globals = NULL;
    const char * prompt = NULL;

    cligen_output(stdout, "\n╔════════════════════════════════════════════════════════════╗\n");
    cligen_output(stdout, "║                   Network Configuration CLI                ║\n");
    cligen_output(stdout, "║                                                            ║\n");
    cligen_output(stdout, "║  Features:                                                 ║\n");
    cligen_output(stdout, "║  • CLI spec compiled into binary (no network.cli needed)   ║\n");
    cligen_output(stdout, "║  • Tab completion for interface names                      ║\n");
    cligen_output(stdout, "║  • Type validation (IPv4/v6 addresses)                     ║\n");
    cligen_output(stdout, "║  • Command history and editing                             ║\n");
    cligen_output(stdout, "║                                                            ║\n");
    cligen_output(stdout, "║  Type 'help' for available commands                        ║\n");
    cligen_output(stdout, "╚════════════════════════════════════════════════════════════╝\n\n");

    g_cli_handle = cligen_init();
    if (!g_cli_handle) {
        cligen_output(stderr, "Failed to initialize CLIgen\n");
        return 1;
    }

    signal(SIGINT, sigint_handler);

    cligen_hist_init(g_cli_handle, 10);

    /* Parse CLI specification from string */
    globals = cvec_new(0);
    if (clispec_parse_str(g_cli_handle, embedded_cli_spec, "qn-cli", NULL, NULL, globals) < 0) {
        cligen_output(stderr, "Error: Failed to parse embedded CLI spec\n");
        goto error_out;
    }

    if ((prompt = cvec_find_str(globals, "prompt")) != NULL) {
        cligen_prompt_set(g_cli_handle, prompt);
    }

    /* Register callbacks and expands for all parse trees */
    head = NULL;
    while ((head = cligen_ph_each(g_cli_handle, head)) != NULL) {
        tree = cligen_ph_parsetree_get(head);

        if (cligen_callbackv_str2fn(tree, str2fn, NULL) < 0) {
            cligen_output(stderr, "Error: Failed to register callbacks\n");
            goto error_out;
        }

        if (cligen_expand_str2fn(tree, str2fn_expand, NULL) < 0) {
            cligen_output(stderr, "Error: Failed to register expand functions\n");
            goto error_out;
        }
    }

    cvec_free(globals);
    globals = NULL;

    /* Run the CLI */
    if (cligen_loop(g_cli_handle) < 0) {
        cligen_output(stderr, "Error in CLI loop\n");
        goto error_out;
    }

    goto out;

error_out:
    error = -1;

out:
    cvec_free(globals);
    cligen_exit(g_cli_handle);

    return error ? -1 : 0;
}
