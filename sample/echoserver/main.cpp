#include <iostream>
#include <light/tcp_server.h>
#include <light/tcp_client.h>
#include <light/event_loop.h>
#include <event2/event.h>
#include <log4cplus/config.hxx>
#include <event2/thread.h>

int main() {

	WSADATA data;
	if (WSAStartup(MAKEWORD(2,2), &data) != 0) {

	}
	
	evthread_use_windows_threads();

	log4cplus::initialize();

	light::EventLoopPtr loop = boost::make_shared<light::EventLoop>("TEST");

	light::TcpServer server(loop);

	server.start("127.0.0.1:8080", 4);

	loop->loop();

    return 0;
}