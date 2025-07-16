#include "Socket.h"
#include "util.h"

Socket::Socket()
{
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    error_if(socket_fd_ ==  -1, "create socket failed");
}

Socket::Socket(int socket_fd)
{
    socket_fd_ = socket_fd;
    error_if(socket_fd_ ==  -1, "Invalid socket file descriptor passed to constructor");
}

Socket::~Socket()
{
    if (socket_fd_ != -1)
        close(socket_fd_);
}

int Socket::get_socket_fd() const
{
    return socket_fd_;
}

void Socket::bind_socket(const InetAddress &server_addr)
{
    const sockaddr_in &addr = server_addr.get_addr();
    error_if(bind(socket_fd_, (sockaddr *)&addr, sizeof(addr)) == -1, "bind socket failed");
}

void Socket::listen_socket(int backlog)
{
    error_if(listen(socket_fd_, backlog) < 0, "socket listen failed");
}

void Socket::set_socket_non_blocking()
{
    error_if(fcntl(socket_fd_, F_SETFL, fcntl(socket_fd_, F_GETFL) | O_NONBLOCK) == -1, "failed to set socket to non-blocking state");
}

int Socket::accept_client_socket(InetAddress &client_addr)
{
    sockaddr_in addr = client_addr.get_addr();
    socklen_t addr_len = sizeof(addr);
    return accept(socket_fd_, (sockaddr *)&addr, &addr_len);
}
