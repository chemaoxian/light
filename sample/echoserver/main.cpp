#include <iostream>
#include <light/tcp_server.h>
#include <light/tcp_client.h>
#include <light/event_loop.h>

int main() {

	light::EventLoopPtr loop = boost::make_shared<light::EventLoop>("TEST");

	light::TcpServer server(loop);



    return 0;
}