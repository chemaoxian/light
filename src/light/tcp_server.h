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

	void setCodecHandler(const CodecHandler& handler) {_codecHandler = handler;}

	// NOT Thread safe call befor start
	void setConnectionHandler(const ConnectionHandler& handler) {_connection_handler = handler;}

	// NOT Thread safe call befor start
	void setMessageHandler(const MessageHandler& handler) { _message_handler = handler;}

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

	ConnectionHandler _connection_handler;
	MessageHandler _message_handler;
	CodecHandler _codecHandler;
};


}

#endif