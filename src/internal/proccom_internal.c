#include "proccom_internal.h"

// Platform-specific initialization
int proccom_initialize() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed with error %d\n", result);
        return -1;
    }
#endif
    return 0;
}

// Platform-specific shutdown
void proccom_shutdown() {
#ifdef _WIN32
    WSACleanup();
#endif
}

// Create a socket
int socket_create(socket_t* sock, const char* ip, int port) {
    sock->handle = socket(AF_INET, SOCK_STREAM, 0);
    if (sock->handle == SOCKET_INVALID) {
        return SOCKET_ERROR_CODE;
    }

    // Set up the address structure
    sock->address.sin_family = AF_INET;
    sock->address.sin_port = htons(port);
    sock->address.sin_addr.s_addr = inet_addr(ip);

    return 0;
}

// Bind a socket to a specific address and port
int socket_bind(socket_t* sock) {
    if (bind(sock->handle, (struct sockaddr*)&sock->address, sizeof(sock->address)) < 0) {
        return SOCKET_ERROR_CODE;
    }
    return 0;
}

// Connect a socket to a remote address
int socket_connect(socket_t* sock) {
    if (connect(sock->handle, (struct sockaddr*)&sock->address, sizeof(sock->address)) < 0) {
        return SOCKET_ERROR_CODE;
    }
    return 0;
}

// Send data over a socket
int socket_send(socket_t* sock, const void* data, size_t size) {
    int bytes_sent = send(sock->handle, (const char*)data, size, 0);
    if (bytes_sent == SOCKET_ERROR_CODE) {
        return SOCKET_ERROR_CODE;
    }
    return bytes_sent;
}

// Receive data from a socket
int socket_receive(socket_t* sock, void* buffer, size_t size) {
    int bytes_received = recv(sock->handle, (char*)buffer, size, 0);
    if (bytes_received == SOCKET_ERROR_CODE) {
        return SOCKET_ERROR_CODE;
    }
    return bytes_received;
}

// Close the socket
void socket_close(socket_t* sock) {
    closesocket(sock->handle);
}
