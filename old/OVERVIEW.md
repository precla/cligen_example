# CLIgen Example - Dynamic Interface Loading Overview

## What You Have

A complete working example in `/home/sartura/aprcela/cligen_example/` that demonstrates **dynamic interface expansion** - the core power of CLIgen.

## The Problem This Solves

**Traditional CLI:**
```bash
show interface eth0     # Hardcoded interface names
show interface eth1     # Must recompile if you add eth2
show interface eth2
```

**Dynamic CLI (this example):**
```bash
cli> ip                 # Automatically discovers interfaces
[Found 3 interface(s)]
- lo       (UP)   127.0.0.1
- ens192   (UP)   172.16.222.109
- docker0  (UP)   172.17.0.1
```

## How to Use

### Quick Test (30 seconds)

```bash
cd /home/sartura/aprcela/cligen_example
make
./simple_cli
```

Type `ip` and press Enter → **See your actual network interfaces loaded dynamically**

### What Happens

```
┌─────────────────────────────────────────────────────────┐
│ User types: ip                                          │
│ Presses: Enter                                          │
└─────────────────────────────────────────────────────────┘
            ↓
┌─────────────────────────────────────────────────────────┐
│ Program calls: getifaddrs()                             │
│ This queries the OPERATING SYSTEM                       │
│ "Tell me what network interfaces exist right now"       │
└─────────────────────────────────────────────────────────┘
            ↓
┌─────────────────────────────────────────────────────────┐
│ OS returns: lo, ens192, docker0                         │
│ (Whatever interfaces your system actually has)          │
└─────────────────────────────────────────────────────────┘
            ↓
┌─────────────────────────────────────────────────────────┐
│ Program formats and displays in a nice table:           │
│ ┌──────────────┬────────┬────────────────┐              │
│ │ Interface    │ Status │ IP Address     │              │
│ ├──────────────┼────────┼────────────────┤              │
│ │ lo           │ UP     │ 127.0.0.1      │              │
│ │ ens192       │ UP     │ 172.16.222.109 │              │
│ │ docker0      │ UP     │ 172.17.0.1     │              │
│ └──────────────┴────────┴────────────────┘              │
└─────────────────────────────────────────────────────────┘
```

## Files Included

### Source Code
- **`simple_cli.c`** (230 lines)
  - Main program
  - Endless loop waiting for user input
  - `get_interfaces()` function that queries OS
  - Command handlers
  - Formatted output

### Build
- **`Makefile`**
  - Compile with: `make`
  - Run with: `make run`
  - Clean with: `make clean`

### Documentation
- **`README_SIMPLE_CLI.md`** (comprehensive guide)
  - Full explanation of how it works
  - Code walkthroughs
  - Architecture diagrams
  - Extension examples

- **`QUICKSTART.md`** (quick start guide)
  - 3-step setup
  - Command examples
  - Troubleshooting

- **`OVERVIEW.md`** (this file)
  - High-level summary

## The Core Concept

### What Makes It "Dynamic"

```c
int get_interfaces(interface_info *interfaces, int max_count)
{
    struct ifaddrs *ifaddr, *ifa;
    
    /* THIS is the magic: Query the OS AT RUNTIME */
    if (getifaddrs(&ifaddr) == -1) {
        return -1;
    }

    /* Populate with whatever interfaces actually exist */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        // Add interface to list
        // No hardcoding, no assumptions
    }

    return count;
}
```

**Why it's dynamic:**
- Called **every time** user enters `ip`
- **Queries the OS** each time
- Returns **actual current interfaces**
- No hardcoded lists
- No recompilation needed

### Comparison

| Aspect | Hardcoded | Dynamic (This Program) |
|--------|-----------|------------------------|
| Interface list | `char *if[] = {"eth0"}` | `getifaddrs()` |
| Updates | Must recompile | Automatic at runtime |
| New interface | Code change + rebuild | Works immediately |
| Portability | Single machine | Works everywhere |
| Data freshness | Compile-time | Real-time |

## Architecture

```
┌─────────────────────────────────────────┐
│          CLI Main Loop                  │
│  while (1) {                            │
│      read user input                    │
│      process command                    │
│  }                                      │
└──────────┬──────────────────────────────┘
           │
           ├──→ "ip"      → get_interfaces()   → display_interfaces()
           ├──→ "help"    → show_help()
           ├──→ "clear"   → system("clear")
           ├──→ "exit"    → break loop
           └──→ unknown   → show error
```

## The Three Key Functions

### 1. `get_interfaces()`
- **What**: Queries OS for network interfaces
- **How**: Uses `getifaddrs()` system call
- **Returns**: Array of interface information
- **Dynamics**: Called at runtime, always current

### 2. `display_interfaces()`
- **What**: Formats interfaces into a nice table
- **How**: Prints ASCII art with borders
- **Returns**: Nothing (displays to stdout)
- **Polish**: Makes output user-friendly

### 3. `main()` - The endless loop
- **What**: Reads user input and processes commands
- **How**: `fgets()` + command matching
- **Returns**: Never (exits on "exit" command)
- **Pattern**: Classic CLI design pattern

## Why This Matters

### For Understanding CLIgen

CLIgen does exactly this, but:
- ✅ Automatically at every parameter
- ✅ For multiple parameters at once
- ✅ With type validation
- ✅ With hierarchical menus
- ✅ With auto-completion
- ✅ With help generation

This simple program shows the **core idea** that makes CLIgen powerful.

### Real-World Use Cases

You can expand from:
- **Network interfaces** (this example) ← shown here
- **Database queries** (select users from users table)
- **File listings** (ls /config/\*)
- **Remote APIs** (call HTTP endpoint)
- **Generated options** (context-aware choices)

## Running It

### Interactive Mode (Recommended)

```bash
./simple_cli
cli> ip
[output shows your actual interfaces]
cli> help
[output shows available commands]
cli> exit
```

### Scripted Mode (Testing)

```bash
echo -e "ip\nexit" | ./simple_cli
```

## What You'll Learn

By studying this example, you'll understand:

1. **Dynamic data loading** - How to query OS/APIs at runtime
2. **CLI patterns** - Endless loop, command parsing, handlers
3. **User interaction** - Prompts, input reading, output formatting
4. **The OS interface** - How `getifaddrs()` works
5. **Why CLIgen is useful** - Automates all of this for you

## Next Steps

1. ✅ Run: `./simple_cli` → type `ip`
2. 📖 Read: `simple_cli.c` → understand the code
3. 🔧 Modify: Add a new command (e.g., `stats`, `dns`)
4. 🎓 Learn: Read the CLIgen tutorial
5. 🚀 Build: Create a full CLIgen application

## File Locations

```
/home/sartura/aprcela/
├── cligen/                  ← Full CLIgen source code
├── cligen_example/          ← This example
│   ├── simple_cli.c         ← Main program
│   ├── Makefile
│   ├── README_SIMPLE_CLI.md
│   ├── QUICKSTART.md
│   └── OVERVIEW.md          ← This file
```

## Quick Reference

| Task | Command |
|------|---------|
| Compile | `cd cligen_example && make` |
| Run | `./simple_cli` |
| Clean | `make clean` |
| Test | `echo -e "ip\nexit" \| ./simple_cli` |
| Read help | `./simple_cli` then type `help` |

## Summary

**This example shows:**
- ✅ How to build an interactive CLI
- ✅ How to load data dynamically from the OS
- ✅ How to parse and process user commands
- ✅ Why hardcoding is bad (and how to avoid it)
- ✅ The foundation concepts that CLIgen automates

**To run it:**
```bash
cd /home/sartura/aprcela/cligen_example
make
./simple_cli
```

**Then type:** `ip` and press Enter

---

**You now have a working example of dynamic interface loading!**
