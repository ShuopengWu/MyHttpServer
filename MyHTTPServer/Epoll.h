#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <cstring>

class Epoll {
    static constexpr int MAX_EVENT_SIZE = 0x0400;
public:
    Epoll();
    ~Epoll();
    void add_event(int fd, uint32_t event_flags);
    void delete_event(int fd);
    std::vector<epoll_event> poll(int timeout = -1);
private:
    int epoll_fd_;
    epoll_event *events;
};

#endif
