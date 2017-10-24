#include <light/tcp_server.h>
#include <light/inner_log.h>
#include <light/event_loop.h>
#include <light/event_loop_thread_pool.h>
#include <light/tcp_connection.h>

namespace light {

	TcpServer::TcpServer(EventLoopPtr& loop, const std::string& name)
		:_is_running(false),
		 _listener(loop, name + ":" + "TcpListener"),
		 _loop(loop),
		 _threadPool(boost::make_shared<EventLoopThreadPool>(name + ":EventLoopThreadPool")),
		 _max_connection(0),
		 _name(name),
		 _next_connection_id(0)
	{
		_connection_handler = boost::bind(&TcpServer::_default_connection_handler, this, _1);
		_message_handler = boost::bind(&TcpServer::_default_message_handler, this, _1, _2);
		_codecHandler = codec::DefaultPacketCodecHandler<uint16_t>();

		_listener.setNewConnectionHandler(boost::bind(&TcpServer::_new_connection_handler, this, _1, _2, _3));
		_listener.setErrorHandler(boost::bind(&TcpServer::_listener_error_handler, this));
	}

	TcpServer::~TcpServer()
	{

	}

	bool TcpServer::start(const std::string& host, int threadCount, int max_connection/*=50000*/)
	{
		BOOST_ASSERT(threadCount >= 0);
		BOOST_ASSERT(max_connection > 0);

		bool expect = false;
		if (_is_running.compare_exchange_strong(expect, true)) {

			if (!_listener.start(host)) {
				return false;
			}

			_max_connection = max_connection;

			_threadPool->start(threadCount);
		}

		return true;
	}

	void TcpServer::_default_connection_handler(TcpConnectionPtr& conn)
	{

	}

	void TcpServer::_default_message_handler(TcpConnectionPtr& buffer, const BufferPtr&)
	{

	}

	void TcpServer::_new_connection_handler(evutil_socket_t s, struct sockaddr* addr, int socklen)
	{
		if (_max_connection <= _connections.size())
		{
			evutil_closesocket(s);
		}
		else
		{
			EventLoopPtr next_looper = _threadPool->getNextEventLoop();
			uint32_t conn_id = _next_connection_id ++;
			std::string conn_name = _name + ":" + boost::to_string(conn_id);

            evutil_make_socket_nonblocking(s);
			TcpConnectionPtr new_conn = boost::make_shared<TcpConnection>(next_looper,conn_name, s, *addr);

			new_conn->setMessageHandler(_message_handler);
			new_conn->setConnectionHandler(_connection_handler);
			new_conn->setCodecHandler(_codecHandler);
			new_conn->setCloseHandler(boost::bind(&TcpServer::_close_connection_handler, this, _1));

			if (!new_conn->start())
			{
				return ;
			}

			_connections.insert(ConnectionMap::value_type(conn_name, new_conn));
		}
	}

	void TcpServer::_listener_error_handler()
	{

	}

	void TcpServer::_close_connection_handler(TcpConnectionPtr& conn)
	{
		_loop->runInQueue(boost::bind(&TcpServer::_close_connection_handler_in_loop, this, conn));
	}

	void TcpServer::_close_connection_handler_in_loop(TcpConnectionPtr& conn)
	{
		_connections.erase(conn->getName());
	}

}
