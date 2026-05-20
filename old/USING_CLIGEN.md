# Using Real CLIgen - No Reinventing the Wheel

## What Changed

Replaced manual implementation with **CLIgen's built-in functions**:

| Feature | Before (simple_cli.c) | After (simple_cli_cligen.c) |
|---------|---|---|
| Input reading | `fgets()` | `cliread()` ← readline built-in |
| Tab completion | Manual callbacks | Automatic via CLIgen |
| Command parsing | Manual `sscanf()` | `cliread_parse()` ← automatic |
| Type validation | Manual `is_valid_ipv4()` | CLIgen type system |
| Command dispatch | Manual if/else | `cliread_eval()` ← automatic |
| Line editing | None | Full support via readline |
| History | None | Automatic |
| Code lines | ~310 | ~250 (and more powerful!) |

## Files

### Source Code
- **network.cli** - CLI specification (defines syntax, not code)
- **simple_cli_cligen.c** - Main program using CLIgen functions

### What They Do

**network.cli:**
```cli
show interfaces("Show all interfaces"), show_interfaces();
config interface <ifname:string interface()> {
    ip <ipaddr:ipv4addr>("IP address"), set_interface_ip();
}
```

- Defines the CLI syntax
- No code generation, just interpretation
- CLIgen parses this at runtime

**simple_cli_cligen.c:**
```c
int expand_interface(cligen_handle h, ...)   // Dynamic data
int show_interfaces(cligen_handle h, ...)    // Command handler
int set_interface_ip(cligen_handle h, ...)   // Command handler
```

- Expand callbacks (provide dynamic data)
- Command callbacks (handle execution)
- CLIgen calls these automatically

## How It Works

### User Types Command

```
network> config interface <TAB>
```

### CLIgen's Magic

1. **cliread()** detects TAB
2. Sees: `config interface <ifname:string interface()>`
3. Calls: `expand_interface()` callback
4. Gets: ["lo", "ens192", "docker0"]
5. Shows them: `lo  ens192  docker0`
6. User types: `ens192 192.168.1.100`
7. **cliread_parse()** validates:
   - interface exists? ✓ (from expand)
   - IP is ipv4addr? ✓ (type validation)
8. **cliread_eval()** calls: `set_interface_ip()`
9. Command executes

**No manual parsing. No manual validation. All automatic.**

## Building

### Prerequisites

First, build CLIgen library:

```bash
cd /home/sartura/aprcela/cligen
./configure
make
sudo make install
```

(Or just `make` without install if you don't have sudo)

### Compilation

```bash
cd /home/sartura/aprcela/cligen_example

# If you installed CLIgen system-wide:
gcc -Wall -g -std=c99 -D_DEFAULT_SOURCE \
    -o simple_cli_cligen simple_cli_cligen.c \
    -lcligen

# If you built CLIgen locally:
gcc -Wall -g -std=c99 -D_DEFAULT_SOURCE \
    -o simple_cli_cligen simple_cli_cligen.c \
    -I/home/sartura/aprcela/cligen \
    -L/home/sartura/aprcela/cligen/.libs \
    -lcligen
```

## Running

```bash
cd /home/sartura/aprcela/cligen_example
./simple_cli_cligen
```

## Features You Get Automatically

### Tab Completion
```
network> config interface <TAB>
lo  ens192  docker0

network> config interface en<TAB>
ens192
```

### Type Validation
```
network> config interface ens192 ip 999.999.999.999
Error: Invalid ipv4addr format

network> config interface ens192 ip 192.168.1.100
✓ IP Configuration...
```

### Command History
```
<UP arrow>     - Previous command
<DOWN arrow>   - Next command
```

### Line Editing
```
<LEFT>/<RIGHT> - Move cursor
Ctrl+A         - Start of line
Ctrl+E         - End of line
Ctrl+U         - Clear line
Ctrl+K         - Delete to end
```

## What CLIgen Is Doing For You

### Old Way (simple_cli.c)
```c
while (1) {
    printf("cli> ");
    fgets(input, sizeof(input), stdin);
    
    /* Manual parsing */
    sscanf(input, "config interface %s ip %s", ifname, ip);
    
    /* Manual validation */
    if (!interface_exists(ifname)) {
        printf("Invalid interface\n");
        continue;
    }
    if (!is_valid_ipv4(ip)) {
        printf("Invalid IP\n");
        continue;
    }
    
    /* Manual dispatch */
    if (strcmp(cmd, "show interfaces") == 0) {
        show_interfaces();
    } else if (strcmp(cmd, "config") == 0) {
        // ... more code ...
    }
}
```

**Lines of code: ~150 just for the loop!**

### New Way (simple_cli_cligen.c)
```c
while (!cligen_exiting(h)) {
    cliread(h, &line);                    // Readline + tab completion
    cliread_parse(h, line, pt, ...);      // Automatic parsing + validation
    cliread_eval(h, &line, ...);          // Automatic dispatch
}
```

**Lines of code: ~20!**

CLIgen handles:
- ✓ Readline integration
- ✓ Tab completion
- ✓ History
- ✓ Line editing
- ✓ Parsing
- ✓ Type validation
- ✓ Command dispatch

## Example Usage

### Show all interfaces
```
network> show interfaces
╔════════════════════╦════════════╦═══════════════════════╗
║ Interface          ║ Status     ║ IP Address            ║
╠════════════════════╬════════════╬═══════════════════════╣
║ lo                 ║ UP         ║ 127.0.0.1             ║
║ ens192             ║ UP         ║ 172.16.222.109        ║
║ docker0            ║ UP         ║ 172.17.0.1            ║
╚════════════════════╩════════════╩═══════════════════════╝
```

### Show specific interface
```
network> show interface ens192
╔════════════════════════════════════╗
║ Interface: ens192                  ║
║ Status: UP                         ║
║ IP Address: 172.16.222.109         ║
╚════════════════════════════════════╝
```

### Configure IP (with tab completion!)
```
network> config interface <TAB>
docker0  ens192  lo

network> config interface ens192 ip 192.168.1.100
✓ IP Configuration
  Interface: ens192
  IP Address: 192.168.1.100
```

### Remove IP
```
network> config interface ens192 no ip
✓ IP Removed
  Interface: ens192
```

## Code Comparison

### Expand Callback

**What you provide:**
```c
int expand_interface(cligen_handle h, const char *fn_str,
                     cvec *cvv, cvec *argv,
                     cvec *commands, cvec *helptexts)
{
    g_interface_count = get_interfaces(g_interfaces, 100);
    
    for (int i = 0; i < g_interface_count; i++) {
        cvec_add_string(commands, NULL, g_interfaces[i].name);
        cvec_add_string(helptexts, NULL, help_text);
    }
    
    return 0;
}
```

**What CLIgen does with it:**
- Calls it automatically on TAB
- Shows the results
- Allows selection/filtering
- Validates the selection
- Passes selected value to command handler

### Command Callback

**What you provide:**
```c
int set_interface_ip(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv = cvec_find(cvv, "ifname");
    char *ifname = cv_string_get(cv);
    
    cv = cvec_find(cvv, "ipaddr");
    char *ipaddr = cv_string_get(cv);
    
    printf("✓ IP Configuration\n");
    printf("  Interface: %s\n", ifname);
    printf("  IP Address: %s\n", ipaddr);
    
    return 0;
}
```

**What CLIgen already handled:**
- ✓ Parsed the input
- ✓ Validated the interface exists (via expand_interface)
- ✓ Validated the IP is ipv4addr (type system)
- ✓ Called your function
- ✓ Passed you the parsed values

## Key CLIgen Functions Used

### `cliread(cligen_handle h, char **stringp)`
Reads input with readline integration, tab completion, history, etc.

**Returns:** -1 on error, 0 on EOF, >0 on success

### `cliread_parse(cligen_handle h, char *string, parse_tree *pt, ...)`
Parses input string against parse tree, validates, and populates variables.

**Returns:** 0 on success, -1 on error

### `cliread_eval(cligen_handle h, char **line, ...)`
Evaluates the matched command, calling appropriate callbacks.

**Returns:** 0 on success, -1 on error

## Why This Matters

This shows the **real power of CLIgen**:

You don't write a CLI framework. You write:
1. A syntax specification (network.cli)
2. Expand callbacks (what values are available)
3. Command callbacks (what to do)

CLIgen handles everything else:
- Parsing
- Validation
- Completion
- History
- Editing
- Dispatch

## Next Steps

1. **Build CLIgen library**
   ```bash
   cd /home/sartura/aprcela/cligen
   ./configure && make
   ```

2. **Compile the example**
   ```bash
   cd /home/sartura/aprcela/cligen_example
   gcc ... -lcligen simple_cli_cligen.c
   ```

3. **Run it**
   ```bash
   ./simple_cli_cligen
   network> show interfaces
   ```

4. **Try tab completion**
   ```bash
   network> config interface <TAB>
   ```

## Summary

- ✅ Uses `cliread()` for readline integration
- ✅ Uses `cliread_parse()` for automatic parsing
- ✅ Uses `cliread_eval()` for automatic dispatch
- ✅ Uses CLIgen's type system for validation
- ✅ Expand callbacks provide dynamic data
- ✅ Command callbacks do the actual work
- ✅ No manual parsing code
- ✅ No manual validation code
- ✅ No manual dispatch code
- ✅ Tab completion works automatically
- ✅ History works automatically
- ✅ Line editing works automatically

**This is what you get by using CLIgen properly!**
