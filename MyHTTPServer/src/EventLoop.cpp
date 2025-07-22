#include "EventLoop.h"
#include "Channel.h"

EventLoop::EventLoop() : epoll_(std::make_unique<Epoll>()), quit_(false)
{

}

void EventLoop::loop()
{
	if (!quit_)
	{
		std::vector<Channel*> channels = epoll_->poll();
		for (Channel *channel : channels) {
			channel->handle_callback();
		}
	}
}

void EventLoop::update_channel(Channel* channel)
{
	epoll_->update_channel(channel);
}