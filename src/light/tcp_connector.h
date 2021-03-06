#ifndef _TCP_CONNECTOR_H_
#define _TCP_CONNECTOR_H_

#include <light/forward.hpp>

namespace light {

class TcpConnector : public boost::noncopyable {
public:
    TcpConnector(EventLoopPtr& loop, const std::string& name = "");
    
	~TcpConnector();

	// NOT Thread safe call before start
	void setNewConnectionHandler(const ConnectionHandler& handler) {_connection_handler = handler;}

	// NOT Thread safe call before start
	void setErrorHandler(const Handler& handler) {_error_handler = handler;}

	// Thread safe
	EventLoopPtr getEventLooper() { return _loop; }

	// NO Thread safe
	bool start(const std::string& host);

	// NO Thread safe
	void stop();

	// NOT thread safe
	bool restart();
	
private:
	static void _eventCallback(struct bufferevent *bev, short what, void *ctx);

	void _handleEventCallabck(short what);
private:
	bool _is_runing;
	EventLoopPtr _loop;
	std::string	_name;
	ConnectionHandler _connection_handler;
	Handler _error_handler;
	bufferevent* _bufferEvent;
	sockaddr _addr;
	int		_addrSize;
};

}
#endif