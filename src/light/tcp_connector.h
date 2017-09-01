#ifndef _TCP_CONNECTOR_H_
#define _TCP_CONNECTOR_H_

#include <light/forward.hpp>

namespace light {

class TcpConnector : public boost::noncopyable {
public:
    TcpConnector(EventLoopPtr& loop, const std::string& name = "");
    
	~TcpConnector();

	// NOT Thread safe call befor start
	void setNewConnectionHandler(const ConnectionHandler& handler) {_connection_handler = handler;}

	void setErrorHandler(const Handler& handler);

	// Thread safe
    bool start(const std::string& host, bool auto_connect);

	EventLoopPtr getEventLooper() { return _loop; }

private:


private:
	boost::atomic<bool> _is_running;
	EventLoopPtr _loop;
	std::string	_name;
	ConnectionHandler _connection_handler;
}

#endif