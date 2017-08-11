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

	EventLoopThread(EventLoopPtr parentLoop, const std::string& name);

	~EventLoopThread();

	bool start();

	bool stop(bool handlePendding=true);

	bool isRunning() {return _status.load() == kStarted;}

	Status getStatus() {return _status.load();}

	const char* getStatusName();

	bool isInThreadLoop();

	EventLoopPtr getEventLoop() {return _loop;}

	boost::thread::id getId();
private:
	void threadLoop();

private:
	boost::atomic<Status> _status;
	boost::mutex _lock;
	boost::condition_variable _cond;
	EventLoopPtr _parentLooper;
	EventLoopPtr _loop;
	std::string _name;
	boost::scoped_ptr<boost::thread> _thread;
};

}

#endif