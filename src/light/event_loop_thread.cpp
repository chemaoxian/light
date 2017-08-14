#include <light/event_loop_thread.h>
#include <light/event_loop.h>
#include <light/log4cplus_forward.h>

namespace light {

EventLoopThread::EventLoopThread(const std::string& name)
	:_started(false),
	 _name(name){

}

EventLoopThread::~EventLoopThread() {
	_stop();
}

EventLoopPtr EventLoopThread::start() {

	if (_started ) {
		return _loop;
	}

	_started = true;

	_thread.reset(new boost::thread(boost::bind(&EventLoopThread::threadLoop, this)));

	boost::lock_guard<boost::mutex> guard(_lock);
	while (_loop.get() == NULL) {
		_cond.wait(boost::unique_lock<boost::mutex>(_lock));
	}

	LOG4CPLUS_WARN(glog, "event loop thread : " << _name << " started."
			<< " tid : " << _thread->get_id());

	return _loop;
}

void EventLoopThread::_stop()
{
	if (_started) {
		
		{
			boost::lock_guard<boost::mutex> guard(_lock);
			while (_loop.get() == NULL) {
				_cond.wait(boost::unique_lock<boost::mutex>(_lock));
			}
		}
		
		boost::thread::id tid = _loop->get_id();

		_loop->stop(true);

		if (_thread && _thread->joinable()) {
			_thread->join();
		}
		_loop.reset();

		LOG4CPLUS_WARN(glog, "event loop thread : " << _name << " stoped." << " tid : " << tid);
	}
}

void EventLoopThread::threadLoop()
{
	LOG4CPLUS_TRACE(glog, "event loop thread begin, name = " 
		<< _name << " tid = " 
		<< boost::this_thread::get_id());

	{
		boost::lock_guard<boost::mutex> guard(_lock);
		_loop = boost::make_shared<EventLoop>(_name + ":EventLoop");
		_cond.notify_all();
	}

	_loop->loop();

	LOG4CPLUS_TRACE(glog, "event loop thread end, name = " 
		<< _name << " tid = " 
		<< boost::this_thread::get_id());
}

}