#ifndef NETWORK_SPEC_H
#define NETWORK_SPEC_H

static const char *embedded_cli_spec =
"# Network Configuration CLI Specification\n"
"# This file defines the CLI syntax using CLIgen format\n"
"\n"
"prompt = \"network> \";\n"
"comment = \"#\";\n"
"\n"
"# Show available interfaces dynamically\n"
"show(\"Show network configuration\") {\n"
"    interfaces(\"Show all interfaces\"), show_interfaces();\n"
"    interface <ifname:string interface()>(\"Interface name\"), show_interface();\n"
"}\n"
"\n"
"# Configure interface IP address\n"
"config(\"Configure network settings\") {\n"
"    interface <ifname:string interface()>(\"Interface name\") {\n"
"        ip <ipaddr:ipv4addr>(\"IP address\"), set_interface_ip();\n"
"        no ip(\"remove IP address\"), remove_interface_ip();\n"
"        ipv6 <ipaddr:ipv6addr>(\"IPv6 address\"), set_interface_ipv6();\n"
"        no ipv6(\"remove IPv6 address\"), remove_interface_ipv6();\n"
"        mtu <size:uint16 range[68:65535]>(\"MTU size\"), set_mtu();\n"
"    }\n"
"}\n"
"\n"
"# Other commands\n"
"help(\"Show help\"), help_cmd();\n"
"quit(\"Quit the CLI\"), quit_cmd();\n";

#endif