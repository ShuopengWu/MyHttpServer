add_library(my_http_server_src STATIC
				"Channel.cpp"
				"Epoll.cpp"
				"InetAddress.cpp"
				"Socket.cpp"
				"util.cpp"
				"EventLoop.cpp"
				"Server.cpp")

target_include_directories(my_http_server_src PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})