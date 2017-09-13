#include <light/tcp_connector.h>
#include <light/event_loop.h>
#include <light/log4cplus_forward.h>

namespace light {
    

	TcpConnector::TcpConnector(EventLoopPtr& loop, const std::string& name)
		:_loop(loop),
		 _name(name),
		 _is_running(false),
		 _bufferEvent(NULL){

	}

	TcpConnector::~TcpConnector() {

	}

	
	bool TcpConnector::start(const std::string& host, bool auto_connect) {

		sockaddr addr = {0};
		int addr_size = 0;
		if (evutil_parse_sockaddr_port(host.c_str(), &addr, &addr_size) < 0)
		{
			return false;
		}

		bool expect = false;
		if (_is_running.compare_exchange_strong(expect, true)) {
			
			_bufferEvent = bufferevent_socket_new(_loop->getEventBase(), -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
			if (_bufferEvent == NULL) {
				_is_running.store(false);
				return false;
			}

			bufferevent_setcb(_bufferEvent, 
				NULL, 
				NULL,
				&TcpConnector::_eventCallback,
				this);

			_addr = addr;
			_addrSize = addr_size;
		}	

		return true;
	}

	void TcpConnector::stop() {
		bool expect = true;
		if (_is_running.compare_exchange_strong(expect, false)) {
			bufferevent_setcb(_bufferEvent, NULL, NULL, NULL, this);
			bufferevent_free(_bufferEvent);
		}
	}

	void TcpConnector::_eventCallback(struct bufferevent *bev, short what, void *ctx)
	{
		TcpConnector* connctorPtr = static_cast<TcpConnector*>(ctx);
		connctorPtr->_handleEventCallabck(what);
	}

	void TcpConnector::_handleEventCallabck(short what)
	{
		if (what | BEV_EVENT_ERROR || what | BEV_EVENT_TIMEOUT) {
			_error_handler();
		} else if (what | BEV_EVENT_CONNECTED) {
			
			//_connection_handler();
		} else {
			LOG4CPLUS_WARN(glog, "unhandled event : " << what << " name : " << _name);
		}
	}

}