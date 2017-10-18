#include <iostream>
#include <light/forward.hpp>
#include <light/tcp_server.h>
#include <light/tcp_client.h>
#include <light/event_loop.h>
#include <event2/event.h>
#include <log4cplus/config.hxx>
#include <event2/thread.h>
#include <light/tcp_connection.h>
#include <light/log4cplus_forward.h>
#ifdef WIN32
#include <log4cplus/win32consoleappender.h>
#else
#include <log4cplus/consoleappender.h>
#endif
#include <signal.h>
#include <light/signal_event.hpp>

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
	    LOG4CPLUS_INFO(light::light_logger, "current count " << _count.load()
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
#ifdef WIN32
	WSADATA data;
	if (WSAStartup(MAKEWORD(2,2), &data) != 0) {
		printf("init WAS failed\n");
	}

	log4cplus::initialize();

	log4cplus::SharedAppenderPtr appender(new log4cplus::Win32ConsoleAppender());
	std::auto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout("%D{[%y%m%d %H:%M:%S]} [%l] %-5p %c{2} %%%x%% - %m %n"));

	appender->setLayout(layout);
	light::light_logger.addAppender(appender);

	evthread_use_windows_threads();
#else
	log4cplus::initialize();

	log4cplus::SharedAppenderPtr appender(new log4cplus::ConsoleAppender());
	std::auto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout("%D{[%y%m%d %H:%M:%S]} [%l] %-5p %c{2} %%%x%% - %m %n"));

	appender->setLayout(layout);
	light::light_logger.addAppender(appender);

    evthread_use_pthreads();
#endif

	//evthread_enable_lock_debuging();
	//event_enable_debug_mode();

	light::EventLoopPtr loop = boost::make_shared<light::EventLoop>("EchoMain");

	light::SignalEventPtr sig = light::SignalEvent::create(loop, SIGINT, boost::bind(&stopService, loop), true);

	EchoServer server(loop);

	server.start(argv[1], 10);

	loop->run();

    return 0;
}
