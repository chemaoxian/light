#include <light/timer.h> 
#include <light/event_loop.h>
#include <light/exception.h>

namespace light {

Timer::Timer(const EventLoopPtr& eventLoop)
	: _loop(eventLoop), 
	 _repeat(false) {

	_timerEvent = evtimer_new(_loop->getEventBase(), &Timer::_eventTimerCallback, this);
	
	if (_timerEvent == NULL) {
		throw LightException("evtimer_new failed");
	}
}

Timer::~Timer() {
	cancel();
	event_free(_timerEvent);
}

bool Timer::start(const Duration& duration, const Handler& handler, bool repeat) {
	event_add(_timerEvent, &(duration.TimeVal()));
	_repeat.store(repeat);
	_duration = duration;

	return true;
}

bool Timer::isCanceled()
{
	return event_pending(_timerEvent, EV_TIMEOUT, NULL) == 0;
}

void Timer::cancel()
{
	event_del(_timerEvent);
}

void Timer::_eventTimerCallback(evutil_socket_t fd, short what, void* that)
{
	Timer* ptimer = static_cast<Timer*>(that);
	ptimer->_handleTimer();
}

bool Timer::isReapt() {
	return _repeat;
}

void Timer::_handleTimer()
{
	_handler();

	if (isReapt()) {
		event_add(_timerEvent, &(_duration.TimeVal()));
	}
}

Duration Timer::getDuration()
{
	struct timeval tv;
	evutil_timerclear(&tv);
	evtimer_pending(_timerEvent, &tv);

	return Duration(tv);
}


}