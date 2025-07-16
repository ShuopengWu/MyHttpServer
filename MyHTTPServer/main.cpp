#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "util.h"
#include <errno.h>
#include <memory>

void handle_read_event(Epoll *epoll, int client_socket_fd)
{
	static constexpr int MAX_READ_BUFFER_SZIE = 0x0400;
	char buffer[MAX_READ_BUFFER_SZIE];
	while (true) {
		memset(buffer, 0, MAX_READ_BUFFER_SZIE);
		auto read_size = recv(client_socket_fd, buffer, sizeof(buffer), 0);

		if (read_size > 0) {
			std::cout << "receive client " << client_socket_fd << " (" << read_size << " bytes):" << std::endl;
    		std::cout.write(buffer, read_size);
    		std::cout << std::endl;
			//TBD:回显,后续实现完整发送逻辑
			send(client_socket_fd, buffer, read_size, 0);
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
			std::cout << "client " << client_socket_fd << " disconnection!" << std::endl;
			epoll->delete_event(client_socket_fd);
			close(client_socket_fd);
			break;
		}
	}
}

int main()
{
	std::unique_ptr<Socket> server_socket = std::make_unique<Socket>();
	std::unique_ptr<InetAddress> server_address = std::make_unique<InetAddress>();
	std::unique_ptr<Epoll> epoll = std::make_unique<Epoll>();

	server_socket->bind_socket(*server_address);
	server_socket->listen_socket();
	std::cout << "server start..." << std::endl;
	server_socket->set_socket_non_blocking();

	epoll->add_event(server_socket->get_socket_fd(), EPOLLIN | EPOLLET);

	while (true) {
		std::vector<epoll_event> events = epoll->poll();
		for (auto event : events) {
			if (event.data.fd == server_socket->get_socket_fd()) {
				while(true) {
					InetAddress client_address;
					int client_socket_fd = server_socket->accept_client_socket(client_address);

					if (client_socket_fd == -1) {
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							break;
						else {
							std::cerr << "accept error occur, statu is " << errno << std::endl;
							break;
						}
					}

					// TBD : Socket::set_socket_non_blocking static化?
					error_if(fcntl(client_socket_fd, F_SETFL, fcntl(client_socket_fd, F_GETFL) | O_NONBLOCK) == -1, "failed to set socket to non-blocking state");
					epoll->add_event(client_socket_fd, EPOLLIN | EPOLLET);
				}
			}
			else if (event.events & EPOLLIN) {
				handle_read_event(epoll.get(), event.data.fd);
			}
			else {
				std::cout << "other events will be implemented in subsequent versions";
				continue;
			}
		}
	}
	return EXIT_SUCCESS;
}