#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <iostream>

class InetAddress
{
    static constexpr ushort DEFAULT_PORT = 8080;
public:
    explicit InetAddress(ushort port = DEFAULT_PORT);
    explicit InetAddress(std::string ip, ushort port = DEFAULT_PORT);
    explicit InetAddress(const char *ip, ushort port = DEFAULT_PORT);
    sockaddr_in get_addr() const;
private:
    void set_addr(ushort port, const char *ip, int family = AF_INET);
    sockaddr_in addr_in_;
};

#endif
