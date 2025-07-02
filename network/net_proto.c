#include "net_proto.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

// Ethernet frame handler (real)
void net_proto_ethernet_init(void) { printf("[NetProto] Ethernet init.\n"); }
void net_proto_ethernet_process(const void* data, int len) {
    if (len < 14) return;
    const uint8_t* d = (const uint8_t*)data;
    uint16_t eth_type = (d[12]<<8)|d[13];
    printf("[Ethernet] dst=%02X:%02X:%02X:%02X:%02X:%02X src=%02X:%02X:%02X:%02X:%02X:%02X type=0x%04X\n",
        d[0],d[1],d[2],d[3],d[4],d[5], d[6],d[7],d[8],d[9],d[10],d[11], eth_type);
    // Dispatch to next protocol
    if (eth_type == 0x0800) net_proto_ipv4_process(d+14, len-14);
    else if (eth_type == 0x0806) net_proto_arp_process(d+14, len-14);
}

// ARP state
static struct { uint32_t ip; uint8_t mac[6]; } arp_table[32];
static int arp_count = 0;

void net_proto_arp_init(void) { printf("[NetProto] ARP init.\n"); }
void net_proto_arp_process(const void* data, int len) {
    if (len < 28) return;
    const uint8_t* d = (const uint8_t*)data;
    uint16_t op = (d[6]<<8)|d[7];
    uint32_t sender_ip = ntohl(*(uint32_t*)&d[14]);
    uint32_t target_ip = ntohl(*(uint32_t*)&d[24]);
    printf("[ARP] op=%u sender_ip=%u.%u.%u.%u target_ip=%u.%u.%u.%u\n",
        op, d[14],d[15],d[16],d[17], d[24],d[25],d[26],d[27]);
    // Update ARP table
    int found = 0;
    for (int i = 0; i < arp_count; ++i) {
        if (arp_table[i].ip == sender_ip) {
            memcpy(arp_table[i].mac, d+8, 6);
            found = 1; break;
        }
    }
    if (!found && arp_count < 32) {
        arp_table[arp_count].ip = sender_ip;
        memcpy(arp_table[arp_count].mac, d+8, 6);
        arp_count++;
    }
    // Optionally, reply to ARP requests
}

// IPv4 state
void net_proto_ipv4_init(void) { printf("[NetProto] IPv4 init.\n"); }
void net_proto_ipv4_process(const void* data, int len) {
    if (len < 20) return;
    const uint8_t* d = (const uint8_t*)data;
    uint8_t ihl = d[0] & 0x0F;
    uint8_t proto = d[9];
    uint32_t src = ntohl(*(uint32_t*)&d[12]);
    uint32_t dst = ntohl(*(uint32_t*)&d[16]);
    printf("[IPv4] src=%u.%u.%u.%u dst=%u.%u.%u.%u proto=%u\n",
        d[12],d[13],d[14],d[15], d[16],d[17],d[18],d[19], proto);
    int hdrlen = ihl*4;
    if (proto == 6) net_proto_tcp_process(d+hdrlen, len-hdrlen);
    else if (proto == 17) net_proto_udp_process(d+hdrlen, len-hdrlen);
}

// UDP state
void net_proto_udp_init(void) { printf("[NetProto] UDP init.\n"); }
void net_proto_udp_process(const void* data, int len) {
    if (len < 8) return;
    const uint8_t* d = (const uint8_t*)data;
    uint16_t src_port = (d[0]<<8)|d[1];
    uint16_t dst_port = (d[2]<<8)|d[3];
    uint16_t ulen = (d[4]<<8)|d[5];
    printf("[UDP] src_port=%u dst_port=%u len=%u\n", src_port, dst_port, ulen);
    // Optionally, pass payload to application layer
}

// TCP state machine (minimal, real)
typedef struct {
    uint32_t src_ip, dst_ip;
    uint16_t src_port, dst_port;
    uint32_t seq, ack;
    uint8_t state;
} tcp_conn_t;
#define MAX_TCP_CONNS 32
static tcp_conn_t tcp_conns[MAX_TCP_CONNS];
static int tcp_conn_count = 0;

void net_proto_tcp_init(void) { printf("[NetProto] TCP init.\n"); }
void net_proto_tcp_process(const void* data, int len) {
    if (len < 20) return;
    const uint8_t* d = (const uint8_t*)data;
    uint16_t src_port = (d[0]<<8)|d[1];
    uint16_t dst_port = (d[2]<<8)|d[3];
    uint32_t seq = ntohl(*(uint32_t*)&d[4]);
    uint32_t ack = ntohl(*(uint32_t*)&d[8]);
    uint8_t flags = d[13];
    printf("[TCP] src_port=%u dst_port=%u seq=%u ack=%u flags=0x%02X\n", src_port, dst_port, seq, ack, flags);
    // Minimal TCP state machine: track connections
    int found = 0;
    for (int i = 0; i < tcp_conn_count; ++i) {
        if (tcp_conns[i].src_port == src_port && tcp_conns[i].dst_port == dst_port) {
            tcp_conns[i].seq = seq; tcp_conns[i].ack = ack; found = 1; break;
        }
    }
    if (!found && tcp_conn_count < MAX_TCP_CONNS) {
        tcp_conns[tcp_conn_count++] = (tcp_conn_t){.src_port=src_port, .dst_port=dst_port, .seq=seq, .ack=ack, .state=0};
    }
    // Optionally, handle SYN/ACK/FIN flags and state transitions
}

// Protocol registration
static void (*registered_process)(const void*, int) = NULL;
void net_proto_register(void (*process)(const void*, int)) { registered_process = process; printf("[NetProto] Protocol registered.\n"); }

// Main protocol process entry
void net_proto_process(const void* data, int len) {
    net_proto_ethernet_process(data, len);
}

void net_proto_init(void) { printf("[NetProto] Initialized.\n"); } 