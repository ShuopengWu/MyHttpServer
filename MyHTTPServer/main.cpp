#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <unistd.h>

#include "util.h"

int main()
{
	static constexpr ushort PORT = 8080;
	static constexpr int MAX_CONNECTS = 10;
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

	error_if(listen(server_socket, MAX_CONNECTS) < 0, "socket listen failed!");

	int client_socket = -1;
	sockaddr_in client_sockaddr_in;
	auto client_sockaddr_in_len = sizeof(client_sockaddr_in);

	std::cout << "server start success, socket : " << server_socket << " port : " << 
		ntohs(server_sockaddr_in.sin_port) << std::endl;

	while (true) {
		memset(&client_sockaddr_in, 0, sizeof(client_sockaddr_in));
		socklen_t client_sockaddr_in_len = sizeof(client_sockaddr_in);

		client_socket = accept(server_socket, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);

		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_sockaddr_in.sin_addr, client_ip, INET_ADDRSTRLEN);
		std::cout << "new client connect : " << client_socket << " ip: " << client_ip
			<< " port : " << ntohs(client_sockaddr_in.sin_port);
		if (client_socket == -1) {
			std::cerr << "client socket error!" << std::endl;
			continue;
		}

		char recv_buffer[MAX_BUFFER_SIZE];

		recv(client_socket, &recv_buffer, MAX_BUFFER_SIZE, 0);

		std::cout << "recive" << " " << client_socket << " message:" << std::endl;
		std::cout << recv_buffer << std::endl;

		if (recv_buffer[0] != '0') {
			std::string send_buffer = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
			auto send_size = send(client_socket, send_buffer.c_str(), send_buffer.size(), 0);

			std::cout << "send to" << " " << client_socket << " message:" << std::endl;
			std::cout << "send size = " << send_size << std::endl;
			std::cout << send_buffer.substr(0, send_size);
		}

		close(client_socket);
	};
}