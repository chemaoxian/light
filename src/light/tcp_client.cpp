#include <light/tcp_client.h>
#include <light/tcp_connector.h>
#include <light/tcp_connection.h>
#include <light/event_loop.h>
#include <light/timer_event.h>

namespace light {

	TcpClient::TcpClient(EventLoopPtr& loop, const std::string& name /*= ""*/)
		:_runing(false),
		 _autoConnector(false),
		 _connector(new TcpConnector(_loop, name)),
		 _connectInterval(0)
	{
		_connector->setNewConnectionHandler(boost::bind(&TcpClient::handleNewConnection, shared_from_this(), _1));
		_connector->setErrorHandler(boost::bind(&TcpClient::handleConnectError, shared_from_this()));
	}

	TcpClient::~TcpClient()
	{
		stop();
	}

	bool TcpClient::start(const std::string& host, const Duration& connect_duration, bool auto_connect)
	{
		bool expect = false;
		if (_runing.compare_exchange_strong(expect, true)) {

			_loop->runInLoop(boost::bind(&TcpClient::startInLoop, shared_from_this(), host, connect_duration, auto_connect));

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
		conn->stMessageCodec(_codecHandler);

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

}