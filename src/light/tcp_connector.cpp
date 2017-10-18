#include <light/tcp_connector.h>
#include <light/event_loop.h>
#include <light/log4cplus_forward.h>
#include <light/tcp_connection.h>

namespace light {
    

	TcpConnector::TcpConnector(EventLoopPtr& loop, const std::string& name)
		:_loop(loop),
		 _name(name),
		 _is_runing(false),
		 _bufferEvent(NULL){

	}

	TcpConnector::~TcpConnector() {
		stop();
	}

	
	bool TcpConnector::start(const std::string& host) {

		BOOST_ASSERT(_loop->isInLoopThread());

		sockaddr addr = {0};
		int addr_size = sizeof(addr);
		if (evutil_parse_sockaddr_port(host.c_str(), &addr, &addr_size) < 0)
		{
			return false;
		}

		if (!_is_runing) {
		
			_bufferEvent = bufferevent_socket_new(_loop->getEventBase(), -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
			if (_bufferEvent == NULL) {
				_is_runing = false;
				return false;
			}

			bufferevent_setcb(_bufferEvent, 
				NULL, 
				NULL,
				&TcpConnector::_eventCallback,
				this);

			if (bufferevent_socket_connect(_bufferEvent, &addr, addr_size) < 0) {
				_is_runing = false;
				bufferevent_free(_bufferEvent);
				_bufferEvent = NULL;
				return false;
			}

			_addr = addr;
			_addrSize = addr_size;
			_is_runing = true;
		}	

		return true;
	}

	void TcpConnector::stop() {

		BOOST_ASSERT(_loop->isInLoopThread());

		if (!_is_runing) {
			return ;
		}

		_is_runing = false;

		if (_bufferEvent != NULL) {
			bufferevent_setcb(_bufferEvent, NULL, NULL, NULL, this);
			bufferevent_free(_bufferEvent);

			_bufferEvent = NULL;
		}	
	}

	void TcpConnector::_eventCallback(struct bufferevent *bev, short what, void *ctx)
	{
		TcpConnector* connctorPtr = static_cast<TcpConnector*>(ctx);
		connctorPtr->_handleEventCallabck(what);
	}

	void TcpConnector::_handleEventCallabck(short what)
	{
		if (what & BEV_EVENT_ERROR || what & BEV_EVENT_TIMEOUT) {

			_error_handler();

			stop();

		} else if (what & BEV_EVENT_CONNECTED) {
			
			evutil_socket_t sock = bufferevent_getfd(_bufferEvent);

			std::string connection_name = _name;
			connection_name.append(":");
			connection_name.append(boost::to_string(sock));

			TcpConnectionPtr connection = boost::make_shared<TcpConnection>(_loop, connection_name, _bufferEvent, _addr);

			_connection_handler(connection);

			_bufferEvent = NULL;
		} else {
			LOG4CPLUS_WARN(light_logger, "unhandled event : " << what << " name : " << _name);
		}

		stop();
	}

	bool TcpConnector::restart()
	{
		BOOST_ASSERT(_loop->isInLoopThread());

		_is_runing = true;

		if (_bufferEvent != NULL) {
			bufferevent_setcb(_bufferEvent, NULL, NULL, NULL, this);
			bufferevent_free(_bufferEvent);
			_bufferEvent = NULL;
		}
		
		_bufferEvent = bufferevent_socket_new(_loop->getEventBase(), -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
		if (_bufferEvent == NULL) {
			_is_runing = false;
			return false;
		}

		bufferevent_setcb(_bufferEvent, 
			NULL, 
			NULL,
			&TcpConnector::_eventCallback,
			this);

		if (bufferevent_socket_connect(_bufferEvent, &_addr, _addrSize) < 0) {
			_is_runing = false;
			bufferevent_free(_bufferEvent);
			_bufferEvent = NULL;
			return false;
		}

		return true;
	}

}