#include <light/event_loop_thread.h>
#include <light/event_loop.h>
#include <light/log4cplus_forward.h>

namespace light {

EventLoopThread::EventLoopThread(EventLoopPtr parentLooper, const std::string& name)
	:_parentLooper(parentLooper),
	 _loop(boost::make_shared<EventLoop>(name + ":EventLoop")),
	 _name(name),
	 _status(kNotInitialized) {

}

EventLoopThread::~EventLoopThread() {
	stop();
}

bool EventLoopThread::start() {
	
	Status expiredStatus = kNotInitialized;
	if (_status.compare_exchange_strong(expiredStatus, kStarting)) {

		_thread.reset(new boost::thread(boost::bind(&EventLoopThread::threadLoop, this)));

		boost::lock_guard<boost::mutex> guard(_lock);
		while (_status.load() != kStarted) {
			_cond.wait(boost::unique_lock<boost::mutex>(_lock));
		}

		LOG4CPLUS_WARN(glog, "start event loop thread : " << _name << " success."
			<< " current status : " << getStatusName()
			<< " tid : " << getId());

		return true;

	} else {

		LOG4CPLUS_WARN(glog, "start event loop thread : " << _name << " failed, current status : " << getStatusName());

		return false;
	}	
}


bool EventLoopThread::isInThreadLoop() {
	return _loop->isInLoopThread();
}

bool EventLoopThread::stop(bool handlePendding)
{
	Status expiredStatus = kStarted;
	if (_status.compare_exchange_strong(expiredStatus, kStarting)) {
		_loop->stop(handlePendding);

		if (_thread->joinable()) {
			_thread->join();
		}

		_status.store(kNotInitialized);

		LOG4CPLUS_WARN(glog, "stop event loop thread : " << _name << " success."
			<< " current status : " << getStatusName()
			<< " tid : " << getId());

		return true;
	} else {

		LOG4CPLUS_WARN(glog, "stop event loop thread : " << _name << " failed, current status : " << getStatusName());

		return false;
	}
}

void EventLoopThread::threadLoop()
{
	LOG4CPLUS_TRACE(glog, "thread begin, name = " 
		<< _name << " tid = " 
		<< boost::this_thread::get_id());
	{
		boost::lock_guard<boost::mutex> guard(_lock);
		_cond.notify_all();
	}

	_loop->loop();

	LOG4CPLUS_TRACE(glog, "thread end, name = " 
		<< _name << " tid = " 
		<< boost::this_thread::get_id());
}

boost::thread::id EventLoopThread::getId() {
	return _loop->get_id();
}

const char* EventLoopThread::getStatusName()
{
	switch (_status.load()) {
	case kNotInitialized:
		return "kNotInitialized";
	case kStarting:
		return "kStarting";
	case kStarted:
		return "kStarted";
	case kStopping:
		return "kStopping";
	default:
		return "invalid state";
	}
}

}