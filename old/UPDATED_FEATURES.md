# Updated Features - Dynamic IP Configuration

## What Changed

The CLI has been updated to support **dynamic IP address configuration** with interface suggestions.

## New Command Format

### Before
```
cli> ip
[Shows table of all interfaces]
```

### After
```
cli> ip
[Shows available interfaces to configure, with current IPs]
Usage: ip <interface> <ip_address>
Example: ip eth0 192.168.1.100

cli> ip ens192 192.168.1.100
[Simulates setting IP address]
```

## Key Features

### 1. Show Available Interfaces
When user types just `ip`, it shows:
- All available interfaces (dynamically loaded from OS)
- Current status (UP/DOWN)
- Currently assigned IP address
- Usage instructions

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

### 2. Configure IP Address
User types: `ip <interface> <ip_address>`

```
cli> ip ens192 192.168.1.100

✓ IP Configuration
  Interface: ens192
  IP Address: 192.168.1.100
  Status: Would be applied

  [Note: This is a simulation. In a real system,
   this would run: ip addr add 192.168.1.100/24 dev ens192]
```

### 3. Error Handling

#### Invalid Interface
```
cli> ip nonexistent 192.168.1.1

Error: Interface 'nonexistent' not found

Available interfaces:
  - lo
  - ens192
  - docker0
```

#### Invalid IP Address
```
cli> ip ens192 invalid.ip

Error: Invalid IPv4 address format: 'invalid.ip'
Expected format: XXX.XXX.XXX.XXX (e.g., 192.168.1.100)
```

#### Incomplete Command
```
cli> ip ens192

[Available interfaces to configure:]

  lo                 (UP   )  Currently: 127.0.0.1
  ens192             (UP   )  Currently: 172.16.222.109
  docker0            (UP   )  Currently: 172.17.0.1

Usage: ip <interface> <ip_address>
Example: ip eth0 192.168.1.100
```

## Implementation Details

### New Functions

#### `handle_ip_command(const char *input)`
- Parses the full command: `ip <interface> <ip_address>`
- Dynamically loads interfaces from OS
- Validates interface exists
- Validates IP address format
- Shows appropriate messages

#### `is_valid_ipv4(const char *ip)`
- Validates IPv4 address format
- Returns 1 if valid, 0 if invalid
- Uses standard `inet_aton()` function

#### `interface_exists(const char *ifname, ...)`
- Checks if interface exists in the list
- Returns 1 if exists, 0 if not

### Key Improvements

1. **Dynamic Interface Discovery**
   - Interfaces are queried each time from OS
   - Works on any system
   - Always shows current state

2. **Argument Parsing**
   - Parses `ip <interface> <ip_address>`
   - Uses `sscanf()` for simple parsing
   - Handles missing arguments gracefully

3. **Input Validation**
   - Interface name validation (checks against OS)
   - IP address format validation (using `inet_aton()`)
   - Helpful error messages

4. **User Guidance**
   - Shows available options
   - Provides usage examples
   - Clear success/error messages

## How It Demonstrates CLIgen Concepts

### Dynamic Data Loading
Even though this is a simple program, it shows:
- **Dynamic interface discovery** (not hardcoded)
- **Real-time data** (queries OS at runtime)
- **Argument parsing** (handles `ip <interface> <ip>` format)
- **Validation** (checks if interface exists, validates IP)

### What CLIgen Would Add
With CLIgen, you would get automatically:
- Tab-completion for interface names
- IP address type validation (ipv4addr type)
- Hierarchical menus
- Auto-generated help
- Better argument parsing
- Interactive command suggestions

## Command Examples

### Show all available interfaces
```bash
cli> ip
```

### Configure interface with IP
```bash
cli> ip lo 127.0.0.2
cli> ip ens192 192.168.100.50
cli> ip docker0 172.17.0.2
```

### View help
```bash
cli> help
```

### Clear screen
```bash
cli> clear
```

### Exit
```bash
cli> exit
```

## Testing

### Test 1: Show interfaces
```bash
echo "ip" | ./simple_cli
```

### Test 2: Set IP on existing interface
```bash
echo "ip ens192 192.168.1.100" | ./simple_cli
```

### Test 3: Error - invalid interface
```bash
echo "ip nonexistent 192.168.1.1" | ./simple_cli
```

### Test 4: Error - invalid IP
```bash
echo "ip ens192 invalid.ip" | ./simple_cli
```

### Test 5: Complete workflow
```bash
echo -e "help\nip\nip ens192 192.168.1.50\nexit" | ./simple_cli
```

## Code Changes

### Main Changes in `simple_cli.c`

1. **Added** `handle_ip_command()` function
   - Processes the full command with arguments
   - Dynamically loads interfaces
   - Validates and shows results

2. **Added** `is_valid_ipv4()` function
   - Validates IPv4 address format
   - Uses standard library `inet_aton()`

3. **Added** `interface_exists()` function
   - Checks if interface is in the list
   - Returns boolean result

4. **Updated** `show_help()` function
   - Shows new command format
   - Provides examples

5. **Updated** `main()` function
   - Changed command processing to handle `ip <args>`
   - Uses `strncmp()` instead of `strcmp()` for prefix matching
   - Calls `handle_ip_command()` for all `ip` variants

## Why This Matters

This update shows:

✓ **Real command argument parsing**
  - Beyond simple single-word commands
  - Multiple arguments with validation

✓ **Dynamic data integration**
  - Uses actual OS data
  - Real interface discovery
  - Real IP validation

✓ **Error handling**
  - Validates input
  - Provides helpful messages
  - Shows available options

✓ **How CLIgen helps**
  - These features (validation, parsing, suggestions) come automatically
  - Write less code, get more features

## Next Steps

### To Extend This
1. Add IP deletion: `no ip <interface>`
2. Add IP retrieval: `show ip <interface>`
3. Add interface status: `interface <name> up|down`
4. Add DNS configuration: `dns <server>`
5. Integrate with actual `ip` command

### To Use CLIgen
```cli
interface <ifname:string interface()> {
    ip address <ipaddr:ipv4addr>, set_ip();
    no ip address, del_ip();
    mtu <size:int32>, set_mtu();
    shutdown, shutdown_if();
}
```

This would give you all features automatically!

## Summary

The updated CLI now:
- ✓ Dynamically shows available interfaces
- ✓ Parses `ip <interface> <ip_address>` commands
- ✓ Validates interfaces exist
- ✓ Validates IP address format
- ✓ Shows helpful error messages
- ✓ Simulates IP configuration
- ✓ Works on any system

This demonstrates the core concepts that CLIgen automates!
