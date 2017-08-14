#ifndef _LIGHT_TCP_LISTENER_H_
#define _LIGHT_TCP_LISTENER_H_

#include <light/forward.hpp>

namespace light {

class TcpListener 
	: public boost::noncopyable {
public:
	typedef boost::function <void(evutil_socket_t , struct sockaddr* , int socklen)> NewConnectionHandler;
public:
	TcpListener(EventLoopPtr loopPtr, const std::string& name);
	~TcpListener();

	// Must call before start
	void setNewConnectionHandler(const NewConnectionHandler& handler) { _newConnectHandler = handler;}

	// Must call befor start
	void setErrorHandler(const Handler& handler) { _errorHandler = handler; }

	// NOT thead safe
	bool start(const std::string& listenAddr);

	bool isListenning();

private:
	void _handleRead(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen);
	
	void _handleError(struct evconnlistener *);

	static void _eventCallback(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);
	
	static void _eventError(struct evconnlistener *, void *);
private:
	bool _started;
	std::string _name;
	EventLoopPtr _loop;
	std::string _lisenerAddr;
	struct evconnlistener* _listener;
	NewConnectionHandler _newConnectHandler;
	Handler	_errorHandler;
};

}

#endif