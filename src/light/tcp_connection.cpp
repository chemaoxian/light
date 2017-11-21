#include <light/tcp_connection.h>
#include <light/event_loop.h>
#include <light/inner_log.h>

namespace light {

	TcpConnection::TcpConnection(EventLoopPtr looper, const std::string& name, evutil_socket_t fd,  const struct sockaddr& peer)
		:_looper(looper),
		 _name(name),
		 _peer(peer),
		 _bufferEvent(bufferevent_socket_new(looper->getEventBase(), fd,
											 BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE)),
		 _status(kConnecting),
		 _closeMode(kNone),
		 _messageBuffer(new Buffer(Buffer::kInitialSize, 0)){

		bufferevent_setcb(_bufferEvent,
			&TcpConnection::_readCallback,
			&TcpConnection::_writeCallabck,
			&TcpConnection::_eventCallback,
			this);
	}

	TcpConnection::TcpConnection(EventLoopPtr looper, const std::string& name, bufferevent* buffer, const struct sockaddr& peer)
		:_looper(looper),
		_name(name),
		_peer(peer),
		_bufferEvent(buffer),
		_status(kConnecting),
		_closeMode(kNone),
		_messageBuffer(new Buffer(Buffer::kInitialSize, 0)){

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
		BOOST_ASSERT(getStatus() == kConnecting);
		_msgHandler = handler;
	}

	void TcpConnection::setConnectionHandler(const ConnectionHandler& handler) {
		BOOST_ASSERT(getStatus() == kConnecting);
		_connectionHandler = handler;
	}

	void TcpConnection::setCloseHandler(const ConnectionHandler& handler) {
		BOOST_ASSERT(getStatus() == kConnecting);
		_closeHandler = handler;
	}

	bool TcpConnection::start() {

		Status expectStatus = kConnecting;
		if (_status.compare_exchange_strong(expectStatus, kConnected)) {

			int ret = bufferevent_enable(_bufferEvent, EV_READ|EV_WRITE);
			if (ret != 0) {
				LOG4CPLUS_ERROR(LIGHT_LOGGER, "bufferevent_enable for " << _name << " ret " << ret);
				return false;
			}

			_status.store(kConnected);

			_looper->runInLoop(boost::bind(_connectionHandler, shared_from_this()));
		}

		return true;
	}

	bool TcpConnection::send(void* buffer, int len) {
		if (getStatus() != kConnected) {
			LOG4CPLUS_ERROR(LIGHT_LOGGER, "send for " << _name << " failed, invalid status " << getStatus());
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
		if (getStatus() != kDisconnected && getStatus() != kConnecting) {

			_status.store(kDisconnecting);
			_looper->runInQueue(boost::bind(&TcpConnection::_handleClose, shared_from_this(), kCloseActive));
		}
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

			_status.store(kDisconnected);

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

			TcpConnectionPtr connPtr = shared_from_this();

			while (true) {
				codec::CodecStatus status = _codec_hander(inputBuffer, _messageBuffer);

				if (status == codec::kComplete)
				{
					_msgHandler(connPtr, _messageBuffer);

					_messageBuffer->Reset();
				}
				else
				{
					break;
				}
			}
		}
	}

	void TcpConnection::_handleWrite() {
		// nothing todo noo
	}

	void TcpConnection::_handleEvent(short what) {
		if (what & BEV_EVENT_EOF) {
			_handleClose(kCloseByPeer);
		} else if (what & BEV_EVENT_ERROR ||
			       what & BEV_EVENT_TIMEOUT) {
			_handleClose(kCloseWithError);
		} else {
			LOG4CPLUS_WARN(LIGHT_LOGGER, "unhandled event : " << what << " name : " << _name);
		}
	}

	void TcpConnection::_readCallback(struct bufferevent *bev, void *ctx)
	{
		TcpConnection* connection_ptr = static_cast<TcpConnection*>(ctx);
		connection_ptr->_handleRead();
	}

	void TcpConnection::_writeCallabck(struct bufferevent *bev, void *ctx)
	{
		TcpConnection* connection_ptr = static_cast<TcpConnection*>(ctx);
		connection_ptr->_handleWrite();
	}

	void TcpConnection::_eventCallback(struct bufferevent *bev, short what, void *ctx)
	{
		TcpConnection* connection_ptr = static_cast<TcpConnection*>(ctx);
		connection_ptr->_handleEvent(what);
	}

	std::string TcpConnection::getPeerIp()
	{
		sockaddr_in* peerAddrIn = reinterpret_cast<sockaddr_in*>(&_peer);
        char addrBuf[32] = {0};
		return evutil_inet_ntop(AF_INET, &(peerAddrIn->sin_addr), addrBuf, sizeof(addrBuf));
	}

	uint16_t TcpConnection::getPeerPort()
	{
		sockaddr_in* peerAddrIn = reinterpret_cast<sockaddr_in*>(&_peer);
		return ntohs(peerAddrIn->sin_port);
	}

}
