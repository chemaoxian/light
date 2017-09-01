#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <stdint.h>
#include <string>
#include <light/forward.hpp>
#include <light/tcp_listener.h>
#include <light/codec.h>

namespace light {

class TcpServer : public boost::noncopyable {
public:
    TcpServer(EventLoopPtr& loop, const std::string& name = "");
    
	~TcpServer();

	void setCodecHandler(const CodecHandler& handler) {_codecHandler = handler;}

	// NOT Thread safe call befor start
	void setConnectionHandler(const ConnectionHandler& handler) {_connection_handler = handler;}

	// NOT Thread safe call befor start
	void setMessageHandler(const MessageHandler& handler) { _message_handler = handler;}

	// Thread safe
    bool start(const std::string& host, int threadCount, int max_connection=50000);

	EventLoopPtr getEventLooper() { return _loop; }

	EventLoopThreadPoolPtr getEventLoopThreadPool(){return _threadPool;}

private:
	typedef boost::unordered_map<std::string, TcpconnectionPtr> ConnectionMap;
	
private:
	void _new_connection_handler(evutil_socket_t , struct sockaddr* , int socklen);
	void _close_connection_handler(TcpconnectionPtr& conn);
	void _close_connection_handler_in_loop(TcpconnectionPtr& conn);
	void _listener_error_handler();
	void _default_connection_handler(TcpconnectionPtr& conn);
	void _default_message_handler(TcpconnectionPtr& buffer, const BufferPtr&);

private:
	boost::atomic<bool> _is_running;
	TcpListener _listener;
	EventLoopPtr _loop;
	EventLoopThreadPoolPtr _threadPool;
	uint32_t _max_connection;
	uint32_t _next_connection_id;
	ConnectionMap _connections;
	std::string	_name;

	ConnectionHandler _connection_handler;
	MessageHandler _message_handler;
	CodecHandler _codecHandler;
};


}

#endif