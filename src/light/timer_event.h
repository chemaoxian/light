#ifndef _LIGHT_TIMER_EVENT_H_
#define _LIGHT_TIMER_EVENT_H_

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/atomic.hpp>
#include <light/forward.hpp>
#include <light/duration.h>

namespace light {


class TimerEvent : public boost::enable_shared_from_this<TimerEvent>
{
public:
	static TimerEventPtr create(EventLoopPtr& eventLoop, const Duration& duration, const Handler& handler, bool repeat=false);

	~TimerEvent();

	void cancel();

	bool isCanceled();

	bool isReapt();

	Duration getDuration();
private:
	TimerEvent(EventLoopPtr& eventLoop);

	void _start(const Duration& duration, const Handler& handler, bool repeat);

	void _handleTimer();

	static void _eventTimerCallback(evutil_socket_t fd, short what, void* that);
private:
	boost::atomic<bool> _started;
	Duration _interval;
	EventLoopPtr _loop;
	Handler	_handler;
	bool	_repeat;
	struct event* _timerEvent;

	boost::shared_ptr<TimerEvent> _selfPtr;
};



}

#endif