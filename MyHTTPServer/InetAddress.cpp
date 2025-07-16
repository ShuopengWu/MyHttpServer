#include "InetAddress.h"

InetAddress::InetAddress(ushort port)
{
    set_addr(port, "0.0.0.0");
}

InetAddress::InetAddress(std::string ip, ushort port)
{
    set_addr(port, ip.c_str());
}

InetAddress::InetAddress(const char *ip, ushort port)
{
    set_addr(port, ip);
}

sockaddr_in InetAddress::get_addr() const
{
    return addr_in_;
}

void InetAddress::set_addr(ushort port, const char *ip, int family)
{
    memset(&addr_in_, 0, sizeof(addr_in_));
    addr_in_.sin_family = AF_INET;
    if (strlen(ip) == 0)
        addr_in_.sin_addr.s_addr = INADDR_ANY;
    else {
        if (inet_pton(AF_INET, ip, &addr_in_.sin_addr.s_addr) <= 0) {
            std::cerr << "Invalid IP address provided: " << ip << ". Defaulting to INADDR_ANY." << std::endl;
            addr_in_.sin_addr.s_addr = INADDR_ANY;
        }
    }
    addr_in_.sin_port = htons(port);
}
