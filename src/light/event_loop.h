#ifndef _LIGHT_EVENT_LOOP_H_
#define _LIGHT_EVENT_LOOP_H_

#include <light/forward.hpp>
#include <light/duration.h>

namespace light {

	class EventLoop : public boost::enable_shared_from_this<EventLoop>
	{	
	public:
		EventLoop(const std::string& name);

		~EventLoop();

		void loop();

		void stop(bool handlePenddingEvent=false);

		bool isRuning();

		void runInLoop(const Handler& handler);

		void runInQueue(const Handler& handler);

		TimerEventPtr runAfter(const Duration& interval, const Handler& handler);

		TimerEventPtr runEvery(const Duration& interval, const Handler& handler);

		bool isInLoopThread();

		event_base* getEventBase() {return _eventBase;}

		std::string getName() {return _name;}

		boost::thread::id get_id();
	private:
		void _doPenddingHandlers();

		static void notifyCallback(evutil_socket_t fd, short what, void* that);
	private:
		boost::thread::id _tid;
		event_base* _eventBase;
		boost::mutex _pendingLock;
		std::list<Handler> _pendingHandles;
		std::string _name;
	};

}

#endif