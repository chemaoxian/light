#include <light/event_loop_thread.h>
#include <light/event_loop.h>
#include <light/log4cplus_forward.h>

namespace light {



EventLoopThread::EventLoopThread(const std::string& name)
	: _name(name), 
	_loop(boost::make_shared<EventLoop>(name + ":" + "EventLoop")),
	_isStarted(false){

}

EventLoopThread::~EventLoopThread() {
	stop();
}

bool EventLoopThread::start() {

	bool oldValue = false;
	if (_isStarted.compare_exchange_strong(oldValue, true)) {
		_thread.reset(new boost::thread(boost::bind(&EventLoopThread::threadLoop, this)));
		_tid = _thread->get_id();
	}

	return true;
}

bool EventLoopThread::isRunning() {
	return _isStarted.load();
}

bool EventLoopThread::isInThreadLoop() {
	if (!_isStarted.load()) {
		return false;
	}

	return _tid == boost::this_thread::get_id();
}

void EventLoopThread::stop(bool handlePendding)
{
	bool oldValue = true;
	if (_isStarted.compare_exchange_strong(oldValue, false)) {
		_loop->stop(handlePendding);
		_thread->join();
		_thread.reset();
	}
}

void EventLoopThread::threadLoop()
{
	LOG4CPLUS_TRACE(LOG4CPLUS_LOGGER, "thread begin, name = " 
		<< _name << " tid = " 
		<< boost::this_thread::get_id());

	_loop->loop();

	LOG4CPLUS_TRACE(LOG4CPLUS_LOGGER, "thread end, name = " 
		<< _name << " tid = " 
		<< boost::this_thread::get_id());
}

boost::thread::id EventLoopThread::getId() {
	if (!_isStarted.load()) {
		return boost::thread::id();
	}

	return _tid;
}

}