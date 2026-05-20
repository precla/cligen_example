/*
 * Minimal test showing how CLIgen expansion callbacks work
 * This demonstrates the interface() expand function concept
 */

#include <stdio.h>
#include <string.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>

/* Simulated CVec structure (simplified) */
typedef struct {
    char *name;
    char *value;
} cvec_entry;

typedef struct {
    cvec_entry *items;
    int count;
} cvec;

/* Helper to add string to vector */
void cvec_add_string(cvec *v, const char *name, const char *value)
{
    if (v == NULL || value == NULL)
        return;

    v->count++;
    v->items = realloc(v->items, v->count * sizeof(cvec_entry));
    v->items[v->count - 1].name = name ? strdup(name) : NULL;
    v->items[v->count - 1].value = strdup(value);
}

/* THE KEY FUNCTION: Dynamic interface expansion
 *
 * This is called every time user presses TAB on <ifname:string interface()>
 * It queries the OS and returns the list of actual interfaces
 */
int expand_interface(cvec *commands, cvec *helptexts)
{
    struct ifaddrs *ifaddr, *ifa;
    char help[256];

    printf("[expand_interface called at runtime]\n\n");

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    /* Query OS for actual interfaces */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_name == NULL)
            continue;

        /* Skip duplicates */
        int already_added = 0;
        for (int i = 0; i < commands->count; i++) {
            if (strcmp(commands->items[i].value, ifa->ifa_name) == 0) {
                already_added = 1;
                break;
            }
        }
        if (already_added)
            continue;

        /* Add to completion list */
        cvec_add_string(commands, NULL, ifa->ifa_name);

        /* Add status help text */
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

int main()
{
    cvec commands = {0};
    cvec helptexts = {0};

    printf("===== CLIgen Dynamic Expansion Test =====\n\n");

    printf("Simulating: User types 'show interface <TAB>'\n");
    printf("This calls the expand_interface() callback...\n\n");

    /* Call the expansion function (this is what CLIgen does internally) */
    if (expand_interface(&commands, &helptexts) < 0) {
        fprintf(stderr, "Expansion failed\n");
        return 1;
    }

    printf("\nCompletion options found:\n");
    printf("%-20s %s\n", "Interface", "Status");
    printf("%-20s %s\n", "---------", "------");

    for (int i = 0; i < commands.count; i++) {
        printf("%-20s %s\n", commands.items[i].value, helptexts.items[i].value);
    }

    printf("\n[User selects: eth0]\n\n");
    printf("Parsed variables passed to callback:\n");
    printf("  ifname = eth0\n");

    printf("\nCallback would then:\n");
    printf("  1. Call show_interface(ifname='eth0')\n");
    printf("  2. Query interface info (flags, IP, etc.)\n");
    printf("  3. Display the info to user\n");

    /* Cleanup */
    for (int i = 0; i < commands.count; i++) {
        free(commands.items[i].name);
        free(commands.items[i].value);
        free(helptexts.items[i].name);
        free(helptexts.items[i].value);
    }
    free(commands.items);
    free(helptexts.items);

    return 0;
}
