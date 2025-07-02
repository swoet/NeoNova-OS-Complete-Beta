#ifndef NET_IF_H
#define NET_IF_H
void net_if_init(void);
int net_if_send(const void* data, int len);
int net_if_recv(void* buf, int maxlen);
#endif // NET_IF_H 