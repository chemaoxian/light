#include <light/tcp_client.h>
#include <light/tcp_connector.h>
#include <light/tcp_connection.h>
#include <light/event_loop.h>
#include <light/timer_event.h>
#include <light/log4cplus_forward.h>

namespace light {

	TcpClient::TcpClient(EventLoopPtr& loop, const std::string& name /*= ""*/)
		:_runing(false),
		 _loop(loop),
		 _autoConnector(false),
		 _connector(new TcpConnector(_loop, name)),
		 _connectInterval(0)
	{
		_connector->setNewConnectionHandler(boost::bind(&TcpClient::handleNewConnection, this, _1));
		_connector->setErrorHandler(boost::bind(&TcpClient::handleConnectError, this));
	}

	TcpClient::~TcpClient()
	{
        stopInLoop();
	}

	bool TcpClient::start(const std::string& host, const Duration& connect_duration, bool auto_connect)
	{
		bool expect = false;
		if (_runing.compare_exchange_strong(expect, true)) {

			_loop->runInLoop(boost::bind(&TcpClient::startInLoop, this, host, connect_duration, auto_connect));

			return true;

		} else {

			return false;
		}
	}

	void TcpClient::handleNewConnection(TcpConnectionPtr conn)
	{
		conn->setMessageHandler(_messageHandler);
		conn->setConnectionHandler(_connectionHandler);
		conn->setCloseHandler(boost::bind(&TcpClient::handleCloseConnection, this, _1));
		conn->setCodecHandler(_codecHandler);

		{
			boost::lock_guard<boost::mutex> lock(_connectionLock);
			_connection = conn;
		}

		if (!conn->start())
		{
			return ;
		}
	}

	void TcpClient::handleConnectError()
	{
		TimerEventPtr timer = _loop->runAfter(_connectInterval, boost::bind(&TcpClient::restartInLoop, this));

		{
			boost::lock_guard<boost::mutex> lock(_connectionLock);
			_connection.reset();
			_timerPtr = timer;
		}
	}

	void TcpClient::handleCloseConnection(TcpConnectionPtr conn)
	{
		{
			boost::lock_guard<boost::mutex> lock(_connectionLock);
			_connection.reset();
		}

		TimerEventPtr timer = _loop->runAfter(_connectInterval, boost::bind(&TcpClient::restartInLoop, this));

		{
			boost::lock_guard<boost::mutex> lock(_connectionLock);
			_timerPtr = timer;
		}
	}

	void TcpClient::stop()
	{
		bool expect = true;
		if (_runing.compare_exchange_strong(expect, false)) {

			_loop->runInLoop(boost::bind(&TcpClient::stopInLoop, shared_from_this()));
		}
	}

	void TcpClient::restartInLoop()
	{
		BOOST_ASSERT(_loop->isInLoopThread());

		boost::lock_guard<boost::mutex> lock(_connectionLock);
		_connection.reset();
		if (_timerPtr) {
			_timerPtr->cancel();
			_timerPtr.reset();
		}

		if (!_connector->restart()) {
			_runing.store(false);
		}
	}

	void TcpClient::startInLoop(const std::string& host, const Duration& connect_duration, bool auto_connect)
	{
		BOOST_ASSERT(_loop->isInLoopThread());

		_connectInterval = connect_duration;
		_autoConnector = auto_connect;

		if (!_connector->start(host)) {
			stopInLoop();
			_runing.store(false);
		}
	}

	void TcpClient::stopInLoop()
	{
		BOOST_ASSERT(_loop->isInLoopThread());

		{
			boost::lock_guard<boost::mutex> lock(_connectionLock);
			_connection.reset();
			if (_timerPtr) {
				_timerPtr->cancel();
				_timerPtr.reset();
			}
		}


		_connector->stop();
	}

	light::TcpConnectionPtr TcpClient::getConnection()
	{
		boost::lock_guard<boost::mutex> lock(_connectionLock);
		return _connection;
	}

}
