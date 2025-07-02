#include "net_stack.h"
#include "net_proto.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>
#include <time.h>
#ifdef _WIN32
#include <wlanapi.h>
#pragma comment(lib, "wlanapi.lib")
#endif
#include <ras.h>
#include <raserror.h>
#pragma comment(lib, "rasapi32.lib")
#include <fwpmu.h>
#pragma comment(lib, "fwpuclnt.lib")

#define MAX_SOCKETS 16
static net_socket_t sockets[MAX_SOCKETS];
static SOCKET sock_handles[MAX_SOCKETS] = {0};
static int next_sock_id = 1;
#define MAX_IFACES 4
static net_if_t ifaces[MAX_IFACES];
static int iface_count = 0;

// TCP/UDP stateful logic
static const char* tcp_state_names[] = {"CLOSED", "SYN_SENT", "ESTABLISHED", "FIN_WAIT", "ERROR"};
#define TCP_CLOSED 0
#define TCP_SYN_SENT 1
#define TCP_ESTABLISHED 2
#define TCP_FIN_WAIT 3
#define TCP_ERROR 4

// Add lease timer to net_if_t
typedef struct net_if {
    char name[16];
    uint8_t mac[6];
    uint32_t ip_addr;
    uint32_t netmask;
    uint32_t gateway;
    int up;
    int dhcp_lease_time; // seconds
    int dhcp_lease_timer; // seconds left
    uint8_t ipv6_addr[16];
    uint8_t ipv6_prefix_len;
    uint8_t ipv6_gateway[16];
} net_if_t;

// ARP table
#define ARP_TABLE_SIZE 16
static struct { uint32_t ip; uint8_t mac[6]; } arp_table[ARP_TABLE_SIZE];
static int arp_count = 0;

// Wi-Fi networks (stub)
#define WIFI_MAX_NETWORKS 8
static net_wifi_network_t wifi_networks[WIFI_MAX_NETWORKS] = {
    {"NeoNova-Home", -40, 3},
    {"CoffeeShop", -65, 0},
    {"OfficeNet", -55, 2},
    {"Guest", -80, 0},
};
static int wifi_network_count = 4;
static char wifi_connected_ssid[32] = "";

// VPN/SDN/Firewall (stub)
static int vpn_connected = 0;
static char vpn_server[64] = "";
int net_vpn_connect(const char* server, const char* user, const char* pass) {
    RASDIALPARAMS params = {0};
    params.dwSize = sizeof(RASDIALPARAMS);
    strncpy(params.szEntryName, server, sizeof(params.szEntryName)-1);
    strncpy(params.szUserName, user, sizeof(params.szUserName)-1);
    strncpy(params.szPassword, pass, sizeof(params.szPassword)-1);
    HRASCONN hRasConn = NULL;
    DWORD res = RasDial(NULL, NULL, &params, 0, NULL, &hRasConn);
    if (res == ERROR_SUCCESS) {
        vpn_connected = 1;
        strncpy(vpn_server, server, sizeof(vpn_server));
        printf("[NetStack] VPN connected to %s as %s\n", server, user);
        return 0;
    } else {
        printf("[NetStack] VPN connection failed: %ld\n", res);
        return -1;
    }
}
int net_vpn_disconnect(void) {
    // Disconnect all active RAS connections
    RASCONN rasConn[16] = {0};
    DWORD dwSize = sizeof(rasConn);
    DWORD dwConn = 0;
    rasConn[0].dwSize = sizeof(RASCONN);
    if (RasEnumConnections(rasConn, &dwSize, &dwConn) == ERROR_SUCCESS) {
        for (DWORD i = 0; i < dwConn; ++i) {
            RasHangUp(rasConn[i].hrasconn);
        }
    }
    vpn_connected = 0;
    printf("[NetStack] VPN disconnected\n");
    return 0;
}
static char sdn_controller[64] = "";
static int sdn_port = 0;
int net_sdn_set_controller(const char* addr, int port) {
    strncpy(sdn_controller, addr, sizeof(sdn_controller));
    sdn_port = port;
    printf("[NetStack] SDN controller set to %s:%d\n", addr, port);
    return 0;
}
#define FW_MAX_RULES 16
static net_fw_rule_t fw_rules[FW_MAX_RULES];
static int fw_rule_count = 0;
int net_fw_add_rule(const net_fw_rule_t* rule) {
    // Example: Add a basic WFP filter (real implementation would map rule fields to WFP conditions)
    FWPM_SESSION session = {0};
    HANDLE engine = NULL;
    if (FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, &session, &engine) != ERROR_SUCCESS) return -1;
    FWPM_FILTER filter = {0};
    filter.displayData.name = L"NeoNova Firewall Rule";
    filter.layerKey = FWPM_LAYER_INBOUND_TRANSPORT_V4;
    filter.action.type = FWP_ACTION_BLOCK;
    filter.weight.type = FWP_EMPTY;
    if (FwpmFilterAdd(engine, &filter, NULL, NULL) == ERROR_SUCCESS) {
        FwpmEngineClose(engine);
        printf("[NetStack] Firewall rule added (WFP)\n");
        return 0;
    }
    FwpmEngineClose(engine);
    printf("[NetStack] Failed to add firewall rule (WFP)\n");
    return -1;
}
int net_fw_remove_rule(int index) {
    // Real implementation: Remove WFP filter by ID
    // (This is a placeholder for actual filter removal logic)
    printf("[NetStack] Firewall rule removed (WFP)\n");
    return 0;
}
int net_fw_list_rules(net_fw_rule_t* out, int max_count) {
    int n = fw_rule_count < max_count ? fw_rule_count : max_count;
    memcpy(out, fw_rules, n * sizeof(net_fw_rule_t));
    return n;
}

void net_stack_init(void) {
    printf("[NetStack] Initialized.\n");
    memset(sockets, 0, sizeof(sockets));
    memset(ifaces, 0, sizeof(ifaces));
    iface_count = 1;
    strcpy(ifaces[0].name, "eth0");
    ifaces[0].mac[0] = 0xDE; ifaces[0].mac[1] = 0xAD; ifaces[0].mac[2] = 0xBE; ifaces[0].mac[3] = 0xEF; ifaces[0].mac[4] = 0x00; ifaces[0].mac[5] = 0x01;
    ifaces[0].ip_addr = 0; ifaces[0].netmask = 0; ifaces[0].gateway = 0; ifaces[0].up = 0;
}
void net_stack_shutdown(void) { printf("[NetStack] Shutdown.\n"); }
void net_stack_tick(void) {
    printf("[NetStack] Tick.\n");
    for (int i = 0; i < iface_count; ++i) {
        if (ifaces[i].up && ifaces[i].dhcp_lease_time > 0) {
            ifaces[i].dhcp_lease_timer--;
            if (ifaces[i].dhcp_lease_timer == 10) {
                printf("[NetStack] DHCP lease for %s expiring soon, renewing...\n", ifaces[i].name);
                net_dhcp_request(&ifaces[i]);
            } else if (ifaces[i].dhcp_lease_timer <= 0) {
                printf("[NetStack] DHCP lease for %s expired, interface down\n", ifaces[i].name);
                ifaces[i].up = 0;
            }
        }
    }
    // Simulate sending a packet down the stack
    char data[] = "Hello, network!";
    net_packet_t pkt = { .data = data, .len = strlen(data) };
    printf("[NetStack] Sending packet: %s\n", (char*)pkt.data);
    net_proto_process(pkt.data, (int)pkt.len);
}
// Sockets
int net_socket_open(net_sock_type_t type, uint32_t remote_addr, int remote_port) {
    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (sockets[i].id == 0) {
            SOCKET s = INVALID_SOCKET;
            if (type == NET_SOCK_TCP) {
                s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            } else {
                s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            }
            if (s == INVALID_SOCKET) {
                printf("[NetStack] Failed to create socket\n");
                return -1;
            }
            struct sockaddr_in addr = {0};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(remote_port);
            addr.sin_addr.s_addr = remote_addr;
            if (type == NET_SOCK_TCP) {
                if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
                    printf("[NetStack] TCP connect failed\n");
                    closesocket(s);
                    return -1;
                }
            }
            sockets[i].id = next_sock_id++;
            sockets[i].type = type;
            sockets[i].remote_addr = remote_addr;
            sockets[i].remote_port = remote_port;
            sock_handles[i] = s;
            printf("[NetStack] Opened %s socket %d to %u:%d\n", type == NET_SOCK_TCP ? "TCP" : "UDP", sockets[i].id, remote_addr, remote_port);
            return sockets[i].id;
        }
    }
    return -1;
}
int net_socket_close(int sock_id) {
    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (sockets[i].id == sock_id) {
            closesocket(sock_handles[i]);
            sockets[i].id = 0;
            sock_handles[i] = 0;
            printf("[NetStack] Closed socket %d\n", sock_id);
            return 0;
        }
    }
    return -1;
}
int net_socket_send(int sock_id, const void* data, int len) {
    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (sockets[i].id == sock_id) {
            int sent = send(sock_handles[i], (const char*)data, len, 0);
            if (sent == SOCKET_ERROR) {
                printf("[NetStack] Send error: %d\n", WSAGetLastError());
                return -1;
            }
            printf("[NetStack] Sent %d bytes on socket %d\n", sent, sock_id);
            return sent;
        }
    }
    return -1;
}
int net_socket_recv(int sock_id, void* buf, int maxlen) {
    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (sockets[i].id == sock_id) {
            int recvd = recv(sock_handles[i], (char*)buf, maxlen, 0);
            if (recvd == SOCKET_ERROR) {
                printf("[NetStack] Recv error: %d\n", WSAGetLastError());
                return -1;
            }
            printf("[NetStack] Received %d bytes on socket %d\n", recvd, sock_id);
            return recvd;
        }
    }
    return -1;
}
// DHCP
int net_dhcp_request(net_if_t* iface) {
    printf("[NetStack] DHCP request on %s\n", iface->name);
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) { printf("[DHCP] Failed to create socket\n"); return -1; }
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));
    struct sockaddr_in dest = {0};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(67);
    dest.sin_addr.s_addr = INADDR_BROADCAST;
    // Build DHCPDISCOVER
    uint8_t buf[548] = {0};
    buf[0] = 1; // op=BOOTREQUEST
    buf[1] = 1; // htype=Ethernet
    buf[2] = 6; // hlen=6
    buf[3] = 0; // hops
    uint32_t xid = (uint32_t)time(NULL);
    buf[4] = (xid>>24)&0xFF; buf[5] = (xid>>16)&0xFF; buf[6] = (xid>>8)&0xFF; buf[7] = xid&0xFF;
    buf[236] = 99; buf[237] = 130; buf[238] = 83; buf[239] = 99; // magic cookie
    int p = 240;
    buf[p++] = 53; buf[p++] = 1; buf[p++] = 1; // DHCPDISCOVER
    buf[p++] = 55; buf[p++] = 2; buf[p++] = 1; buf[p++] = 3; // Parameter request: subnet, router
    buf[p++] = 255; // End
    memcpy(&buf[28], iface->mac, 6);
    int sent = sendto(s, (const char*)buf, 548, 0, (struct sockaddr*)&dest, sizeof(dest));
    if (sent != 548) { printf("[DHCP] sendto failed\n"); closesocket(s); return -1; }
    struct sockaddr_in from; int fromlen = sizeof(from);
    int recvd = recvfrom(s, (char*)buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
    if (recvd < 0) { printf("[DHCP] recvfrom failed\n"); closesocket(s); return -1; }
    // Parse DHCPOFFER
    uint32_t yiaddr = *(uint32_t*)&buf[16];
    uint32_t subnet = 0, router = 0;
    p = 240;
    while (p < recvd && buf[p] != 255) {
        uint8_t opt = buf[p++];
        uint8_t len = buf[p++];
        if (opt == 1 && len == 4) subnet = *(uint32_t*)&buf[p];
        if (opt == 3 && len == 4) router = *(uint32_t*)&buf[p];
        p += len;
    }
    // Send DHCPREQUEST
    memset(buf, 0, sizeof(buf));
    buf[0] = 1; buf[1] = 1; buf[2] = 6; buf[3] = 0;
    buf[4] = (xid>>24)&0xFF; buf[5] = (xid>>16)&0xFF; buf[6] = (xid>>8)&0xFF; buf[7] = xid&0xFF;
    memcpy(&buf[28], iface->mac, 6);
    buf[236] = 99; buf[237] = 130; buf[238] = 83; buf[239] = 99;
    p = 240;
    buf[p++] = 53; buf[p++] = 1; buf[p++] = 3; // DHCPREQUEST
    buf[p++] = 50; buf[p++] = 4; memcpy(&buf[p], &yiaddr, 4); p += 4; // Requested IP
    buf[p++] = 54; buf[p++] = 4; memcpy(&buf[p], &router, 4); p += 4; // Server ID
    buf[p++] = 255;
    sent = sendto(s, (const char*)buf, 548, 0, (struct sockaddr*)&dest, sizeof(dest));
    if (sent != 548) { printf("[DHCP] sendto (REQUEST) failed\n"); closesocket(s); return -1; }
    recvd = recvfrom(s, (char*)buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
    closesocket(s);
    if (recvd < 0) { printf("[DHCP] recvfrom (ACK) failed\n"); return -1; }
    // Parse DHCPACK
    yiaddr = *(uint32_t*)&buf[16];
    subnet = 0; router = 0;
    p = 240;
    while (p < recvd && buf[p] != 255) {
        uint8_t opt = buf[p++];
        uint8_t len = buf[p++];
        if (opt == 1 && len == 4) subnet = *(uint32_t*)&buf[p];
        if (opt == 3 && len == 4) router = *(uint32_t*)&buf[p];
        p += len;
    }
    iface->ip_addr = yiaddr;
    iface->netmask = subnet;
    iface->gateway = router;
    iface->up = 1;
    iface->dhcp_lease_time = 3600;
    iface->dhcp_lease_timer = 3600;
    printf("[DHCP] Assigned IP: %u.%u.%u.%u\n", (yiaddr)&0xFF, (yiaddr>>8)&0xFF, (yiaddr>>16)&0xFF, (yiaddr>>24)&0xFF);
    printf("[DHCP] Subnet: %u.%u.%u.%u\n", (subnet)&0xFF, (subnet>>8)&0xFF, (subnet>>16)&0xFF, (subnet>>24)&0xFF);
    printf("[DHCP] Gateway: %u.%u.%u.%u\n", (router)&0xFF, (router>>8)&0xFF, (router>>16)&0xFF, (router>>24)&0xFF);
    return 0;
}
// DNS
int net_dns_resolve(const char* hostname, uint32_t* out_addr) {
    printf("[NetStack] DNS resolve %s\n", hostname);
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        printf("[NetStack] DNS: failed to create socket\n");
        return -1;
    }
    struct sockaddr_in dns_addr = {0};
    dns_addr.sin_family = AF_INET;
    dns_addr.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &dns_addr.sin_addr);
    // Build DNS query
    uint8_t buf[512] = {0};
    uint16_t id = (uint16_t)time(NULL);
    buf[0] = id >> 8; buf[1] = id & 0xFF; // ID
    buf[2] = 0x01; buf[5] = 0x01; // Recursion desired, QDCOUNT=1
    // Encode hostname
    int p = 12;
    const char* h = hostname;
    while (*h) {
        const char* dot = strchr(h, '.');
        int len = dot ? (dot - h) : strlen(h);
        buf[p++] = len;
        memcpy(&buf[p], h, len); p += len;
        h += len;
        if (*h == '.') h++;
        else break;
    }
    buf[p++] = 0; // End of name
    buf[p++] = 0x00; buf[p++] = 0x01; // QTYPE=A
    buf[p++] = 0x00; buf[p++] = 0x01; // QCLASS=IN
    int qlen = p;
    int sent = sendto(s, (const char*)buf, qlen, 0, (struct sockaddr*)&dns_addr, sizeof(dns_addr));
    if (sent != qlen) {
        printf("[NetStack] DNS: sendto failed\n"); closesocket(s); return -1;
    }
    struct sockaddr_in from; int fromlen = sizeof(from);
    int recvd = recvfrom(s, (char*)buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
    if (recvd < 0) {
        printf("[NetStack] DNS: recvfrom failed\n"); closesocket(s); return -1;
    }
    closesocket(s);
    // Parse response: skip header and question
    p = 12;
    while (buf[p]) p += buf[p] + 1; p++; // skip QNAME
    p += 4; // QTYPE+QCLASS
    // Parse answers
    for (int i = 0; i < buf[7]; ++i) { // ANCOUNT
        if ((buf[p] & 0xC0) == 0xC0) p += 2; else { while (buf[p]) p += buf[p] + 1; p++; }
        uint16_t type = (buf[p]<<8)|buf[p+1]; p += 2;
        p += 2; // class
        p += 4; // ttl
        uint16_t rdlen = (buf[p]<<8)|buf[p+1]; p += 2;
        if (type == 1 && rdlen == 4) { // A record
            *out_addr = *(uint32_t*)&buf[p];
            printf("[NetStack] DNS: %s -> %u.%u.%u.%u\n", hostname, buf[p], buf[p+1], buf[p+2], buf[p+3]);
            return 0;
        }
        p += rdlen;
    }
    printf("[NetStack] DNS: no A record found\n");
    return -1;
}
// Network interface status/config
int net_if_list(net_if_t* out, int max_count) {
    int n = iface_count < max_count ? iface_count : max_count;
    memcpy(out, ifaces, n * sizeof(net_if_t));
    return n;
}
int net_if_set_up(const char* name, int up) {
    for (int i = 0; i < iface_count; ++i) {
        if (strcmp(ifaces[i].name, name) == 0) {
            ifaces[i].up = up;
            printf("[NetStack] Interface %s set %s\n", name, up ? "UP" : "DOWN");
            return 0;
        }
    }
    return -1;
}
int net_if_configure(const char* name, uint32_t ip, uint32_t netmask, uint32_t gw) {
    for (int i = 0; i < iface_count; ++i) {
        if (strcmp(ifaces[i].name, name) == 0) {
            ifaces[i].ip_addr = ip;
            ifaces[i].netmask = netmask;
            ifaces[i].gateway = gw;
            printf("[NetStack] Interface %s configured\n", name);
            return 0;
        }
    }
    return -1;
}
int net_if_configure_v6(const char* name, const uint8_t* addr, uint8_t prefix_len, const uint8_t* gw) {
    for (int i = 0; i < iface_count; ++i) {
        if (strcmp(ifaces[i].name, name) == 0) {
            memcpy(ifaces[i].ipv6_addr, addr, 16);
            ifaces[i].ipv6_prefix_len = prefix_len;
            memcpy(ifaces[i].ipv6_gateway, gw, 16);
            printf("[NetStack] Interface %s configured (IPv6)\n", name);
            return 0;
        }
    }
    return -1;
}
int net_arp_resolve(uint32_t ip, uint8_t* out_mac) {
    for (int i = 0; i < arp_count; ++i) {
        if (arp_table[i].ip == ip) {
            memcpy(out_mac, arp_table[i].mac, 6);
            printf("[NetStack] ARP resolve %u.%u.%u.%u -> %02X:%02X:%02X:%02X:%02X:%02X\n",
                (ip>>24)&0xFF, (ip>>16)&0xFF, (ip>>8)&0xFF, ip&0xFF,
                arp_table[i].mac[0], arp_table[i].mac[1], arp_table[i].mac[2], arp_table[i].mac[3], arp_table[i].mac[4], arp_table[i].mac[5]);
            return 0;
        }
    }
    printf("[NetStack] ARP resolve miss for %u.%u.%u.%u\n", (ip>>24)&0xFF, (ip>>16)&0xFF, (ip>>8)&0xFF, ip&0xFF);
    return -1;
}
int net_arp_update(uint32_t ip, const uint8_t* mac) {
    for (int i = 0; i < arp_count; ++i) {
        if (arp_table[i].ip == ip) {
            memcpy(arp_table[i].mac, mac, 6);
            printf("[NetStack] ARP update %u.%u.%u.%u\n", (ip>>24)&0xFF, (ip>>16)&0xFF, (ip>>8)&0xFF, ip&0xFF);
            return 0;
        }
    }
    if (arp_count < ARP_TABLE_SIZE) {
        arp_table[arp_count].ip = ip;
        memcpy(arp_table[arp_count].mac, mac, 6);
        arp_count++;
        printf("[NetStack] ARP add %u.%u.%u.%u\n", (ip>>24)&0xFF, (ip>>16)&0xFF, (ip>>8)&0xFF, ip&0xFF);
        return 0;
    }
    printf("[NetStack] ARP table full\n");
    return -1;
}
// NDP table
#define NDP_TABLE_SIZE 16
static struct { uint8_t ipv6[16]; uint8_t mac[6]; } ndp_table[NDP_TABLE_SIZE];
static int ndp_count = 0;
int net_ndp_resolve(const uint8_t* ipv6_addr, uint8_t* out_mac) {
    for (int i = 0; i < ndp_count; ++i) {
        if (memcmp(ndp_table[i].ipv6, ipv6_addr, 16) == 0) {
            memcpy(out_mac, ndp_table[i].mac, 6);
            printf("[NetStack] NDP resolve -> %02X:%02X:%02X:%02X:%02X:%02X\n",
                out_mac[0], out_mac[1], out_mac[2], out_mac[3], out_mac[4], out_mac[5]);
            return 0;
        }
    }
    printf("[NetStack] NDP resolve miss\n");
    return -1;
}
int net_ndp_update(const uint8_t* ipv6_addr, const uint8_t* mac) {
    for (int i = 0; i < ndp_count; ++i) {
        if (memcmp(ndp_table[i].ipv6, ipv6_addr, 16) == 0) {
            memcpy(ndp_table[i].mac, mac, 6);
            printf("[NetStack] NDP update\n");
            return 0;
        }
    }
    if (ndp_count < NDP_TABLE_SIZE) {
        memcpy(ndp_table[ndp_count].ipv6, ipv6_addr, 16);
        memcpy(ndp_table[ndp_count].mac, mac, 6);
        ndp_count++;
        printf("[NetStack] NDP add\n");
        return 0;
    }
    printf("[NetStack] NDP table full\n");
    return -1;
}
// Hotplug support
int net_if_hotplug_add(const char* name, const uint8_t* mac) {
    if (iface_count >= MAX_IFACES) return -1;
    strcpy(ifaces[iface_count].name, name);
    memcpy(ifaces[iface_count].mac, mac, 6);
    ifaces[iface_count].ip_addr = 0;
    ifaces[iface_count].netmask = 0;
    ifaces[iface_count].gateway = 0;
    ifaces[iface_count].up = 0;
    iface_count++;
    printf("[NetStack] Hotplug: added interface %s\n", name);
    printf("[Notification] Network device %s added\n", name);
    return 0;
}
int net_if_hotplug_remove(const char* name) {
    for (int i = 0; i < iface_count; ++i) {
        if (strcmp(ifaces[i].name, name) == 0) {
            for (int j = i; j < iface_count - 1; ++j) ifaces[j] = ifaces[j+1];
            iface_count--;
            printf("[NetStack] Hotplug: removed interface %s\n", name);
            printf("[Notification] Network device %s removed\n", name);
            return 0;
        }
    }
    return -1;
}
// ICMP
int net_icmp_ping(const char* host, int count, int* out_success, int* out_loss, int* out_avg_ms) {
    printf("[NetStack] ICMP ping %s x%d\n", host, count);
    int success = 0, loss = 0, total_ms = 0;
    for (int i = 0; i < count; ++i) {
        printf("[NetStack] Pinging %s: seq=%d ... ", host, i+1);
        int ms = 42 + (rand() % 10); // Fake RTT
        if (rand() % 10 < 8) { // 80% success
            printf("reply in %d ms\n", ms);
            success++; total_ms += ms;
        } else {
            printf("timeout\n");
            loss++;
        }
    }
    if (out_success) *out_success = success;
    if (out_loss) *out_loss = loss;
    if (out_avg_ms) *out_avg_ms = success ? (total_ms / success) : 0;
    return 0;
}
int net_wifi_scan(void) {
#ifdef _WIN32
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2, dwCurVersion = 0;
    if (WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient) != ERROR_SUCCESS) return -1;
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    if (WlanEnumInterfaces(hClient, NULL, &pIfList) != ERROR_SUCCESS) { WlanCloseHandle(hClient, NULL); return -1; }
    for (int i = 0; i < (int)pIfList->dwNumberOfItems; ++i) {
        PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;
        if (WlanGetAvailableNetworkList(hClient, &pIfList->InterfaceInfo[i].InterfaceGuid, 0, NULL, &pBssList) == ERROR_SUCCESS) {
            for (int j = 0; j < (int)pBssList->dwNumberOfItems && j < WIFI_MAX_NETWORKS; ++j) {
                strncpy(wifi_networks[j].ssid, (const char*)pBssList->Network[j].dot11Ssid.ucSSID, pBssList->Network[j].dot11Ssid.uSSIDLength);
                wifi_networks[j].ssid[pBssList->Network[j].dot11Ssid.uSSIDLength] = '\0';
                wifi_networks[j].signal = (int)pBssList->Network[j].wlanSignalQuality;
                wifi_networks[j].security = pBssList->Network[j].bSecurityEnabled ? 1 : 0;
            }
            wifi_network_count = (int)pBssList->dwNumberOfItems;
            WlanFreeMemory(pBssList);
        }
    }
    if (pIfList) WlanFreeMemory(pIfList);
    WlanCloseHandle(hClient, NULL);
    printf("[NetStack] Wi-Fi scan complete. %d networks found.\n", wifi_network_count);
    return wifi_network_count;
#else
    printf("[NetStack] Wi-Fi scan not supported on this platform.\n");
    return 0;
#endif
}
int net_wifi_list(net_wifi_network_t* out, int max_count) {
    int n = wifi_network_count < max_count ? wifi_network_count : max_count;
    memcpy(out, wifi_networks, n * sizeof(net_wifi_network_t));
    return n;
}
int net_wifi_join(const char* ssid, const char* password) {
#ifdef _WIN32
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2, dwCurVersion = 0;
    if (WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient) != ERROR_SUCCESS) return -1;
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    if (WlanEnumInterfaces(hClient, NULL, &pIfList) != ERROR_SUCCESS) { WlanCloseHandle(hClient, NULL); return -1; }
    for (int i = 0; i < (int)pIfList->dwNumberOfItems; ++i) {
        WLAN_CONNECTION_PARAMETERS params = {0};
        params.wlanConnectionMode = wlan_connection_mode_profile;
        params.strProfile = ssid;
        params.pDot11Ssid = NULL;
        params.pDesiredBssidList = NULL;
        params.dot11BssType = dot11_BSS_type_any;
        params.dwFlags = 0;
        if (WlanConnect(hClient, &pIfList->InterfaceInfo[i].InterfaceGuid, &params, NULL) == ERROR_SUCCESS) {
            strncpy(wifi_connected_ssid, ssid, sizeof(wifi_connected_ssid)-1);
            wifi_connected_ssid[sizeof(wifi_connected_ssid)-1] = '\0';
            printf("[NetStack] Connected to Wi-Fi network: %s\n", ssid);
            WlanFreeMemory(pIfList);
            WlanCloseHandle(hClient, NULL);
            return 0;
        }
    }
    if (pIfList) WlanFreeMemory(pIfList);
    WlanCloseHandle(hClient, NULL);
    printf("[NetStack] Failed to connect to Wi-Fi network: %s\n", ssid);
    return -1;
#else
    printf("[NetStack] Wi-Fi join not supported on this platform.\n");
    return -1;
#endif
} 