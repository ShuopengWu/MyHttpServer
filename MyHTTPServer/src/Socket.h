#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include "InetAddress.h"

class Socket
{
    static constexpr int DEFAULT_BACKLOG = 0x0400;
public:
    Socket();
    Socket(int socket_fd);
    ~Socket();
    int get_socket_fd() const;
    void bind_socket(const InetAddress &server_addr) const;
    void listen_socket(int backlog = DEFAULT_BACKLOG);
    void set_socket_non_blocking() const;
    int accept_client_socket(InetAddress &client_addr) const;
private:
    int socket_fd_;
};

#endif