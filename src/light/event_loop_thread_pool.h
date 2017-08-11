#ifndef _LIGHT_EVENT_LOOP_THREAD_POOL_H_
#define _LIGHT_EVENT_LOOP_THREAD_POOL_H_

#include <light/forward.hpp>
#include <light/event_loop_thread.h>

namespace light {

class EventLoopThreadPool : boost::noncopyable {
public:
	EventLoopThreadPool(EventLoopPtr& parentLoop, const std::string& name);

	~EventLoopThreadPool();

	void start(uint32_t threadCount);

	void stop();

	EventLoopPtr getNextEventLoop();

	EventLoopPtr getEventLoopByIndex(uint32_t index);

	EventLoopPtr getParentEventLoop() {return _parentLoop;}

	std::string	 getName() {return _name;}
private:
	boost::mutex _lock;
	EventLoopPtr _parentLoop;
	std::string _name;
	std::vector<boost::scoped_ptr<EventLoopThread>> _threads;
};


}

#endif