#ifndef PROC_COM_INTERNAL
#define PROC_COM_INTERNAL

#ifdef __cplusplus
extern "C" {
#endif

    // Platform-specific includes and definitions

    // Windows platform
#ifdef _WIN32
#define WINDOWS_LEAN_AND_MEAN // Exclude unnecessary Windows headers
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h> // For CreateThread
#pragma comment(lib, "Ws2_32.lib")

// Define socket type for Windows
    typedef SOCKET socket_handle_t;
#define SOCKET_INVALID INVALID_SOCKET
#define SOCKET_ERROR_CODE SOCKET_ERROR

    // Linux platform
#elif __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // For close()
#include <pthread.h> // For pthread
#define SOCKET_INVALID -1
#define SOCKET_ERROR_CODE -1
    typedef int socket_handle_t

        // macOS (Apple platform)
#elif __APPLE__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // For close()
#include <pthread.h> // For pthread
#define SOCKET_INVALID -1
#define SOCKET_ERROR_CODE -1
    typedef int socket_handle_t

#else
#error "Unsupported platform"
#endif

// Common includes for all platforms
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <stdbool.h>

    typedef struct {
        socket_handle_t handle;        // Socket handle (platform-specific)
        struct sockaddr_in address;    // Socket address (IPv4)
    } socket_t;

    // Platform-specific function declarations

    // Initializes the socket system (e.g., Winsock for Windows)
    int proccom_initialize();

    // Cleans up the socket system (e.g., WSACleanup for Windows)
    void proccom_shutdown();

    // Create a socket and return its handle
    int socket_create(socket_t* sock, const char* ip, int port);

    // Bind a socket to a specific address and port
    int socket_bind(socket_t* sock);

    // Connect a socket to a remote address
    int socket_connect(socket_t* sock);

    // Send data over a socket
    int socket_send(socket_t* sock, const void* data, size_t size);

    // Receive data from a socket
    int socket_receive(socket_t* sock, void* buffer, size_t size);

    // Close the socket
    void socket_close(socket_t* sock);

#ifdef __cplusplus
}
#endif

#endif // PROC_COM_INTERNAL
