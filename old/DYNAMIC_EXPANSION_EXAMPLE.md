# CLIgen Dynamic Interface Expansion Example

This shows how CLIgen dynamically expands available interfaces at runtime.

## How It Works

### 1. CLI Specification (`ifconfig_cli.cli`)

```cli
show("Display interface info"){
  interface <ifname:string interface()>("Interface name"), show_interface();
}
```

The `interface()` function tells CLIgen: *"When user presses TAB here, call the expand_interface callback to get the list of available options"*

### 2. Expansion Callback (C Implementation)

This function is called every time the user presses TAB on that parameter:

```c
int expand_interface(cligen_handle h,
                     const char   *fn_str,
                     cvec         *cvv,
                     cvec         *argv,
                     cvec         *commands,     /* output: list of options */
                     cvec         *helptexts)    /* output: help for each option */
{
    struct ifaddrs *ifaddr, *ifa;
    char help[256];

    /* Query the operating system for actual interfaces */
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

        /* Add interface name to completion list */
        cvec_add_string(commands, NULL, ifa->ifa_name);

        /* Add help text (e.g., show if interface is UP or DOWN) */
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
```

### 3. User Interaction

When the user runs the program:

```
ifconfig> show interface <TAB>
eth0    (UP)
eth1    (DOWN)
lo      (UP)
wlan0   (UP)

ifconfig> show interface eth0
=== Interface: eth0 ===
Flags: UP RUNNING
IP Address: 192.168.1.100
```

**The magic**: The interface list is queried **at runtime** when the user presses TAB, not hardcoded into the CLI definition.

### 4. Why This Matters

**Without CLIgen** (manual parsing):
```bash
$ show eth0  # You hardcode which interfaces exist
$ show eth1  # If you add a new interface, you must update your code
$ show wlan0 # Rebuild and redeploy
```

**With CLIgen dynamic expansion**:
```bash
$ show <TAB>  # Automatically discovers eth0, eth1, wlan0, etc.
              # No code changes needed - just runs getifaddrs()
```

## Complete Working Code

See the files created in this directory:
- `ifconfig_cli.cli` - CLI syntax specification
- `ifconfig_cli.c` - Full C implementation with expansion callbacks

### To compile (requires cligen library):

```bash
gcc -o ifconfig_cli ifconfig_cli.c -lcligen
```

### To use:

```bash
./ifconfig_cli
ifconfig> show interface <TAB>  # See your actual network interfaces
ifconfig> show interface eth0   # Display interface info
ifconfig> set interface eth0 mtu 1500  # Configure interface
```

## Key Concepts

| Concept | Meaning |
|---------|---------|
| `<ifname:string interface()>` | A string parameter that calls interface() to expand |
| `expand_interface()` | Called every time user presses TAB on that parameter |
| `cvec_add_string(commands, ...)` | Add option to completion list |
| `cvec_add_string(helptexts, ...)` | Add help text for that option |
| `getifaddrs()` | Query OS for actual interfaces (dynamic data source) |

## Advanced: Other Expansion Sources

You can expand from any source:

```c
/* Expand from database query */
int expand_users(cligen_handle h, ..., cvec *commands, cvec *helptexts) {
    sqlite3_stmt *stmt;
    const char *username;
    
    sqlite3_prepare_v2(db, "SELECT username FROM users", -1, &stmt, NULL);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        username = (const char*)sqlite3_column_text(stmt, 0);
        cvec_add_string(commands, NULL, username);
        cvec_add_string(helptexts, NULL, "Database user");
    }
    sqlite3_finalize(stmt);
    return 0;
}
```

```c
/* Expand from file listing */
int expand_files(cligen_handle h, ..., cvec *commands, cvec *helptexts) {
    DIR *dir = opendir("/etc");
    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            cvec_add_string(commands, NULL, entry->d_name);
        }
    }
    closedir(dir);
    return 0;
}
```

```c
/* Expand from remote API */
int expand_docker_containers(cligen_handle h, ..., cvec *commands, cvec *helptexts) {
    // Call Docker API to get container list
    // Parse JSON response
    // Add to commands/helptexts
    return 0;
}
```

This is why CLIgen excels at CLI applications - it handles all the dynamic aspects for you.
