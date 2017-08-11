#ifndef _LIGHT_EVENT_H_
#define _LIGHT_EVENT_H_

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/atomic.hpp>
#include <light/forward.hpp>
#include <light/duration.h>

namespace light {


class Timer : public boost::enable_shared_from_this<Timer>
{
public:
	Timer(const EventLoopPtr& eventLoop);

	~Timer();

	bool start(const Duration& duration, const Handler& handler, bool repeat=false);

	void cancel();

	bool isCanceled();

	bool isReapt();

	Duration getDuration();
private:
	void _handleTimer();

	static void _eventTimerCallback(evutil_socket_t fd, short what, void* that);
private:
	EventLoopPtr _loop;
	Handler	_handler;
	boost::atomic_bool	_repeat;
	struct event* _timerEvent;
};



}

#endif