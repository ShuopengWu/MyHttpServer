#ifndef SERVER_H
#define SERVER_H

#include "EventLoop.h"
#include "Socket.h"

class Server
{
public:
	Server(EventLoop *_loop);
	~Server();

	void handle_read_event(int client_fd);
	void new_connection(Socket *server_socket);
private:
	EventLoop* loop_;
	std::shared_ptr<Socket> server_socket_;
	std::shared_ptr<InetAddress> server_address_;
	std::unique_ptr<Channel> server_channel_;
};

#endif // !SERVER_H
