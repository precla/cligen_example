# Quick Start - Updated CLI with IP Configuration

## What's New

The CLI now supports **IP address configuration** with dynamic interface discovery!

```
Old: cli> ip              → Shows table of interfaces
New: cli> ip              → Shows interfaces with usage example
New: cli> ip ens0 192.168.1.100  → Configures IP address
```

## Compile and Run (30 seconds)

### Step 1: Compile
```bash
cd /home/sartura/aprcela/cligen_example
gcc -Wall -g -std=c99 -D_DEFAULT_SOURCE -o simple_cli simple_cli.c
```

### Step 2: Run
```bash
./simple_cli
```

### Step 3: Try Commands
```
cli> help              # Show available commands
cli> ip                # Show available interfaces
cli> ip ens192 192.168.1.100  # Configure IP (simulated)
cli> exit              # Exit
```

## Examples

### Show Available Interfaces
```
cli> ip

[Loading interfaces from system...]
[Available interfaces to configure:]

  lo                 (UP   )  Currently: 127.0.0.1
  ens192             (UP   )  Currently: 172.16.222.109
  docker0            (UP   )  Currently: 172.17.0.1

Usage: ip <interface> <ip_address>
Example: ip eth0 192.168.1.100
```

### Configure IP Address
```
cli> ip ens192 192.168.1.100

[Loading interfaces from system...]

✓ IP Configuration
  Interface: ens192
  IP Address: 192.168.1.100
  Status: Would be applied

  [Note: This is a simulation. In a real system,
   this would run: ip addr add 192.168.1.100/24 dev ens192]
```

### Error - Invalid Interface
```
cli> ip badintf 10.0.0.1

Error: Interface 'badintf' not found

Available interfaces:
  - lo
  - ens192
  - docker0
```

### Error - Invalid IP Format
```
cli> ip ens192 invalid.ip

Error: Invalid IPv4 address format: 'invalid.ip'
Expected format: XXX.XXX.XXX.XXX (e.g., 192.168.1.100)
```

## Command Format

```
ip <interface> <ip_address>
```

- `<interface>` - One of the discovered interfaces (lo, ens192, docker0, etc.)
- `<ip_address>` - Valid IPv4 address (XXX.XXX.XXX.XXX format)

## Valid Examples

```bash
ip lo 127.0.0.2
ip ens192 192.168.1.100
ip docker0 172.17.0.50
ip eth0 10.0.0.1
```

## Test It

### Automated Test
```bash
./TEST_DEMO.sh
```

This runs 6 comprehensive tests showing all features.

### Manual Test
```bash
echo -e "ip\nip ens192 192.168.1.100\nexit" | ./simple_cli
```

## Key Features

### ✓ Dynamic Interface Discovery
- Interfaces loaded from OS at runtime
- Shows current status and IP
- Works on any system

### ✓ Argument Parsing
- Parses `ip <interface> <ip>`
- Beyond single-word commands
- Handles multiple arguments

### ✓ Input Validation
- Validates interface exists
- Validates IPv4 format
- Helpful error messages

### ✓ Error Handling
- Shows available options on error
- Suggests correct format
- Guides user to success

## All Commands

| Command | Function |
|---------|----------|
| `ip` | Show available interfaces |
| `ip <interface> <ip>` | Configure IP (simulated) |
| `help` | Show available commands |
| `clear` | Clear screen |
| `exit` | Exit CLI |

## What This Demonstrates

This CLI now shows:

1. **Dynamic Data Loading** - Interfaces from OS, not hardcoded
2. **Argument Parsing** - Handles multiple arguments
3. **Input Validation** - Checks both interface and IP
4. **Error Handling** - Helpful messages
5. **Command Processing** - Complex commands with validation

This is exactly what CLIgen automates!

## Why This Matters

**Without CLIgen (manual approach):**
```c
// Parse arguments manually
sscanf(input, "ip %s %s", interface, ip);

// Validate interface manually
for (int i = 0; i < count; i++) {
    if (strcmp(interfaces[i].name, interface) != 0)
        continue;
    // found it
}

// Validate IP manually
if (inet_aton(ip, &addr) == 0)
    error("Invalid IP");
```

**With CLIgen (automatic):**
```cli
interface <ifname:string interface()> {
    ip address <ipaddr:ipv4addr>, set_ip();
}
```

CLIgen handles all the parsing, validation, and error messages!

## Next Steps

1. ✅ Run `./simple_cli`
2. ✅ Try `ip` to see interfaces
3. ✅ Try `ip ens192 192.168.1.100`
4. 📖 Read `UPDATED_FEATURES.md` for more details
5. 📖 Read `UPDATE_SUMMARY.txt` for complete reference
6. 🧪 Run `./TEST_DEMO.sh` to see all features

## File Overview

- **simple_cli.c** - Main program (now ~310 lines)
- **simple_cli** - Compiled executable
- **UPDATED_FEATURES.md** - Feature documentation
- **UPDATE_SUMMARY.txt** - Complete reference
- **TEST_DEMO.sh** - Automated test script

## Summary

You now have a CLI that:
- ✓ Runs in endless loop
- ✓ Accepts complex commands with arguments
- ✓ Loads data dynamically from OS
- ✓ Validates user input
- ✓ Shows helpful error messages
- ✓ Simulates real configuration

**Start now:** `./simple_cli`
