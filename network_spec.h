#ifndef NETWORK_SPEC_H
#define NETWORK_SPEC_H

/*
 * One shared operational mode for everyone, plus a config mode that only
 * privileged users can reach. Implemented with CLIgen named trees.
 *
 *   guest mode  -> show + help + quit             (prompt: network> )
 *   staff mode  -> show + help + quit + 'config'  (prompt: network> )
 *   config mode -> interface configuration        (prompt: network(config)> )
 *
 * Operator and admin land in 'staff' and can type 'config' to enter config
 * mode. Guests land in 'guest', which has no 'config' command at all.
 *
 * NOTE: commands are inlined per tree (not pulled in via '@tree' references).
 * A bare top-level '@tree' reference sitting next to a callback command (e.g.
 * 'config') left that command's callback unresolved in this CLIgen build, so
 * entering config silently did nothing. Inlining is the robust pattern (it is
 * also what the tutorial and the original base/config spec used).
 *
 * The first parsed tree ('guest') is the default active tree before login.
 */

static const char *embedded_cli_spec =
"prompt = \"network> \";\n"
"comment = \"#\";\n"
"\n"
"# ===== Operational mode: guest variant (no 'config') =====\n"
"treename = \"guest\";\n"
"show(\"Show network configuration\") {\n"
"    interfaces(\"Show all interfaces\"), show_interfaces();\n"
"    interface <ifname:string interface()>(\"Interface name\"), show_interface();\n"
"    history(\"Show history\"), show_history();\n"
"}\n"
"help(\"Show help for this mode\"), help_cmd();\n"
"quit(\"Quit the CLI\"), quit_cmd();\n"
"\n"
"# ===== Operational mode: operator/admin variant (adds 'config') =====\n"
"treename = \"staff\";\n"
"show(\"Show network configuration\") {\n"
"    interfaces(\"Show all interfaces\"), show_interfaces();\n"
"    interface <ifname:string interface()>(\"Interface name\"), show_interface();\n"
"    history(\"Show history\"), show_history();\n"
"}\n"
"config(\"Enter configuration mode\"), mode(\"config\", \"network(config)>\");\n"
"help(\"Show help for this mode\"), help_cmd();\n"
"quit(\"Quit the CLI\"), quit_cmd();\n"
"\n"
"# ===== Configuration mode (reached from 'staff' via 'config') =====\n"
"treename = \"config\";\n"
"hostname(\"Change hostname\") {\n"
"    <hostname:string length[8:32]>(\"hostname\"), set_hostname();\n"
"}\n"
"history(\"Change command history size\") {\n"
"    <size:uint16 range[5:100]>(\"history size\"), set_history_size();\n"
"}\n"
"# Configure interface settings\n"
"interface <ifname:string interface()>(\"Interface name\") {\n"
"    ip <ipaddr:ipv4addr>(\"IPv4 address\"), set_interface_ip();\n"
"    no ip(\"Remove IPv4 address\"), remove_interface_ip();\n"
"    ipv6 <ipaddr:ipv6addr>(\"IPv6 address\"), set_interface_ipv6();\n"
"    no ipv6(\"Remove IPv6 address\"), remove_interface_ipv6();\n"
"    mtu <size:uint16 range[68:65535]>(\"MTU size\"), set_mtu();\n"
"}\n"
"show(\"Show network configuration\") {\n"
"    interfaces(\"Show all interfaces\"), show_interfaces();\n"
"    interface <ifname:string interface()>(\"Interface name\"), show_interface();\n"
"    history(\"Show history\"), show_history();\n"
"}\n"
"exit(\"Leave configuration mode\"), mode(\"staff\", \"network> \");\n"
"end(\"Leave configuration mode\"), mode(\"staff\", \"network> \");\n"
"help(\"Show help for this mode\"), help_cmd();\n"
"quit(\"Quit the CLI\"), quit_cmd();\n";

#endif
