#ifndef NET_PROTO_H
#define NET_PROTO_H
void net_proto_init(void);
void net_proto_process(const void* data, int len);
void net_proto_ethernet_init(void);
void net_proto_arp_init(void);
void net_proto_ipv4_init(void);
void net_proto_udp_init(void);
void net_proto_tcp_init(void);
void net_proto_register(void (*process)(const void*, int));
#endif // NET_PROTO_H 