#ifndef _LIGHT_EVENT_LOOP_THREAD_POOL_H_
#define _LIGHT_EVENT_LOOP_THREAD_POOL_H_

#include <light/forward.hpp>
#include <light/event_loop_thread.h>

namespace light {

class EventLoopThreadPool : boost::noncopyable {
public:
	enum Status {
		kNotInitialized,
		kStarting,
		kStarted,
		kStopping
	};

	EventLoopThreadPool(const std::string& name);

	~EventLoopThreadPool();

	// NOT thead safe
	void start(uint32_t threadCount);

	bool isRunning() { return _started;}

	EventLoopPtr getNextEventLoop();

	EventLoopPtr getEventLoopByIndex(uint32_t index);

	uint32_t	threadCount() {return (uint32_t)_threads.size();}

	std::string	 getName() {return _name;}

private:
	void _stop();

private:
	bool _started;
	boost::atomic<uint32_t> _nextLoopIndex;
	std::string _name;
	std::vector<boost::shared_ptr<EventLoopThread> > _threads;


};


}

#endif