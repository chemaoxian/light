#include <light/tcp_listener.h>
#include <light/event_loop.h>
#include <light/log4cplus_forward.h>

namespace light {

TcpListener::TcpListener(EventLoopPtr loop, const std::string& name)
	:_loop(loop),
	 _name(name),
     _listener(NULL),
	 _started(false) {


}

TcpListener::~TcpListener() {
	if (_started) {
		if (_listener != NULL) {
			evconnlistener_free(_listener);
		}
	}
}

bool TcpListener::start(const std::string& listenAddr) {

	BOOST_ASSERT(_started == false);

	_started = true;
	sockaddr addr = {0};
	int addrLen = sizeof(sockaddr);
	int ret = evutil_parse_sockaddr_port(listenAddr.c_str(), &addr, &addrLen);
	if (ret != 0) {
		LOG4CPLUS_ERROR(light_logger, "failed to listen addr " << listenAddr
			<< ", ret = " << ret);

		_started = false;
		return false;
	}

	_listener = evconnlistener_new_bind(_loop->getEventBase(),
		&TcpListener::_eventCallback,
		this,
		LEV_OPT_CLOSE_ON_FREE|LEV_OPT_CLOSE_ON_EXEC|LEV_OPT_REUSEABLE|LEV_OPT_THREADSAFE,
		-1,
		&addr,
		addrLen);

	if (_listener == NULL) {
		_started = false;
		return false;
	}

	evconnlistener_set_error_cb(_listener, &TcpListener::_eventError);

	return true;
}

bool TcpListener::isListenning()
{
	return _started;
}

void TcpListener::_handleRead(struct evconnlistener * l, evutil_socket_t fd, struct sockaddr * addr, int socklen) {
	if (_newConnectHandler) {
		_newConnectHandler(fd, addr, socklen);
	} else {
		evutil_closesocket(fd);
	}
}

void TcpListener::_handleError(struct evconnlistener *) {

	LOG4CPLUS_ERROR(light_logger, "TcpListener [" << _name <<  "] occur a errror");

	if (_errorHandler) {
		_errorHandler();
	}
}

void TcpListener::_eventCallback(struct evconnlistener * l, evutil_socket_t fd, struct sockaddr * addr, int socklen, void * that) {
	TcpListener* listener = static_cast<TcpListener*>(that);
	listener->_handleRead(l, fd, addr, socklen);
}

void TcpListener::_eventError(struct evconnlistener * l, void * that) {
	TcpListener* listener = static_cast<TcpListener*>(that);
	listener->_handleError(l);
}

}

