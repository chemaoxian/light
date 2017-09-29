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

#include <light/timer_event.h>
#include <light/signal_event.hpp>
#include <signal.h>

class EchoClient : boost::noncopyable
{
public:
	EchoClient(light::EventLoopPtr looper, int id)
		:_looper(looper),
		 _client(looper, "echo client"),
		 _id(id){

	}

	~EchoClient() {

	}


	bool start (const std::string& host) {

		_client.setCodecHandler(light::codec::DefaultStringCoderHandler());
		_client.setMessageHandler(boost::bind(&EchoClient::handlerMessage, this, _1, _2));
		_client.setConnectionHandler(boost::bind(&EchoClient::handleConnect, this, _1));
		return _client.start(host, light::Duration(1.0));
	}

	void handlerMessage(light::TcpConnectionPtr& connPtr, const light::BufferPtr& buffer) {
		//printf("%s\n", buffer->data());

		//connPtr->send(buffer->data());
	}

	void handleConnect(light::TcpConnectionPtr& connPtr) {
		printf("new connect : %s === state : %s\n", connPtr->getName().c_str(), connPtr->getStatusString());

		if (connPtr->getStatus() == light::TcpConnection::kConnected)
		{
			_timer = _looper->runEvery(light::Duration(1.0), boost::bind(&EchoClient::sendMessage, this));
		}
		else
		{
			_timer->cancel();
		}
	}

	void sendMessage() {
		light::TcpConnectionPtr connection = _client.getConnection();
		if (connection) {

            const char* buffer = "123456789"
                "123456789123456789123456789123456789123456789"
                "123456789123456789123456789123456789123456789"
                "123456789123456789123456789123456789123456789"
                "123456789123456789123456789123456789123456789"
                "123456789123456789123456789123456789123456789"
                "123456789123456789123456789123456789123456789\n";

			if (!connection->send(buffer)) {
				printf("send message failed \n");
			}
		}
	}

private:
	light::EventLoopPtr _looper;
	light::TcpClient _client;
	light::TimerEventPtr _timer;
	int _id;
};

void stopService(light::EventLoopPtr looper) {
	looper->stop();
}

std::vector<boost::shared_ptr<EchoClient> > clients;
light::TimerEventPtr timer;
void create_client(light::EventLoopPtr looper, const char* host) {

	if (clients.size() > 10000)
	{
		timer->cancel();
		return ;
	}

	boost::shared_ptr<EchoClient> echo_client_ptr = boost::make_shared<EchoClient>(looper, clients.size());
	echo_client_ptr->start(host);

	clients.push_back(echo_client_ptr);
}



int main(int argc, const char* argv[]) {

	if (argc != 2) {
		printf("usage : prog host\n");
		return -1;
	}
#ifdef WIN32
    WSADATA data;
	if (WSAStartup(MAKEWORD(2,2), &data) != 0) {

	}

	log4cplus::initialize();

	log4cplus::SharedAppenderPtr appender(new log4cplus::Win32ConsoleAppender());
	std::auto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout("%D{[%y%m%d %H:%M:%S]} [%l] %-5p %c{2} %%%x%% - %m %n"));

	appender->setLayout(layout);
	light::light_logger.addAppender(appender);

	evthread_use_windows_threads();
	//evthread_enable_lock_debuging();
	//event_enable_debug_mode();
#else
    log4cplus::initialize();

	log4cplus::SharedAppenderPtr appender(new log4cplus::ConsoleAppender());
	std::auto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout("%D{[%y%m%d %H:%M:%S]} [%l] %-5p %c{2} %%%x%% - %m %n"));

	appender->setLayout(layout);
	light::light_logger.addAppender(appender);

	evthread_use_pthreads();
	//evthread_enable_lock_debuging();
	//event_enable_debug_mode();

#endif

	light::EventLoopPtr loop = boost::make_shared<light::EventLoop>("TEST");

	light::SignalEventPtr sig = light::SignalEvent::create(loop, SIGINT, boost::bind(&stopService, loop), true);


	timer = loop->runEvery(light::Duration(0.01), boost::bind(&create_client, loop, argv[1]));

	loop->run();

	clients.clear();
    return 0;
}
