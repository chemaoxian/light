#include <light/tcp_connection.h>
#include <light/event_loop.h>
#include <light/log4cplus_forward.h>

namespace light {

	TcpConnection::TcpConnection(EventLoopPtr looper, std::string& name, evutil_socket_t fd,  const struct sockaddr& peer, const struct sockaddr& local)
		:_looper(looper),
		 _name(name),
		 _peer(peer),
		 _local(local),
		 _bufferEvent(bufferevent_socket_new(looper->getEventBase(), fd, 
											 BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE | BEV_OPT_DEFER_CALLBACKS)),
		 _status(kConnecting),
		 _closeMode(kNone) {

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
		LOG4CPLUS_ASSERT(glog, getStatus() == kConnecting);
		_msgHandler = handler;
	}

	void TcpConnection::setConnectionHandler(const ConnectionHandler& handler) {
		LOG4CPLUS_ASSERT(glog, getStatus() == kConnecting);
		_connectionHandler = handler;
	}

	void TcpConnection::setCloseHandler(const ConnectionHandler& handler) {
		LOG4CPLUS_ASSERT(glog, getStatus() == kConnecting);
		_closeHandler = handler;
	}

	bool TcpConnection::start() {
		
		Status expectStatus = kConnecting;
		if (_status.compare_exchange_strong(expectStatus, kConnected)) {
			
			int ret = bufferevent_enable(_bufferEvent, EV_READ|EV_WRITE);
			if (ret != 0) {
				LOG4CPLUS_ERROR(glog, "bufferevent_enable for " << _name << " ret " << ret);
				return false;
			}
		}

		return true;
	}

	bool TcpConnection::send(void* buffer, int len) {
		if (getStatus() != kConnected) {
			LOG4CPLUS_ERROR(glog, "send for " << _name << " failed, invalid status " << getStatus());
			return false;
		} else {
			return bufferevent_write(_bufferEvent, buffer, len) == 0;
		}
	}

	bool TcpConnection::send(const Slice& slice) {
		return send(const_cast<char*>(slice.data()), slice.size());
	}

	bool TcpConnection::send(const Buffer& buffer) {
		return send(const_cast<char*>(buffer.data()), buffer.length());
	}

	void TcpConnection::close() {
		_looper->runInLoop(boost::bind(&TcpConnection::_handleClose, shared_from_this(), kCloseActive));
	}

	void TcpConnection::closeWithDuration(const Duration& d) {
		_looper->runAfter(d, boost::bind(&TcpConnection::_handleClose, shared_from_this(), kCloseActive));
	}

	void TcpConnection::_handleClose(CloseMode mode) {
		evutil_socket_t connectFd = bufferevent_getfd(_bufferEvent);
		if (connectFd == -1) {
			return ;
		} else {
			bufferevent_setfd(_bufferEvent, -1);
		}
		
		if (connectFd) {
			evutil_closesocket(connectFd);

			_closeMode = mode;

			if (_connectionHandler) {
				_looper->runInLoop(boost::bind(_connectionHandler, shared_from_this()));
			}

			if (_closeHandler) {
				_looper->runInLoop(boost::bind(_closeHandler, shared_from_this()));
			}
		}
	}

	TcpConnection::Status TcpConnection::getStatus() {
		return _status.load();
	}

	const char* TcpConnection::getStatusString() {
		switch(_status.load()) {
		case kConnecting:
			return "kConnecting";
		case kConnected:
			return "kConnected";
		case kDisconnecting:
			return "kDisconnecting";
		case kDisconnected:
			return "kDisconnected";
		default:
			return "unkown state";
		}
	}

	void TcpConnection::_handleRead() {
		evbuffer* inputBuffer = bufferevent_get_input(_bufferEvent);
		if (_msgHandler) {
			_msgHandler(shared_from_this(), inputBuffer);
		}
	}

	void TcpConnection::_handleWrite() {
		// nothing todo noo
	}

	void TcpConnection::_handleEvent(short what) {
		if (what | BEV_EVENT_EOF) {
			_handleClose(kCloseByPeer);
		} else if (what | BEV_EVENT_ERROR ||
			       what | BEV_EVENT_TIMEOUT) {
			_handleClose(kCloseWithError);
		} else {
			LOG4CPLUS_WARN(glog, "unhandled event : " << what << " name : " << _name);
		}
	}

}