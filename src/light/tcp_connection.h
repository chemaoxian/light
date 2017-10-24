#ifndef _LIGHT_TCP_CONNECTION_H_
#define _LIGHT_TCP_CONNECTION_H_

#include <light/forward.hpp>
#include <light/duration.h>
#include <light/codec.h>

namespace light {

class TcpConnection 
	: public boost::enable_shared_from_this<TcpConnection> {

public:
	enum Status {
		kConnecting,
		kConnected,
		kDisconnecting,
		kDisconnected
	};

	enum CloseMode {
		kNone,
		kCloseByPeer,
		kCloseActive,
		kCloseWithError
	};

public:
	TcpConnection(EventLoopPtr looper, const std::string& name, evutil_socket_t fd,  const struct sockaddr& peer);
	
	TcpConnection(EventLoopPtr looper, const std::string& name, bufferevent* buffer, const struct sockaddr& peer);

	~TcpConnection();

	// thread safe
	bool send(void* buffer, int len);

	// thread safe
	bool send(const Slice& slice);

	// thread safe
	bool send(const Buffer& buffer);
	
	// call in start thread loop
	void close();

	// call in start thread loop
	void closeWithDuration(const Duration& d);

	
public:
	EventLoopPtr getLooper() {return _looper;}

	Status getStatus();

	const char* getStatusString();

	EventLoopPtr getEventLoop() {return _looper;}

	CloseMode getCloseMode() {return _closeMode;}

	boost::any& getContext() {return _context; }
	
	void setContext(const boost::any& context) {_context = context;}

	std::string getName() {return _name;}

public: // internal use, NOT thread safe functions, for init the tcp connnection 
	//NOT thead safe
	void setMessageHandler(const MessageHandler& handler);

	//NOT thead safe
	void setConnectionHandler(const ConnectionHandler& handler);

	//NOT thread safe
	void setCloseHandler(const ConnectionHandler& handler);

	void setCodecHandler(const CodecHandler& handler) {_codec_hander = handler;}

	// Thread Safe, call by TcpClient or TcpServer
	bool start();

private:
	void _handleRead();
	void _handleWrite();
	void _handleEvent(short what);
	void _handleClose(CloseMode mode);

	static void _readCallback(struct bufferevent *bev, void *ctx);
	static void _writeCallabck(struct bufferevent *bev, void *ctx);
	static void _eventCallback(struct bufferevent *bev, short what, void *ctx);
private:
	EventLoopPtr _looper;
	std::string _name;
	struct sockaddr _peer;
	struct bufferevent* _bufferEvent;
	MessageHandler _msgHandler;
	ConnectionHandler _connectionHandler;
	ConnectionHandler _closeHandler;
	CodecHandler _codec_hander;
	boost::atomic<Status> _status;
	CloseMode	_closeMode;
	boost::any  _context;
	BufferPtr _messageBuffer;
	
};

}

#endif