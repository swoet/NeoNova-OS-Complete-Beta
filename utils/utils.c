// System utilities and CLI tools

#include <stdio.h>
#include <string.h>

void utils_help() {
    printf("[Utils] Available commands: help, version, echo <msg>\n");
}

void utils_version() {
    printf("[Utils] NeoNova OS Utilities v1.0\n");
}

void utils_echo(const char* msg) {
    printf("[Utils] %s\n", msg);
}
