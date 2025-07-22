#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <cstring>

class Channel;

class Epoll {
    static constexpr int MAX_EVENT_SIZE = 0x0400;
public:
    Epoll();
    ~Epoll();
    std::vector<Channel *> poll(int timeout = -1);
    void update_channel(Channel* channel) const;
private:
    int epoll_fd_;
    epoll_event *events;
};

#endif
