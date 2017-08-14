#include <light/timer_event.h> 
#include <light/event_loop.h>
#include <light/exception.h>

namespace light {

TimerEvent::TimerEvent(EventLoopPtr& eventLoop)
	: _loop(eventLoop), 
	 _repeat(false),
	  _started(false){

	_timerEvent = evtimer_new(_loop->getEventBase(), &TimerEvent::_eventTimerCallback, this);
	
	if (_timerEvent == NULL) {
		throw LightException("evtimer_new failed");
	}
}

TimerEvent::~TimerEvent() {
	cancel();
	event_free(_timerEvent);
}

void TimerEvent::_start(const Duration& duration, const Handler& handler, bool repeat) {
	event_add(_timerEvent, &(duration.TimeVal()));
	_repeat = repeat;
	_handler = handler;
	_interval = duration;
	_started.store(true);
}

bool TimerEvent::isCanceled()
{
	return !_started.load();
}

void TimerEvent::cancel()
{
	bool started = true;
	if (_started.compare_exchange_strong(started, false))
	{
		event_del(_timerEvent);
		_selfPtr.reset();
	}
}

void TimerEvent::_eventTimerCallback(evutil_socket_t fd, short what, void* that)
{
	TimerEvent* ptimer = static_cast<TimerEvent*>(that);
	ptimer->_handleTimer();
}

bool TimerEvent::isReapt() {
	return _repeat;
}

void TimerEvent::_handleTimer()
{
	_handler();

	if (isReapt()) {
		struct timeval tv = _interval.TimeVal();
		event_add(_timerEvent, &tv);
	} else {
		cancel();
	}
}

Duration TimerEvent::getDuration()
{
	return _interval;
}

light::TimerEventPtr TimerEvent::create(EventLoopPtr& eventLoop, const Duration& duration, const Handler& handler, bool repeat/*=false*/) {
	TimerEventPtr timer = boost::shared_ptr<TimerEvent>(new TimerEvent(eventLoop));
	timer->_start(duration, handler, repeat);
	timer->_selfPtr = timer;

	return timer;
}

}