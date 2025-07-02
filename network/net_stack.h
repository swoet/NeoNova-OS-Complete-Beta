#ifndef NET_STACK_H
#define NET_STACK_H
#include <stddef.h>
#include <stdint.h>
// Generic network packet structure
typedef struct net_packet {
    void* data;
    size_t len;
} net_packet_t;
// Socket types
typedef enum { NET_SOCK_TCP, NET_SOCK_UDP } net_sock_type_t;
typedef struct net_socket {
    int id;
    net_sock_type_t type;
    int local_port;
    int remote_port;
    uint32_t remote_addr;
    int state;
} net_socket_t;
// Network interface
typedef struct net_if {
    char name[16];
    uint8_t mac[6];
    uint32_t ip_addr;
    uint32_t netmask;
    uint32_t gateway;
    uint8_t ipv6_addr[16];
    uint8_t ipv6_prefix_len;
    uint8_t ipv6_gateway[16];
    int up;
    int dhcp_lease_time; // seconds
    int dhcp_lease_timer; // seconds left
} net_if_t;
// API
void net_stack_init(void);
void net_stack_shutdown(void);
void net_stack_tick(void);
// Sockets
int net_socket_open(net_sock_type_t type, uint32_t remote_addr, int remote_port);
int net_socket_close(int sock_id);
int net_socket_send(int sock_id, const void* data, int len);
int net_socket_recv(int sock_id, void* buf, int maxlen);
// DHCP
int net_dhcp_request(net_if_t* iface);
// DNS
int net_dns_resolve(const char* hostname, uint32_t* out_addr);
// Network interface status/config
int net_if_list(net_if_t* out, int max_count);
int net_if_set_up(const char* name, int up);
int net_if_configure(const char* name, uint32_t ip, uint32_t netmask, uint32_t gw);
int net_if_hotplug_add(const char* name, const uint8_t* mac);
int net_if_hotplug_remove(const char* name);
// ICMP
int net_icmp_ping(const char* host, int count, int* out_success, int* out_loss, int* out_avg_ms);
// IPv6 config
int net_if_configure_v6(const char* name, const uint8_t* addr, uint8_t prefix_len, const uint8_t* gw);
// NDP
int net_ndp_resolve(const uint8_t* ipv6_addr, uint8_t* out_mac);
int net_ndp_update(const uint8_t* ipv6_addr, const uint8_t* mac);
// ARP
int net_arp_resolve(uint32_t ip, uint8_t* out_mac);
int net_arp_update(uint32_t ip, const uint8_t* mac);
// Wi-Fi
typedef struct net_wifi_network {
    char ssid[32];
    int signal_strength; // dBm
    int security; // 0=open, 1=WEP, 2=WPA, 3=WPA2
} net_wifi_network_t;
int net_wifi_scan(void);
int net_wifi_list(net_wifi_network_t* out, int max_count);
int net_wifi_join(const char* ssid, const char* password);
// Firewall
typedef struct net_fw_rule {
    char action[8]; // "allow" or "deny"
    char proto[8];  // "tcp", "udp", "icmp", etc.
    uint32_t src_ip, dst_ip;
    int src_port, dst_port;
} net_fw_rule_t;
int net_vpn_connect(const char* server, const char* user, const char* pass);
int net_vpn_disconnect(void);
int net_sdn_set_controller(const char* addr, int port);
int net_fw_add_rule(const net_fw_rule_t* rule);
int net_fw_remove_rule(int index);
int net_fw_list_rules(net_fw_rule_t* out, int max_count);
#endif // NET_STACK_H 