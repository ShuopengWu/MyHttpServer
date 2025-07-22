#include "src/Server.h"
#include "src/EventLoop.h"

int main()
{
	std::shared_ptr<EventLoop> event_loop = std::make_shared<EventLoop>();
	std::unique_ptr<Server> server = std::make_unique<Server>(event_loop.get());

	event_loop->loop();

	return EXIT_SUCCESS;
}