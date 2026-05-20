# Readline Version - Tab Autocomplete Explained

## The Problem With the Original Version

The original `simple_cli.c` uses basic `fgets()`:
```c
while (1) {
    printf("cli> ");
    fgets(input, sizeof(input), stdin);
    // process input
}
```

**Problem:** Tab just inserts a tab character. No autocomplete.

## The Solution: GNU Readline

The readline version uses GNU readline library:
```c
input = readline("cli> ");
add_history(input);
// process input
```

**Benefits:**
- ✓ Tab autocomplete
- ✓ Command history (arrow keys)
- ✓ Line editing (left/right arrows, Ctrl+A, Ctrl+E, etc.)
- ✓ Custom completion callbacks

## How Tab Autocomplete Works

### What CLIgen Does (Under the Hood)

CLIgen uses readline with custom completion callbacks:

1. User types: `ip <TAB>`
2. Readline calls the completion callback
3. Callback queries available interfaces
4. Readline shows the list
5. User selects or types to filter

### What Our Readline Version Does

```c
char **interface_completion(const char *text, int start, int end)
{
    char **matches = NULL;

    /* Check if we're after "ip " */
    if (start > 0 && strncmp(rl_line_buffer, "ip ", 3) == 0) {
        int space_count = 0;
        for (int i = 0; i < end; i++) {
            if (rl_line_buffer[i] == ' ') space_count++;
        }

        /* Complete interface names after "ip " */
        if (space_count == 1) {
            matches = rl_completion_matches(text, interface_generator);
        }
    }

    return matches;
}
```

## Usage - With Tab Autocomplete

```
$ ./simple_cli_readline

cli> ip <TAB>
docker0  ens192  lo

cli> ip e<TAB>
ens192

cli> ip ens192 192.168.1.100 <ENTER>
✓ IP Configuration
  Interface: ens192
  IP Address: 192.168.1.100
```

## Key Features

### Tab Completion for Interfaces
```
cli> ip <TAB>
```
Shows: docker0, ens192, lo

### Partial Matching
```
cli> ip en<TAB>
```
Shows: ens192 (only matches beginning with "en")

### History Navigation
```
<UP>/<DOWN> arrows - Navigate through command history
```

### Line Editing
```
<LEFT>/<RIGHT> - Move cursor
Ctrl+A - Start of line
Ctrl+E - End of line
Ctrl+U - Clear line
```

## Compilation

### On Linux (with readline development libraries):

```bash
# Install readline development headers (if not already installed)
sudo apt-get install libreadline-dev

# Compile
gcc -Wall -g -std=c99 -D_DEFAULT_SOURCE -o simple_cli_readline \
    simple_cli_readline.c -lreadline -lncurses
```

### On macOS:

```bash
# Using Homebrew
brew install readline

# Compile
gcc -Wall -g -std=c99 -o simple_cli_readline \
    simple_cli_readline.c -I/usr/local/opt/readline/include \
    -L/usr/local/opt/readline/lib -lreadline
```

## Files

- **simple_cli_readline.c** - Source code with readline integration
- **This file** - Documentation and explanation

## Comparison: Simple vs Readline

### Simple Version (simple_cli.c)
```
Pros:
  ✓ No external dependencies
  ✓ Compiles everywhere
  ✓ Simple to understand
  
Cons:
  ✗ No tab autocomplete
  ✗ No history
  ✗ Basic line editing
```

### Readline Version (simple_cli_readline.c)
```
Pros:
  ✓ Tab autocomplete for interfaces
  ✓ Command history
  ✓ Full line editing
  ✓ Like professional CLIs
  
Cons:
  ✗ Requires readline library
  ✗ More complex code
```

## Why CLIgen is Better

CLIgen handles this automatically:

**CLIgen spec:**
```cli
interface <ifname:string interface()> {
    ip address <ipaddr:ipv4addr>, set_ip();
}
```

**What you get automatically:**
- ✓ Tab autocomplete (all levels)
- ✓ Type validation
- ✓ Help generation
- ✓ History
- ✓ Hierarchical menus
- ✓ Command parsing

**No manual code needed!**

## Code Structure

### Global State (For Completion)
```c
static interface_info g_interfaces[MAX_INTERFACES];
static int g_interface_count = 0;
```

Readline completion callbacks need access to the interface list, so it's stored globally.

### Interface Generator
```c
char *interface_generator(const char *text, int state)
```

Called by readline to generate completions:
- First call: state=0, initialize
- Subsequent calls: state>0, return next match

### Completion Function
```c
char **interface_completion(const char *text, int start, int end)
```

Main completion callback:
- Checks if we're completing interface names
- Calls `interface_generator` to get matches
- Returns array of completions

### Main Loop
```c
input = readline("cli> ");  /* This is the key difference */
add_history(input);
```

`readline()` handles all the interactive line editing and tab completion.

## Testing Autocomplete

### Test 1: Show interfaces
```bash
cli> ip <ENTER>
[Shows available interfaces]
```

### Test 2: Autocomplete interface name
```bash
cli> ip <TAB>
[Shows: docker0  ens192  lo]

cli> ip ens<TAB>
ens192

cli> ip ens192 192.168.1.100 <ENTER>
[Configures IP]
```

### Test 3: History
```bash
cli> ip ens192 192.168.1.100
[Configure succeeds]

cli> <UP>
[Shows previous command: ip ens192 192.168.1.100]
```

### Test 4: Line editing
```bash
cli> ip ens192 192.168.1.100<CTRL-A>
[Cursor moves to start]

cli> ip ens192 192.168.1.100<CTRL-E>
[Cursor moves to end]
```

## Learning Path

1. **Understand simple_cli.c** - Basic CLI loop
2. **Understand simple_cli_readline.c** - Readline integration
3. **Learn CLIgen** - Which automates this for you

## Installation Instructions for Different OS

### Ubuntu/Debian
```bash
sudo apt-get install libreadline-dev
gcc -Wall -g -std=c99 -D_DEFAULT_SOURCE -o simple_cli_readline \
    simple_cli_readline.c -lreadline -lncurses
```

### Fedora/RHEL
```bash
sudo dnf install readline-devel
gcc -Wall -g -std=c99 -D_DEFAULT_SOURCE -o simple_cli_readline \
    simple_cli_readline.c -lreadline
```

### macOS (with Homebrew)
```bash
brew install readline
gcc -Wall -g -std=c99 -o simple_cli_readline \
    simple_cli_readline.c \
    -I/usr/local/opt/readline/include \
    -L/usr/local/opt/readline/lib \
    -lreadline
```

### macOS (with MacPorts)
```bash
sudo port install readline
gcc -Wall -g -std=c99 -o simple_cli_readline \
    simple_cli_readline.c \
    -I/opt/local/include \
    -L/opt/local/lib \
    -lreadline
```

## Why This Matters for CLIgen Understanding

**This version shows:**

1. **How readline integration works** - The foundation of CLIgen's CLI
2. **Custom completion callbacks** - How CLIgen provides context-aware completions
3. **Dynamic data + Interactive UI** - Combining runtime data with user interaction
4. **Why CLIgen is valuable** - It automates all this for you

## Summary

- **simple_cli.c** - Works everywhere, no dependencies, no autocomplete
- **simple_cli_readline.c** - Requires readline, but has tab autocomplete
- **CLIgen** - Handles this + much more automatically

Choose based on your needs:
- Simple example without dependencies? → `simple_cli.c`
- Want to see tab autocomplete? → `simple_cli_readline.c`
- Building production CLI? → Use CLIgen!
