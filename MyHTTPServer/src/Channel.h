#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <cstdint>

class EventLoop;

class Channel
{
public:
	Channel(EventLoop *_loop, int _fd);

	int get_fd() const;
	uint32_t get_events() const;
	uint32_t get_revents() const;
	bool is_in_epoll() const;
	void handle_callback();

	void set_revents(uint32_t _revents);
	void set_in_epoll(bool _in_epoll);
	void set_callback(std::function<void()> _callback);

	bool enable_reading();
private:
	EventLoop* loop_;
	int fd_;
	uint32_t events_;
	uint32_t revents_;
	bool in_epoll_;
	std::function<void()> callback_;
};


#endif // !CHANNEL_H
