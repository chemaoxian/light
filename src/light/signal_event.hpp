#ifndef _LIGHT_SIGNAL_EVENT_H_
#define _LIGHT_SIGNAL_EVENT_H_

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/atomic.hpp>
#include <light/forward.hpp>
#include <light/duration.h>

namespace light {

	class SignalEvent : public boost::enable_shared_from_this<SignalEvent>
	{
	public:
		static SignalEventPtr create(EventLoopPtr& eventLoop, int sig, const Handler& handler, bool repeat);

		~SignalEvent();

		void cancel();

		bool isCanceled();

		bool isRepeat();

		int getSig();
	private:
		SignalEvent(EventLoopPtr& eventLoop, int sig, bool repeat);

		void _start(const Handler& handler);

		void _handlerEvent();

		static void _eventCallback(evutil_socket_t fd, short what, void* that);
	private:
		boost::atomic<bool> _started;
		int		_sig;
		EventLoopPtr _loop;
		Handler	_handler;
		bool	_repeat;
		struct event* _event;

		boost::shared_ptr<SignalEvent> _selfPtr;
	};

}

#endif