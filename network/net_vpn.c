#include "net_vpn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void net_vpn_init(void) { printf("[NetVPN] Initialized.\n"); }

// Launch OpenVPN or WireGuard as a subprocess for real VPN support
void net_vpn_connect(const char* config_path) {
    printf("[NetVPN] Attempting to start VPN using config: %s\n", config_path);
    // Try OpenVPN first
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "openvpn --config \"%s\" --daemon", config_path);
    int ret = system(cmd);
    if (ret != 0) {
        // Try WireGuard
        snprintf(cmd, sizeof(cmd), "wireguard /installtunnelservice \"%s\"", config_path);
        ret = system(cmd);
        if (ret != 0) {
            printf("[NetVPN] Failed to start VPN. Ensure OpenVPN or WireGuard is installed.\n");
        } else {
            printf("[NetVPN] WireGuard tunnel started.\n");
        }
    } else {
        printf("[NetVPN] OpenVPN tunnel started.\n");
    }
} 