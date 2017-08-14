#include <light/tcp_connection.h>
#include <light/event_loop.h>

namespace light {

	TcpConnection::TcpConnection(EventLoopPtr looper, std::string& name, evutil_socket_t fd,  const struct sockaddr& peer, const struct sockaddr& local)
		:_looper(looper),
		 _name(name),
		 _fd(fd),
		 _peer(peer),
		 _local(local),
		 _bufferEvent(bufferevent_socket_new(looper->getEventBase(), _fd, 
											 BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE | BEV_OPT_DEFER_CALLBACKS)) {
		bufferevent_setcb(_bufferEvent, 
			&TcpConnection::_readCallback, 
			&TcpConnection::_writeCallabck,
			&TcpConnection::_eventCallback,
			this);
	}

	TcpConnection::~TcpConnection() {
		bufferevent_free(_bufferEvent);
	}

	void TcpConnection::setMessageHandler(const MessageHandler& handler) {
		bufferevent_lock(_bufferEvent);
		_msgHandler = handler;
		bufferevent_unlock(_bufferEvent);
	}

	void TcpConnection::setConnectionHandler(const ConnectionHandler& handler) {
		bufferevent_lock(_bufferEvent);
		_connectionHandler = handler;
		bufferevent_unlock(_bufferEvent);
	}

	void TcpConnection::setCloseHandler(const ConnectionHandler& handler) {
		bufferevent_lock(_bufferEvent);
		_closeHandler = handler;
		bufferevent_unlock(_bufferEvent);
	}

	bool TcpConnection::send(void* buffer, int len) {
		return bufferevent_write(_bufferEvent, buffer, len) == 0;
	}

	bool TcpConnection::send(const Slice& slice) {
		return send(const_cast<char*>(slice.data()), slice.size());
	}

	bool TcpConnection::send(const Buffer& buffer) {
		return send(const_cast<char*>(buffer.data()), buffer.length());
	}

	void TcpConnection::shutdown() {
		bufferevent_setfd(_bufferEvent, -1);
		evutil_closesocket(_fd);
	}

	void TcpConnection::close() {

	}

	void TcpConnection::closeWithDuration(const Duration& d) {

	}

}