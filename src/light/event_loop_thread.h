#ifndef _LIGHT_EVENT_LOOP_THREAD_H_
#define _LIGHT_EVENT_LOOP_THREAD_H_

#include <light/forward.hpp>

namespace light {

class EventLoopThread : public boost::noncopyable
{
public:
	enum Status {
		kNotInitialized,
		kStarting,
		kStarted,
		kStopping
	};

	EventLoopThread(const std::string& name);

	~EventLoopThread();

	// NOT thead safe
	EventLoopPtr start();

	bool isRunning() {return _started;}

	EventLoopPtr getEventLoop() {return _loop;}

private:
	void threadLoop();

	// NOT thread safe
	void _stop();
private:
	bool _started;
	boost::mutex _lock;
	boost::condition_variable _cond;
	EventLoopPtr _loop;
	std::string _name;
	boost::scoped_ptr<boost::thread> _thread;
};

}

#endif