#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* _loop, int _fd) : loop_(_loop), fd_(_fd), in_epoll_(false)
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

void Channel::handle_callback()
{
	callback_();
}

void Channel::set_revents(uint32_t _revents)
{
	revents_ = _revents;
}

void Channel::set_in_epoll(bool _in_epoll)
{
	in_epoll_ = _in_epoll;
}

void Channel::set_callback(std::function<void()> _callback)
{
	callback_ = _callback;
}

bool Channel::enable_reading()
{
	if (fd_ == -1)
		return false;

	events_ = EPOLLIN | EPOLLET;
	loop_->update_channel(this);

	return true;
}