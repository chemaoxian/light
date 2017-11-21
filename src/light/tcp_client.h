#ifndef _LIGHT_TCP_CLIENT_H_
#define _LIGHT_TCP_CLIENT_H_

#include <light/forward.hpp>
#include <light/codec.h>

namespace light {

	class TcpConnector;

	class TcpClient : public boost::enable_shared_from_this<TcpClient> {
	public:
		TcpClient(const EventLoopPtr& loop, const std::string& name = "");

		~TcpClient();

		// NOT Thread safe call before start
		void setCodecHandler(const CodecHandler& handler) {_codecHandler = handler;}

		// NOT Thread safe call before start
		void setConnectionHandler(const ConnectionHandler& handler) {_connectionHandler = handler;}

		// NOT Thread safe call before start
		void setMessageHandler(const MessageHandler& handler) { _messageHandler = handler;}

		// start and stop can not call in concurrence
		bool start(const std::string& host, const Duration& connect_duration, bool auto_connect = true);

		void stop();

		EventLoopPtr getEventLooper() { return _loop; }

		bool isRunning() {return _runing.load();}

		TcpConnectionPtr getConnection();
	private:
		void handleNewConnection(TcpConnectionPtr conn);
		void handleCloseConnection(TcpConnectionPtr conn);
		void handleConnectError();

		void startInLoop(const std::string& host, const Duration& connect_duration, bool auto_connect);
		void stopInLoop();
		void restartInLoop();
	private:
		boost::atomic<bool> _runing;
		EventLoopPtr _loop;
		CodecHandler _codecHandler;
		ConnectionHandler _connectionHandler;
		MessageHandler _messageHandler;
		boost::scoped_ptr<TcpConnector> _connector;
		bool _autoConnector;
		Duration _connectInterval;
		boost::mutex _connectionLock;
		TimerEventPtr	_timerPtr;
		TcpConnectionPtr _connection;
	};


}


#endif
