#include <light/event_loop_thread_pool.h>
#include <light/inner_log.h>
#include <algorithm>
#include <boost/format.hpp>

namespace light {

	template <typename T>
	T tmax(T x, T y) {
		return x > y ?  x: y;
	}

EventLoopThreadPool::EventLoopThreadPool(const std::string& name)
	:_started(false),
     _nextLoopIndex(0),
	 _name(name) {

}

EventLoopThreadPool::~EventLoopThreadPool() {
	_stop();
}

void EventLoopThreadPool::start(uint32_t threadCount) {

	BOOST_ASSERT( _started == false);

	_started = true;

	threadCount = tmax(threadCount, (uint32_t)1);
	for (size_t i=0; i<threadCount; i++) {
		boost::shared_ptr<EventLoopThread> threadPtr(new EventLoopThread((boost::format("%s:%d") % _name % (i+1)).str()));
		threadPtr->start();
		_threads.push_back(threadPtr);
	}
}

void EventLoopThreadPool::_stop() {
	if (_started) {
		_threads.clear();
	}
}

EventLoopPtr EventLoopThreadPool::getNextEventLoop() {
	uint32_t nextIndex = _nextLoopIndex.fetch_add(1);

	uint32_t theadIndex = nextIndex % threadCount();

	return getEventLoopByIndex(theadIndex);
}

EventLoopPtr EventLoopThreadPool::getEventLoopByIndex(uint32_t index) {
	BOOST_ASSERT(index >= 0 && index < _threads.size());

	return _threads[index]->getEventLoop();
}

}
