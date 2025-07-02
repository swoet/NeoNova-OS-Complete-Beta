#include "net_if.h"
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

static SOCKET udp_sock = INVALID_SOCKET;
static struct sockaddr_in dest_addr = {0};

void net_if_init(void) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("[NetIF] WSAStartup failed\n");
        return;
    }
    udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_sock == INVALID_SOCKET) {
        printf("[NetIF] Failed to create UDP socket\n");
        return;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(12345); // Demo port
    inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);
    printf("[NetIF] Initialized (UDP demo on 127.0.0.1:12345)\n");
}

int net_if_send(const void* data, int len) {
    if (udp_sock == INVALID_SOCKET) return -1;
    int sent = sendto(udp_sock, (const char*)data, len, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (sent == SOCKET_ERROR) {
        printf("[NetIF] Send error: %d\n", WSAGetLastError());
        return -1;
    }
    printf("[NetIF] Sent %d bytes\n", sent);
    return sent;
}

int net_if_recv(void* buf, int maxlen) {
    if (udp_sock == INVALID_SOCKET) return -1;
    struct sockaddr_in from;
    int fromlen = sizeof(from);
    int recvd = recvfrom(udp_sock, (char*)buf, maxlen, 0, (struct sockaddr*)&from, &fromlen);
    if (recvd == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) printf("[NetIF] Recv error: %d\n", err);
        return -1;
    }
    printf("[NetIF] Received %d bytes\n", recvd);
    return recvd;
} 