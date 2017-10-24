#include <iostream>
#include <event2/event.h>
#include <event2/thread.h>
#include <light/tcp_connection.h>
#include <light/inner_log.h>
#include <signal.h>
#include <light/signal_event.hpp>
#include <light/light.h>

class EchoServer : boost::noncopyable
{
public:
	EchoServer(light::EventLoopPtr looper)
		:_looper(looper),
		 _server(looper, "echo server"){
		_count.store(0);
		curent_size.store(0);
		request_count.store(0);
	}

	~EchoServer() {

	}

	bool start (const std::string& host, int ioPoolCount) {

		_server.setCodecHandler(light::codec::DefaultStringCoderHandler());
		_server.setMessageHandler(boost::bind(&EchoServer::handlerMessage, this, _1, _2));
		_server.setConnectionHandler(boost::bind(&EchoServer::handleConnect, this, _1));

		_looper->runEvery(light::Duration(1.0), boost::bind(&EchoServer::statics, this));

		return _server.start(host, ioPoolCount, 100000);
	}

    void statics() {
	    LOG4CPLUS_INFO(light::LIGHT_LOGGER, "current count " << _count.load()
				<< " current bytes : " << curent_size.load() / 1024.0f  / 1024.0f << "M"
				<< " qps : " << request_count);

		curent_size.store(0);
		request_count.store(0);
    }

	void handlerMessage(light::TcpConnectionPtr& connPtr, const light::BufferPtr& buffer) {
		//printf("%s\n", buffer->data());

		connPtr->send(buffer->data());
		connPtr->send("\n");

		curent_size.fetch_add(buffer->size());
		request_count.fetch_add(1);
	}

	void handleConnect(light::TcpConnectionPtr& connPtr) {
		//printf("new connect : %s === state : %s\n", connPtr->getName().c_str(), connPtr->getStatusString());
		if (connPtr->getStatus() == light::TcpConnection::kConnected) {
			_count.fetch_add(1);
		}
		else {
			_count.fetch_sub(1);
		}
	}
private:
	light::EventLoopPtr _looper;
	light::TcpServer _server;
	boost::atomic<int> _count;
	boost::atomic<uint64_t> curent_size;
	boost::atomic<int> request_count;
};

void stopService(light::EventLoopPtr looper) {
	looper->stop();
}


int main(int argc, const char* argv[]) {

	if (argc != 2) {
		printf("usage : prog host\n");
		return -1;
	}

	WSADATA data;
	if (WSAStartup(MAKEWORD(2,2), &data) != 0){
		return -1;
	}

	light::initilize("");
	

	light::EventLoopPtr loop = boost::make_shared<light::EventLoop>("EchoMain");

	light::SignalEventPtr sig = light::SignalEvent::create(loop, SIGINT, boost::bind(&stopService, loop), true);

	EchoServer server(loop);

	server.start(argv[1], 10);

	loop->run();

    return 0;
}
