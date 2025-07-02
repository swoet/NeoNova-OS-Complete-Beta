#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../network/net_stack.h"

void print_usage(void) {
    printf("Usage: netmgr <command> [args]\n");
    printf("  list\n");
    printf("  up <iface>\n");
    printf("  down <iface>\n");
    printf("  config <iface> <ip> <netmask> <gw>\n");
    printf("  ping <ip>\n");
    printf("  dns <hostname>\n");
}

int main(int argc, char** argv) {
    if (argc < 2) { print_usage(); return 1; }
    net_stack_init();
    if (strcmp(argv[1], "list") == 0) {
        net_if_t ifs[8];
        int n = net_if_list(ifs, 8);
        for (int i = 0; i < n; ++i) {
            printf("%s  MAC=%02X:%02X:%02X:%02X:%02X:%02X  IP=%u.%u.%u.%u  %s\n",
                ifs[i].name,
                ifs[i].mac[0], ifs[i].mac[1], ifs[i].mac[2], ifs[i].mac[3], ifs[i].mac[4], ifs[i].mac[5],
                (ifs[i].ip_addr >> 24) & 0xFF, (ifs[i].ip_addr >> 16) & 0xFF, (ifs[i].ip_addr >> 8) & 0xFF, ifs[i].ip_addr & 0xFF,
                ifs[i].up ? "UP" : "DOWN");
        }
    } else if (strcmp(argv[1], "up") == 0 && argc == 3) {
        net_if_set_up(argv[2], 1);
    } else if (strcmp(argv[1], "down") == 0 && argc == 3) {
        net_if_set_up(argv[2], 0);
    } else if (strcmp(argv[1], "config") == 0 && argc == 6) {
        uint32_t ip = inet_addr(argv[3]);
        uint32_t mask = inet_addr(argv[4]);
        uint32_t gw = inet_addr(argv[5]);
        net_if_configure(argv[2], ip, mask, gw);
    } else if (strcmp(argv[1], "ping") == 0 && argc == 3) {
        int success = 0, loss = 0, avg = 0;
        net_icmp_ping(argv[2], 4, &success, &loss, &avg);
        printf("[netmgr] Ping stats: success=%d loss=%d avg=%dms\n", success, loss, avg);
    } else if (strcmp(argv[1], "dns") == 0 && argc == 3) {
        uint32_t addr = 0;
        if (net_dns_resolve(argv[2], &addr) == 0) {
            printf("[netmgr] %s resolved to %u.%u.%u.%u\n", argv[2], (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF);
        } else {
            printf("[netmgr] DNS resolution failed for %s\n", argv[2]);
        }
    } else {
        print_usage();
        return 1;
    }
    return 0;
} 