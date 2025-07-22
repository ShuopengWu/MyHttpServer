#include "Channel.h"

Channel::Channel(Epoll* _ep, int _fd) : epoll_(_ep), fd_(_fd)
{
}

int Channel::get_fd() const
{
	return fd_;
}

uint32_t Channel::get_events() const
{
	return events_;
}

uint32_t Channel::get_revents() const
{
	return revents_;
}

bool Channel::is_in_epoll() const
{
	return in_epoll_;
}

void Channel::set_revents(uint32_t _revents)
{
	revents_ = _revents;
}

void Channel::set_in_epoll(bool _in_epoll)
{
	in_epoll_ = _in_epoll;
}

bool Channel::enable_reading()
{
	if (fd_ == -1)
		return false;

	events_ = EPOLLIN | EPOLLET;
	epoll_->update_channel(this);

	return true;
}