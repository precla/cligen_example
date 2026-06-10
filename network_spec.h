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
 * Shared operational commands (show/help/quit) are written ONCE in
 * 'base_common' and pulled into each mode with '@base_common'.
 *
 * The first parsed tree ('guest') is the default active tree before login.
 */

static const char *embedded_cli_spec =
	"prompt = \"network> \";\n"
	"comment = \"#\";\n"
	"\n"
	"# ===== Operational mode: guest variant (no 'config') =====\n"
	"treename = \"guest\";\n"
	"@base_common;\n"
	"\n"
	"# ===== Operational mode: operator/admin variant (adds 'config') =====\n"
	"treename = \"staff\";\n"
	"@base_common;\n"
	"config(\"Enter configuration mode\"), mode(\"config\", \"network(config)>\");\n"
	"\n"
	"# ===== Configuration mode (reached from 'staff' via 'config') =====\n"
	"treename = \"config\";\n"
	"history(\"Change command history size\") {\n"
	"    <size:uint16 range[5:100]>(\"history size\"), set_history_size();\n"
	"}\n"
	"# Configure interface settings\n"
	"interface <ifname:string interface()>(\"Interface name\") {\n"
	"    ip <ipaddr:ipv4addr>(\"IP address\"), set_interface_ip();\n"
	"    no ip(\"Remove IPv4 address\"), remove_interface_ip();\n"
	"    ipv6 <ipaddr:ipv6addr>(\"IPv6 address\"), set_interface_ipv6();\n"
	"    no ipv6(\"Remove IPv6 address\"), remove_interface_ipv6();\n"
	"    mtu <size:uint16 range[68:65535]>(\"MTU size\"), set_mtu();\n"
	"}\n"
	"@base_common;\n"
	"exit(\"Leave configuration mode\"), mode(\"staff\", \"network> \");\n"
	"end(\"Leave configuration mode\"), mode(\"staff\", \"network> \");\n"
	"\n"
	"# ===== Shared operational commands (referenced, never activated directly) =====\n"
	"treename = \"base_common\";\n"
	"show(\"Show network configuration\") {\n"
	"    interfaces(\"Show all interfaces\"), show_interfaces();\n"
	"    interface <ifname:string interface()>(\"Interface name\"), show_interface();\n"
	"    history(\"Show history\"), show_history();\n"
	"}\n"
	"help(\"Show help for this mode\"), help_cmd();\n"
	"quit(\"Quit the CLI\"), quit_cmd();\n";

#endif
