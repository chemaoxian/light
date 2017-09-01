#ifndef _LIGHT_TCP_CLIENT_H_
#define _LIGHT_TCP_CLIENT_H_

#include <light/forward.hpp>

namespace light {

	class TcpClient : public boost::noncopyable {
	public:
		TcpClient(EventLoopPtr& loop, const std::string& name = "");

		~TcpClient();

		void setCodecHandler(const CodecHandler& handler) {_codecHandler = handler;}

		// NOT Thread safe call befor start
		void setConnectionHandler(const ConnectionHandler& handler) {_connectionHandler = handler;}

		// NOT Thread safe call befor start
		void setMessageHandler(const MessageHandler& handler) { _messageHandler = handler;}

		bool start(const std::string& host, bool auto_connect = true, const Duration& connect_duration);

		EventLoopPtr getEventLooper() { return _loop; }

	private:
		EventLoopPtr _loop;
		CodecHandler _codecHandler;
		ConnectionHandler _connectionHandler;
		MessageHandler _messageHandler;
	};


}


#endif