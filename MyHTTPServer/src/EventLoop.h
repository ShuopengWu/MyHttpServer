#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <memory>

#include "Epoll.h"

class Channel;

class EventLoop
{
public:
	EventLoop();
	void loop();
	void update_channel(Channel *channel);
private:
	std::unique_ptr<Epoll> epoll_;
	bool quit_;
};

#endif // !EVENTLOOP_H
