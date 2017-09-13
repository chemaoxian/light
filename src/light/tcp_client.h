#ifndef _LIGHT_TCP_CLIENT_H_
#define _LIGHT_TCP_CLIENT_H_

#include <light/forward.hpp>

namespace light {

	class TcpClient : public boost::noncopyable {
	public:
		TcpClient(EventLoopPtr& loop, const std::string& name = "");

		~TcpClient();

		// NOT Thread safe call before start
		void setCodecHandler(const CodecHandler& handler) {_codecHandler = handler;}

		// NOT Thread safe call before start
		void setConnectionHandler(const ConnectionHandler& handler) {_connectionHandler = handler;}

		// NOT Thread safe call before start
		void setMessageHandler(const MessageHandler& handler) { _messageHandler = handler;}

		bool start(const std::string& host, const Duration& connect_duration, bool auto_connect = true);

		EventLoopPtr getEventLooper() { return _loop; }

	private:
		EventLoopPtr _loop;
		CodecHandler _codecHandler;
		ConnectionHandler _connectionHandler;
		MessageHandler _messageHandler;
	};


}


#endif