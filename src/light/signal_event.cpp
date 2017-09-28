#include <light/signal_event.hpp>
#include <light/event_loop.h>
#include <light/exception.h>

namespace light {

SignalEventPtr SignalEvent::create(EventLoopPtr eventLoop, int sig, const Handler& handler, bool repeat) {
	SignalEventPtr sigEventPtr = boost::shared_ptr<SignalEvent>(new SignalEvent(eventLoop, sig, repeat));
	sigEventPtr->_start(handler);
	sigEventPtr->_selfPtr = sigEventPtr;

	return sigEventPtr;
}


SignalEvent::SignalEvent(EventLoopPtr& eventLoop, int sig, bool repeat)
	:_loop(eventLoop),
	 _repeat(repeat),
	 _started(false),
	 _sig(sig){

	uint32_t flags = EV_SIGNAL;
	if (_repeat) {
		flags |= EV_PERSIST;
	}

	_event = event_new(_loop->getEventBase(), _sig, flags, &SignalEvent::_eventCallback, this);

	if (_event == NULL) {
		throw LightException("signal init failed");
	}
}

SignalEvent::~SignalEvent() {
	cancel();
	event_free(_event);
}

void SignalEvent::cancel() {
	bool started = true;
	if (_started.compare_exchange_strong(started, false)) {
		event_del(_event);
		_selfPtr.reset();
	}
}

bool SignalEvent::isCanceled() {
	return !_started.load();
}

bool SignalEvent::isRepeat() {
	return _repeat;
}

int SignalEvent::getSig() {
	return _sig;
}


void SignalEvent::_start(const Handler& handler) {
	if (event_add(_event, NULL) < 0) {
		throw LightException("signal event add failed");
	}

	_handler = handler;
	_started.store(true);
}

void SignalEvent::_handlerEvent() {

	_handler();

}

void SignalEvent::_eventCallback(evutil_socket_t fd, short what, void* that) {
	SignalEvent* sigEvent = static_cast<SignalEvent*>(that);
	sigEvent->_handlerEvent();
}

}

