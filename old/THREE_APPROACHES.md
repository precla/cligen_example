# Three Approaches - Side by Side

This document shows the evolution from manual to CLIgen-based implementation.

## The Three Versions

### Version 1: Manual Implementation
**File:** `simple_cli.c`

```c
while (1) {
    printf("cli> ");
    fgets(input, sizeof(input), stdin);
    
    // Manual parsing
    sscanf(input, "ip %255s %255s", ifname, ip_addr);
    
    // Manual validation
    if (!interface_exists(ifname, interfaces, count))
        error("Interface not found");
    if (!is_valid_ipv4(ip_addr))
        error("Invalid IP");
    
    // Manual dispatch
    if (strcmp(input, "ip") == 0)
        handle_ip_command(input);
}
```

**Characteristics:**
- No dependencies (just fgets)
- Educational - shows what code you'd write without a framework
- ~310 lines
- No tab completion
- No history
- Manual everything

**Use case:** Learning the concepts

---

### Version 2: Readline Integration
**File:** `simple_cli_readline.c`

```c
// Register readline completion callback
rl_attempted_completion_function = interface_completion;

while (1) {
    input = readline("cli> ");  // Readline handles tab completion!
    add_history(input);
    
    // Still manual parsing
    sscanf(input, "ip %255s %255s", ifname, ip_addr);
    
    // Still manual validation
    if (!is_valid_ipv4(ip_addr))
        error("Invalid IP");
}
```

**Characteristics:**
- Uses readline library (requires libreadline-dev)
- Shows how readline integration works
- ~250 lines
- Tab completion works!
- History works
- Still manual parsing/validation

**Use case:** Understanding readline integration

---

### Version 3: Full CLIgen Integration ⭐
**Files:** `network.cli` + `simple_cli_cligen.c`

**network.cli** (10 lines):
```cli
config interface <ifname:string interface()> {
    ip <ipaddr:ipv4addr>("IP address"), set_interface_ip();
}
```

**simple_cli_cligen.c** (250 lines):
```c
// Provide dynamic data
int expand_interface(cligen_handle h, ...) {
    add_interface_names_to_list();
}

// Provide command logic
int set_interface_ip(cligen_handle h, cvec *cvv, cvec *argv) {
    char *ifname = cv_string_get(cvec_find(cvv, "ifname"));
    char *ipaddr = cv_string_get(cvec_find(cvv, "ipaddr"));
    printf("Setting %s to %s\n", ifname, ipaddr);
}

while (!cligen_exiting(h)) {
    cliread(h, &input);              // Automatic readline
    cliread_parse(h, input, ...);    // Automatic parsing
    cliread_eval(h, &input, ...);    // Automatic dispatch
}
```

**Characteristics:**
- Uses CLIgen library (requires building CLIgen)
- Automatic everything
- ~250 lines (but more powerful!)
- Tab completion ✓
- History ✓
- Automatic parsing ✓
- Automatic validation ✓
- Automatic dispatch ✓

**Use case:** Production CLI applications

---

## Feature Comparison Table

| Feature | simple_cli.c | simple_cli_readline.c | simple_cli_cligen.c |
|---------|---|---|---|
| **Input Reading** | fgets() | readline() | cliread() |
| **Tab Completion** | ✗ | ✓ | ✓ Automatic |
| **History** | ✗ | ✓ | ✓ Automatic |
| **Line Editing** | ✗ | ✓ | ✓ Automatic |
| **Command Parsing** | Manual | Manual | Automatic |
| **Type Validation** | Manual | Manual | Automatic |
| **Command Dispatch** | Manual | Manual | Automatic |
| **Dependencies** | None | libreadline | libcligen |
| **Code Lines** | ~310 | ~250 | ~250 |
| **Lines for Parsing** | ~40 | ~40 | 0 (built-in) |
| **Lines for Validation** | ~30 | ~30 | 0 (built-in) |
| **Lines for Dispatch** | ~50 | ~50 | 0 (built-in) |
| **Maintenance** | High | Medium | Low |
| **Extensibility** | Low | Medium | High |

---

## Code Complexity Comparison

### Manual Parsing (simple_cli.c)
```c
int args_parsed = sscanf(input, "ip %255s %255s", ifname, ip_addr);
if (args_parsed < 2) {
    // Show usage...
}
```

**Problem:** Fragile. One format string. Doesn't scale.

### Readline Callbacks (simple_cli_readline.c)
```c
char *interface_generator(const char *text, int state) {
    static int list_index, len;
    if (!state) { list_index = 0; len = strlen(text); }
    while (list_index < count) {
        // ... matching logic ...
    }
}

char **interface_completion(const char *text, int start, int end) {
    if (start > 0 && strncmp(rl_line_buffer, "ip ", 3) == 0) {
        matches = rl_completion_matches(text, interface_generator);
    }
    return matches;
}
```

**Problem:** Complex. Lots of state management. Hard to debug.

### CLIgen (simple_cli_cligen.c)
```cli
config interface <ifname:string interface()> {
    ip <ipaddr:ipv4addr>("IP address"), set_interface_ip();
}
```

**Benefit:** Simple. Clear. Automatic.

---

## What Each Version Teaches You

### Version 1: simple_cli.c
✅ **Teaches:**
- CLI architecture
- Dynamic data loading
- Command processing pattern
- Why validation is important

❌ **Doesn't show:**
- Interactive line editing
- Tab completion
- How to handle complex syntax

---

### Version 2: simple_cli_readline.c
✅ **Teaches:**
- GNU readline library
- Completion callback registration
- State management in callbacks
- What CLIgen does internally

❌ **Doesn't show:**
- Automatic parsing
- Type system
- Parse trees

---

### Version 3: simple_cli_cligen.c
✅ **Teaches:**
- What a real CLI framework does
- How to use CLIgen functions
- How to structure CLI specs
- Automatic parsing and validation

❌ **Doesn't show:**
- Readline internals (abstracted)
- Callback state management (handled by framework)

---

## Implementation Effort

### Adding a New Command

**simple_cli.c:**
1. Add manual parsing code
2. Add validation code
3. Add dispatch logic
4. Test with various inputs
5. Handle edge cases

~50 lines of code

**simple_cli_cligen.c:**
1. Add to network.cli: `new-cmd <param:type>, handler();`
2. Write handler function in C
3. (Done!)

~5 lines of spec + handler function

---

## Real-World Scenario

### Requirement: Add support for DNS servers

**simple_cli.c approach:**
```c
// 1. Update parsing
sscanf(input, "dns %s", server);

// 2. Add validation
if (!is_valid_ipv4(server))
    error("Invalid IP");

// 3. Add dispatch
if (strcmp(cmd, "dns") == 0)
    set_dns(server);

// 4. Write handler
int set_dns(char *server) { ... }

// Total: ~30 lines
```

**simple_cli_cligen.c approach:**
```cli
// 1. Update network.cli
dns <server:ipv4addr>("DNS server"), set_dns();

// 2. Write handler
int set_dns(cligen_handle h, cvec *cvv, cvec *argv) {
    char *server = cv_string_get(cvec_find(cvv, "server"));
    // ... do something ...
}

// Total: ~10 lines
// Validation? Parsing? Completion? All automatic!
```

---

## The Evolution

```
simple_cli.c
    ↓
"Hey, we need tab completion and history"
    ↓
simple_cli_readline.c
    ↓
"This readline code is getting complex..."
    ↓
"Wait, CLIgen has cliread() that does this!"
    ↓
simple_cli_cligen.c
    ↓
"Much cleaner! No manual parsing/validation!"
    ↓
"This is what a framework is supposed to do!"
```

---

## Why CLIgen Wins

### Separation of Concerns

**Specification (network.cli):**
```cli
interface <ifname:string interface()>
```
Says: "What's the syntax?"

**Implementation (callbacks):**
```c
int expand_interface(...) { /* what interfaces are available */ }
int show_interface(...) { /* what to do */ }
```
Says: "How does it work?"

**CLIgen handles:**
- Parsing the spec
- Matching input against spec
- Calling callbacks
- Type validation
- Completion suggestions

### You only write:
1. ✓ Syntax (what commands exist)
2. ✓ Data (what values are available)
3. ✓ Logic (what happens)

### CLIgen handles:
- ✓ Parsing
- ✓ Validation
- ✓ Completion
- ✓ History
- ✓ Editing
- ✓ Dispatch

---

## When to Use Each

### Use simple_cli.c When:
- Learning CLI concepts
- Need zero dependencies
- Building minimal example
- Teaching/presentation

### Use simple_cli_readline.c When:
- Want to understand readline
- Need tab completion but not full framework
- Studying CLIgen internals
- Educational purposes

### Use simple_cli_cligen.c When:
- Building real applications
- Need professional features
- Managing complexity
- Want maintainability
- **Serious development**

---

## Summary

| Approach | Purpose | Dependencies | Complexity | Power |
|----------|---------|---|---|---|
| simple_cli.c | Learning | None | Low | Basic |
| simple_cli_readline.c | Understanding readline | libreadline | Medium | Intermediate |
| simple_cli_cligen.c | Production | libcligen | Low | Professional |

**The progression shows why CLIgen exists:**
- Manual code is tedious
- Readline adds features but complexity
- CLIgen automates everything

**This is why you use a framework!**
