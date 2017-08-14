#ifndef _LIGHT_TCP_CONNECTION_H_
#define _LIGHT_TCP_CONNECTION_H_

#include <light/forward.hpp>
#include <light/duration.h>

namespace light {

class TcpConnection 
	: public boost::enable_shared_from_this<TcpConnection> {

public:
	TcpConnection(EventLoopPtr looper, std::string& name, evutil_socket_t fd,  const struct sockaddr& peer, const struct sockaddr& local);
	
	~TcpConnection();

	// thead safe
	void setMessageHandler(const MessageHandler& handler);
	
	// thead safe
	void setConnectionHandler(const ConnectionHandler& handler);

	// thread safe
	void setCloseHandler(const ConnectionHandler& handler);

	// thread safe
	bool send(void* buffer, int len);

	// thread safe
	bool send(const Slice& slice);

	// thread safe
	bool send(const Buffer& buffer);
	
	// call in start thread loop
	void shutdown();
	
	// call in start thread loop
	void close();

	// call in start thread loop
	void closeWithDuration(const Duration& d);

	EventLoopPtr getLooper() {return _looper;}

private:
	void _handleRead();
	void _handleWrite();
	void _handleEvent(short what);

	static void _readCallback(struct bufferevent *bev, void *ctx);
	static void _writeCallabck(struct bufferevent *bev, void *ctx);
	static void _eventCallback(struct bufferevent *bev, short what, void *ctx);
private:
	EventLoopPtr _looper;
	std::string _name;
	evutil_socket_t _fd;
	struct sockaddr _peer;
	struct sockaddr _local;
	struct bufferevent* _bufferEvent;
	MessageHandler _msgHandler;
	ConnectionHandler _connectionHandler;
	ConnectionHandler _closeHandler;
	
};

}

#endif