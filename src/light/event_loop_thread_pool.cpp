#include <light/event_loop_thread_pool.h>
#include <algorithm>
#include <boost/format.hpp>

namespace light {

EventLoopThreadPool::EventLoopThreadPool(EventLoopPtr& parentLoop, const std::string& name)
	:_parentLoop(parentLoop), _name(name) {

}

EventLoopThreadPool::~EventLoopThreadPool() {
	stop();
}

void EventLoopThreadPool::start(uint32_t threadCount) {
	
	threadCount = max(threadCount, 1);
	for (size_t i=0; i<threadCount; i++) {
		boost::scoped_ptr<EventLoopThread> threadPtr(new EventLoopThread(_parentLoop, 
																		 (boost::format("%s:%d") % _name % (i+1)).str()));
		threadPtr->start();

		_threads.push_back(threadPtr);
	}
}

void EventLoopThreadPool::stop() {
	for (size_t i=0; i<_threads.size(); i++) {
		_threads[i]->stop();
	}
}

EventLoopPtr EventLoopThreadPool::getNextEventLoop() {
	
}

EventLoopPtr EventLoopThreadPool::getEventLoopByIndex(uint32_t index) {

}

}