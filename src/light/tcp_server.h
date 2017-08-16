#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <stdint.h>
#include <string>
#include <light/forward.hpp>


namespace light {

class TcpServer : public boost::noncopyable {
public:
    TcpServer(EventLoopPtr& loop);
    
	~TcpServer();

	// NOT Thread safe call befor start
	void setConnectionHandler(const ConnectionHandler& handler);

	// NOT Thread safe call befor start
	void setMessageHandler(const MessageHandler& handler);

	// Thread safe
    bool start(const std::string& host, uint16_t port, int threadCount);
	
	EventLoopPtr getEventLooper() { return _loop; }

	EventLoopThreadPoolPtr getEventLoopThreadPool(){return _threadPool;}

private:
	typedef boost::unordered_map<std::string, TcpconnectionPtr> ConnectionMap;
	
private:
	EventLoopPtr _loop;
	EventLoopThreadPoolPtr _threadPool;
	ConnectionMap _connections;
};


}

#endif