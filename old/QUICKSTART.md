# Quick Start Guide

## Build and Run in 3 Steps

### 1. Compile

```bash
cd /home/sartura/aprcela/cligen_example
make clean
make
```

**Expected output:**
```
gcc -Wall -g -std=c99 -D_DEFAULT_SOURCE -o simple_cli simple_cli.c
```

### 2. Run

```bash
./simple_cli
```

**Expected output:**
```
╔════════════════════════════════════════════════════════════╗
║          Simple Dynamic Interface CLI                      ║
║                                                            ║
║  Type 'help' for available commands                        ║
║  Type 'ip' to see network interfaces (loaded dynamically)  ║
╚════════════════════════════════════════════════════════════╝

cli> _
```

### 3. Try Commands

Type these at the `cli>` prompt:

#### Show interfaces (dynamically loaded from OS)
```
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

cli> _
```

#### Show available commands
```
cli> help

Available commands:
  ip              - Show network interfaces (dynamically loaded)
  help            - Show this help message
  clear           - Clear screen
  exit/quit       - Exit the CLI

cli> _
```

#### Clear the screen
```
cli> clear
```

#### Exit the CLI
```
cli> exit
Goodbye!
```

## Key Concept Demonstrated

When you type `ip`, the program:

1. **Queries the Operating System** for actual network interfaces
2. **Displays them dynamically** - not from a hardcoded list
3. **Shows real data**: interface names, UP/DOWN status, and IP addresses

**Why this matters:**
- ✅ Works on any system automatically
- ✅ Shows only interfaces that actually exist
- ✅ Data is always up-to-date
- ✅ No recompilation needed if interfaces change

## Automated Testing

Run the CLI with pre-scripted commands:

```bash
echo -e "ip\nhelp\nexit" | ./simple_cli
```

## Code Structure

### Main Function
- **Initialization**: Prints welcome banner
- **Main loop**: Reads user input, processes commands
- **Exit**: Prints goodbye message

### Key Functions
- `get_interfaces()` - Queries OS for interfaces (the "dynamic expansion")
- `display_interfaces()` - Formats and displays the table
- `show_help()` - Shows available commands

### The Magic: Dynamic Expansion

```c
/* This function is called when user enters 'ip' */
int get_interfaces(interface_info *interfaces, int max_count)
{
    /* getifaddrs() queries the ACTUAL system for interfaces */
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    /* Populate list with real data from OS */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        // Add interface to list
    }

    return count;
}
```

When you run it on your system, it queries your actual OS and shows your actual interfaces.

## Files Overview

| File | Purpose |
|------|---------|
| `simple_cli.c` | Main program source code |
| `Makefile` | Build script |
| `README_SIMPLE_CLI.md` | Detailed explanation |
| `QUICKSTART.md` | This file - quick start guide |

## What's Next?

1. **Read the code**: Open `simple_cli.c` and understand how it works
2. **Understand `getifaddrs()`**: This is what makes it "dynamic"
3. **Extend it**: Add more commands that query system data
4. **Learn CLIgen**: See how CLIgen automates this pattern

## Troubleshooting

### Error: "gcc: command not found"
Install gcc:
```bash
sudo apt-get install build-essential
```

### Error: "make: command not found"
Install make:
```bash
sudo apt-get install make
```

### Program won't compile
Make sure you're in the right directory:
```bash
cd /home/sartura/aprcela/cligen_example
```

### Compilation error about IFF_UP
Add the flag during compilation:
```bash
gcc -Wall -g -std=c99 -D_DEFAULT_SOURCE -o simple_cli simple_cli.c
```

The Makefile already includes this flag, so `make` should work fine.

## Learning Path

```
1. Run ./simple_cli          ← Try it first
2. Type 'ip' and observe     ← See dynamic expansion in action
3. Read simple_cli.c         ← Understand the implementation
4. Modify get_interfaces()   ← Learn how to query OS
5. Add new commands          ← Extend functionality
6. Study CLIgen tutorial     ← Learn full framework
```

## One-Liner Tests

### Test all commands
```bash
echo -e "ip\nip\nhelp\nclear\nexit" | ./simple_cli
```

### Test error handling
```bash
echo -e "invalid\nip\nexit" | ./simple_cli
```

### Test just 'ip' command
```bash
echo -e "ip\nexit" | ./simple_cli
```

---

**Start here:** `./simple_cli` and type `ip` to see dynamic interface loading!
