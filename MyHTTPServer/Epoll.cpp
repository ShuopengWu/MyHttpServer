#include "Epoll.h"
#include "util.h"

Epoll::Epoll()
{
    epoll_fd_ = epoll_create1(0);
    error_if(epoll_fd_ == -1, "epoll create error");
    events = new epoll_event[MAX_EVENT_SIZE];
    memset(events, 0, sizeof(epoll_event) * MAX_EVENT_SIZE);
}

Epoll::~Epoll()
{
    if (epoll_fd_ != -1)
        close(epoll_fd_);
    delete[]events;
}

std::vector<Channel *> Epoll::poll(int timeout)
{
    int nfds = epoll_wait(epoll_fd_, events, MAX_EVENT_SIZE, timeout);
    error_if(nfds == -1, "epoll waiting error");

    std::vector<Channel *> actice_channels;

    for (int i = 0; i < nfds; i++) {
        actice_channels.push_back(static_cast<Channel*>(events[i].data.ptr));
        actice_channels.back()->set_revents(events[i].events);
    }

    return actice_channels;
}

//bug : 目前无法接受相同ip第二次连接
void Epoll::update_channel(Channel* channel)
{
    int fd = channel->get_fd();
    epoll_event ev;
    ev.data.ptr = channel;
    ev.events = channel->get_events();
    if (channel->is_in_epoll()) {
        error_if(epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll update event failed!");
    }
    else {
        error_if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event failed!");
    }
    channel->set_in_epoll(true);
}
