# Simple Dynamic Interface CLI

A minimal working example demonstrating **dynamic interface expansion** - the core feature of CLIgen that makes it powerful for hierarchical CLIs.

## What It Does

This is a simple interactive CLI that:
1. **Runs in an endless loop** waiting for user input
2. When user enters `ip`, **dynamically queries the OS** for network interfaces
3. **Displays them in a formatted table** with status and IP address
4. **Loops back** waiting for more commands

## Files

- **`simple_cli.c`** - Main C source code
- **`Makefile`** - Build configuration

## Compile

```bash
cd /home/sartura/aprcela/cligen_example
make
```

Or manually:
```bash
gcc -Wall -g -std=c99 -D_DEFAULT_SOURCE -o simple_cli simple_cli.c
```

## Run

```bash
./simple_cli
```

## Usage Example

```
╔════════════════════════════════════════════════════════════╗
║          Simple Dynamic Interface CLI                      ║
║                                                            ║
║  Type 'help' for available commands                        ║
║  Type 'ip' to see network interfaces (loaded dynamically)  ║
╚════════════════════════════════════════════════════════════╝

cli> ip
[Loading interfaces from system...]
[Found 3 interface(s)]

╔════════════════════╦════════════╦═══════════════════════╗
║ Interface          ║ Status     ║ IP Address            ║
╠════════════════════╬════════════╬═══════════════════════╣
║ lo                 ║ UP         ║ 127.0.0.1             ║
║ ens192             ║ UP         ║ 172.16.222.109        ║
║ docker0            ║ UP         ║ 172.17.0.1            ║
╚════════════════════╩════════════╩═══════════════════════╝

cli> help

Available commands:
  ip              - Show network interfaces (dynamically loaded)
  help            - Show this help message
  clear           - Clear screen
  exit/quit       - Exit the CLI

cli> exit
Goodbye!
```

## Available Commands

| Command | Description |
|---------|-------------|
| `ip` | Show all network interfaces with status and IP addresses |
| `help` | Show available commands |
| `clear` | Clear the screen |
| `exit` / `quit` | Exit the CLI |

## How It Demonstrates Dynamic Expansion

### The Key Function: `get_interfaces()`

This function is called **every time** the user enters `ip`:

```c
int get_interfaces(interface_info *interfaces, int max_count)
{
    struct ifaddrs *ifaddr, *ifa;
    
    /* Query the OS for actual interfaces */
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    /* Populate the list dynamically */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        // Add interface to list
    }

    freeifaddrs(ifaddr);
    return count;
}
```

### Why This Matters

**Without dynamic expansion (hardcoded):**
```c
char *interfaces[] = {"eth0", "eth1", "docker0"};
// If you add wlan0 to the system, you must recompile!
```

**With dynamic expansion (this program):**
```c
get_interfaces();  // Calls getifaddrs() at runtime
// Add wlan0 to system? Automatically shows up! No recompilation needed!
```

## Architecture

### Main Loop

```
Start CLI
    ↓
Display welcome banner
    ↓
Loop:
    ├── Print prompt: "cli> "
    ├── Read user input
    ├── Process command:
    │   ├── "ip"      → Call get_interfaces() → Display in table
    │   ├── "help"    → Show available commands
    │   ├── "clear"   → Clear screen
    │   ├── "exit"    → Break loop and exit
    │   └── unknown   → Show error message
    └── Repeat
Exit
```

### Data Flow for `ip` Command

```
User enters: ip
    ↓
CLI calls: get_interfaces()
    ↓
Calls: getifaddrs() (OS system call)
    ↓
OS returns: All network interfaces on the system
    ↓
Function populates: interface_info[] array
    ↓
CLI calls: display_interfaces()
    ↓
Formats and prints: Nice ASCII table
    ↓
Returns to main loop
```

## Testing

### Test with automated input:

```bash
echo -e "ip\nhelp\nexit" | ./simple_cli
```

### Test interactively:

```bash
./simple_cli
# Then type commands and observe output
```

## Key Differences from Manual CLI Parsing

| Aspect | Hardcoded CLI | Dynamic CLI (This Program) |
|--------|---|---|
| Interface list | `char *ifaces[] = {"eth0", "eth1"}` | `getifaddrs()` queries OS |
| Add new interface | Must recompile code | Works automatically |
| Portability | Only works on one machine | Works on any system |
| Data freshness | Static at compile time | Fresh on each query |
| Maintenance | High - update code per change | Low - automatic discovery |

## Extending This Program

You can extend `get_interfaces()` to pull data from anywhere:

### Example 1: Filter interfaces

```c
/* Only show UP interfaces */
if (!(ifa->ifa_flags & IFF_UP))
    continue;
```

### Example 2: Add more information

```c
/* Store MTU size */
interfaces[i].mtu = ifa->ifa_data ? /* extract MTU */ : 0;
```

### Example 3: Add new commands

```c
else if (strcmp(input, "routing") == 0) {
    show_routing_table();
}

else if (strcmp(input, "stats") == 0) {
    show_interface_stats();
}
```

### Example 4: Integrate with CLIgen

This simple example demonstrates the **concept** that CLIgen uses. With full CLIgen, you would:

1. Define CLI syntax in `.cli` file
2. Register expand callbacks like `get_interfaces()`
3. CLIgen handles parsing, validation, completion, and help
4. Your code just provides the data

## Why CLIgen is More Powerful

This program handles one simple command (`ip`). CLIgen would handle:

```cli
interface <ifname:string interface()> {
    ip address <ipaddr:ipv4addr>;
    shutdown;
    mtu <size:int32>;
}

routing {
    route <dest:ipv4addr> <gw:ipv4addr>;
}
```

With:
- ✅ Hierarchical command structure (menus)
- ✅ Automatic tab-completion at every level
- ✅ Data type validation (ipv4addr, int32, etc.)
- ✅ Auto-generated help
- ✅ Dynamic expansion at each parameter
- ✅ One expand callback per parameter
- ✅ No manual parsing code needed

This program shows the **core dynamic data loading concept** that makes CLIgen so useful.

## Compilation Notes

- Uses `gcc` C compiler
- Standard C99 features
- Requires `-D_DEFAULT_SOURCE` flag for `getifaddrs()` on Linux
- Only uses standard library (libc)
- No external dependencies

## Next Steps

1. ✅ Understand dynamic expansion with this simple example
2. ⬜ Try CLIgen's full framework with hierarchical commands
3. ⬜ Add your own custom commands and expand functions
4. ⬜ Integrate with databases, APIs, or other data sources
