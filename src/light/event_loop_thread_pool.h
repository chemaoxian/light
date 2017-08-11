#ifndef _LIGHT_EVENT_LOOP_THREAD_POOL_H_
#define _LIGHT_EVENT_LOOP_THREAD_POOL_H_

#include <light/forward.hpp>
#include <light/event_loop_thread.h>

namespace light {

class EventLoopThreadPool : boost::noncopyable {
public:
    
	EventLoopThreadPool(const std::string& name,
						uint32_t threadCount);

	~EventLoopThreadPool();

	bool start();

	bool stop();

	bool isRunning();

	EventLoopPtr getNextEventLoop();

	EventLoopPtr getEventLoopByIndex(uint32_t index);

	EventLoopPtr getParentEventLoop();

private:
	std::string _name;
	std::vector<EventLoopThread> _threads;
};


}

#endif