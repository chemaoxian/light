#include <light/event_loop_thread.h>
#include <light/event_loop.h>
#include <light/log4cplus_forward.h>

namespace light {

EventLoopThread::EventLoopThread(EventLoopPtr parentLooper, const std::string& name)
	:_parentLooper(parentLooper),
	 _loop(boost::make_shared<EventLoop>(name + ":EventLoop")),
	 _name(name),
	 _isRunning(false) {

}

EventLoopThread::~EventLoopThread() {
	stop();
}

void EventLoopThread::start() {
	
	_isRunning = true;

	_thread.reset(new boost::thread(boost::bind(&EventLoopThread::threadLoop, this)));

	boost::lock_guard<boost::mutex> guard(_lock);
	while (!_isRunning) {
		_cond.wait(boost::unique_lock<boost::mutex>(_lock));
	}
}

bool EventLoopThread::isRunning() {
	return _isRunning;
}

bool EventLoopThread::isInThreadLoop() {
	return _loop->isInLoopThread();
}

void EventLoopThread::stop(bool handlePendding)
{
	if (!_isRunning) {
		return ;
	}

	_isRunning = false;

	_loop->stop(handlePendding);

	if (_thread->joinable()) {
		_thread->join();
	}
}

void EventLoopThread::threadLoop()
{
	LOG4CPLUS_TRACE(LOG4CPLUS_LOGGER, "thread begin, name = " 
		<< _name << " tid = " 
		<< boost::this_thread::get_id());
	{
		boost::lock_guard<boost::mutex> guard(_lock);
		_cond.notify_all();
	}

	_loop->loop();

	LOG4CPLUS_TRACE(LOG4CPLUS_LOGGER, "thread end, name = " 
		<< _name << " tid = " 
		<< boost::this_thread::get_id());
}

boost::thread::id EventLoopThread::getId() {
	return _loop->get_id();
}

}