#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <event2/event.h>
#include <light/event_loop.h>
#include <light/exception.h>
#include <light/log4cplus_forward.h>
#include <light/timer_event.h>

namespace light {

	EventLoop::EventLoop(const std::string& name)
		:_eventBase(event_base_new()),
		 _name(name) {
		
		if (_eventBase == NULL) {
			throw LightException("init event base failed");
		}
	}

	EventLoop::~EventLoop() {
		if (_eventBase != NULL) {
			event_base_free(_eventBase);
			_eventBase = NULL;
		}
	}

	void EventLoop::loop() {
		_tid = boost::this_thread::get_id();

		BOOST_ASSERT_MSG(_eventBase != NULL, "call event_loop::loop() with NULL event base");
		
		LOG4CPLUS_TRACE(glog, "EventLoop " << _name << " Loop begin, tid = " << _tid);

		int ret = event_base_loop(_eventBase, 0);

		LOG4CPLUS_TRACE(glog, "EventLoop " << _name << " Loop end, tid = " << _tid << " ret = " << ret);
	}

	void EventLoop::stop(bool handlePenddingEvent) {

		BOOST_ASSERT_MSG(_eventBase != NULL, "call event_loop::stop() with NULL event base");

		if (handlePenddingEvent) {
			event_base_loopexit(_eventBase, 0);
		} else {
			event_base_loopbreak(_eventBase);
		}

		{
			boost::lock_guard<boost::mutex> _(_pendingLock);
			_pendingHandles.clear();
		}
		
	}

	bool EventLoop::isRuning() {
		BOOST_ASSERT_MSG(_eventBase != NULL, "call event_loop::quit() with NULL event base");

		return event_base_got_break(_eventBase) != 0 ||
			event_base_got_exit(_eventBase);
	}

	void EventLoop::runInLoop(const Handler& handler) {
		if (isInLoopThread()) {
			handler();
		} else {
			runInQueue(handler);
		}
	}

	void EventLoop::runInQueue(const Handler& handler) {
		boost::lock_guard<boost::mutex> _(_pendingLock);
		_pendingHandles.push_back(handler);

		event_base_once(_eventBase, -1, EV_TIMEOUT, &EventLoop::notifyCallback, this, NULL);
	}

	TimerEventPtr EventLoop::runAfter(const Duration& interval, const Handler& handler) {
		return TimerEvent::create(shared_from_this(), interval, handler, false);
	}

	TimerEventPtr EventLoop::runEvery(const Duration& interval, const Handler& handler) {
		return TimerEvent::create(shared_from_this(), interval, handler, false);
	}

	bool EventLoop::isInLoopThread() {
		return boost::this_thread::get_id() == _tid;
	}

	void EventLoop::_doPenddingHandlers()
	{
		std::list<Handler> penddingHandlers;
		
		{
			boost::lock_guard<boost::mutex> _(_pendingLock);
			penddingHandlers.swap(_pendingHandles);
		}

		BOOST_FOREACH (const Handler& handler, penddingHandlers) {
			handler();
		}
	}

	void EventLoop::notifyCallback(evutil_socket_t fd, short what, void* that) {
		EventLoop* looper = static_cast<EventLoop*>(that);
		looper->_doPenddingHandlers();
	}

	boost::thread::id EventLoop::get_id()
	{
		return _tid;
	}

}