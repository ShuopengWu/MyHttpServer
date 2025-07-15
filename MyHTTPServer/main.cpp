#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#include "util.h"

int main()
{
	static constexpr ushort PORT = 8080;
	static constexpr int MAX_BACKLOG = 128;
	static constexpr int MAX_EVENTS = 10;
	static constexpr int MAX_BUFFER_SIZE = 0x0400;

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	error_if(server_socket == -1, "socket create failed!");
	
	sockaddr_in server_sockaddr_in;
	memset(&server_sockaddr_in, 0, sizeof(server_sockaddr_in));
	server_sockaddr_in.sin_family = AF_INET;
	server_sockaddr_in.sin_port = htons(PORT);
	server_sockaddr_in.sin_addr.s_addr = INADDR_ANY;

	error_if(bind(server_socket, (sockaddr*)&server_sockaddr_in, sizeof(server_sockaddr_in)) == -1,
		"socket bind failed!");

	error_if(listen(server_socket, MAX_BACKLOG) < 0, "socket listen failed!");

	int epoll_fd = epoll_create1(0); //使用epoll_create1， 使用epoll_create需要保证size > 0
	error_if(epoll_fd == -1, "epoll create failed!");

	epoll_event events[MAX_EVENTS];

	auto make_event = [&](int fd) {
		epoll_event event;
		memset(&event, 0, sizeof(event));
		event.data.fd = fd;
		event.events = EPOLLIN | EPOLLET;
		error_if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1,
			"Failed to set socket to non-blocking state");
		error_if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1,
			"add epoll event failed");
	};

	make_event(server_socket);

	std::cout << "server start success, socket : " << server_socket << " port : " << 
		ntohs(server_sockaddr_in.sin_port) << std::endl;

	while (true) {
		int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		error_if(nfds == -1, "epoll waiting failed!");

		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == server_socket) {
				while (true) {
					int client_socket = -1;
					sockaddr_in client_sockaddr_in;
					memset(&client_sockaddr_in, 0, sizeof(client_sockaddr_in));
					socklen_t client_sockaddr_in_len = sizeof(client_sockaddr_in);

					client_socket = accept(server_socket, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
					if (client_socket < 0) {
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							break;
						else {
							std::cerr << "accept error occur, statu is " << errno << std::endl;
							break;
						}
					}

					char client_ip[INET_ADDRSTRLEN];
					if (inet_ntop(AF_INET, &client_sockaddr_in.sin_addr, client_ip, INET_ADDRSTRLEN) == nullptr)
						strcpy(client_ip, "N/A");
					std::cout << "new client connect : " << client_socket << " ip: " << client_ip
						<< " port : " << ntohs(client_sockaddr_in.sin_port) << std::endl;

				make_event(client_socket);
				}
			}
			else if (events[i].events & EPOLLIN) {
				char recv_buffer[MAX_BUFFER_SIZE];

				while (true) {
					memset(recv_buffer, 0, sizeof(recv_buffer));
					auto recv_size = recv(events[i].data.fd, recv_buffer, sizeof(recv_buffer), 0);

					if (recv_size > 0) {
						std::cout << "recive client " << events[i].data.fd << " :" << std::endl;
						std::cout << recv_buffer << std::endl;
						write(events[i].data.fd, recv_buffer, recv_size);
					}
					else if (recv_size == -1 && errno == EINTR) { //客户端中断,继续读取
						std::cout << "continue rerading" << std::endl;
						continue;
					}
					else if (recv_size == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { //数据读取完毕
						std::cout << "finish reading, errno: " << errno << std::endl;
						break;
					}
					else if (recv_size == 0) { //客户端退出
						std::cout << "client " << events[i].data.fd << " disconnected" << std::endl;
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
						close(events[i].data.fd);
						break;
					}
				}
			}
			else {
				std::cout << "other event" << std::endl;
			}
		}
	};

	close(epoll_fd);
	close(server_socket);
	return EXIT_SUCCESS;
}
