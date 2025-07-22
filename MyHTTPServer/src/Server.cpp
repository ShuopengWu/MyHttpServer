#include "Server.h"
#include "Channel.h"
#include "util.h"
#include "Epoll.h"

Server::Server(EventLoop* _loop) : loop_(_loop)
{
	server_socket_ = std::make_unique<Socket>();
	server_address_ = std::make_unique<InetAddress>();

	server_socket_->bind_socket(*server_address_);
	server_socket_->listen_socket();
	std::cout << "server start..." << std::endl;
	server_socket_->set_socket_non_blocking();

	server_channel_ = std::make_unique<Channel>(loop_, server_socket_->get_socket_fd());
	auto callback = std::bind(&Server::new_connection, this, server_socket_.get());
	server_channel_->set_callback(callback);
	server_channel_->enable_reading();
}

Server::~Server()
{
}

void Server::handle_read_event(int client_fd)
{
	static constexpr int MAX_READ_BUFFER_SZIE = 0x0400;
	char buffer[MAX_READ_BUFFER_SZIE];
	while (true) {
		memset(buffer, 0, MAX_READ_BUFFER_SZIE);
		auto read_size = recv(client_fd, buffer, sizeof(buffer), 0);

		if (read_size > 0) {
			std::cout << "receive client " << client_fd << " (" << read_size << " bytes):" << std::endl;
			std::cout.write(buffer, read_size);
			std::cout << std::endl;
			//TBD:回显,后续实现完整发送逻辑
			send(client_fd, buffer, read_size, 0);
		}
		else if (read_size == -1) {
			if (errno == EINTR) { //客户端中断
				std::cout << "Client interrupted, continue to read" << std::endl;
				continue;
			}
			else if (errno == EAGAIN || errno == EWOULDBLOCK) {	//读取完毕
				std::cout << "finish reading!" << std::endl;
				break;
			}
		}
		else if (read_size == 0) { //客户端退出
			std::cout << "client " << client_fd << " disconnection!" << std::endl;
			close(client_fd);
			break;
		}
	}
}

void Server::new_connection(Socket* server_socket)
{
	InetAddress client_address;
	int client_socket_fd = server_socket->accept_client_socket(client_address);

	if (client_socket_fd == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		else {
			std::cerr << "accept error occur, statu is " << errno << std::endl;
			return;
		}
	}
	// TBD : Socket::set_socket_non_blocking static化?
	auto result = fcntl(client_socket_fd, F_SETFL, fcntl(client_socket_fd, F_GETFL) | O_NONBLOCK);
	error_if(result == -1, "set socket non block failed!");
	Channel* client_channel = new Channel(loop_, client_socket_fd);
	auto callback = std::bind(&Server::handle_read_event, this, client_socket_fd);
	client_channel->set_callback(callback);
	client_channel->enable_reading();
}
