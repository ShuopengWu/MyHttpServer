#ifndef CHANNEL_H
#define CHANNEL_H

#include "Epoll.h"

class Epoll;

class Channel
{
public:
	Channel(Epoll *_ep, int _fd);

	int get_fd() const;
	uint32_t get_events() const;
	uint32_t get_revents() const;
	bool is_in_epoll() const;

	void set_revents(uint32_t _revents);
	void set_in_epoll(bool _in_epoll);

	bool enable_reading();
private:
	Epoll* epoll_;
	int fd_;
	uint32_t events_;
	uint32_t revents_;
	bool in_epoll_;
};


#endif // !CHANNEL_H
