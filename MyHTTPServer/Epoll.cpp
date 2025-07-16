#include "Epoll.h"
#include "util.h"

Epoll::Epoll()
{
    epoll_fd_ = epoll_create1(0);
    error_if(epoll_fd_ == -1, "epoll create error");
    events = new epoll_event[MAX_EVENT_SIZE];
    memset(events, 0, sizeof(*events) * MAX_EVENT_SIZE); //不要使用events(指针)的size
}

Epoll::~Epoll()
{
    if (epoll_fd_ != -1)
        close(epoll_fd_);
    delete []events;
}

void Epoll::add_event(int fd, uint32_t event_flags)
{
    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = event_flags;
    error_if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1, "add event to epoll failed");
}

void Epoll::delete_event(int fd)
{
    error_if(epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1, "delete event to epoll failed");
}

std::vector<epoll_event> Epoll::poll(int timeout)
{
    int nfds = epoll_wait(epoll_fd_, events, MAX_EVENT_SIZE, timeout);
    error_if(nfds == -1, "epoll waiting error");
    
    return std::vector<epoll_event>(events, events + nfds);
}
