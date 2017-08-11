#ifndef _LIGHT_EVENT_LOOP_THREAD_H_
#define _LIGHT_EVENT_LOOP_THREAD_H_

#include <light/forward.hpp>

namespace light {

class EventLoopThread : public boost::noncopyable
{
public:
	EventLoopThread(EventLoopPtr parentLoop, const std::string& name);

	~EventLoopThread();

	void start();

	void stop(bool handlePendding=true);

	bool isRunning();

	bool isInThreadLoop();

	EventLoopPtr getEventLoop() {return _loop;}

	boost::thread::id getId();
private:
	void threadLoop();

private:
	bool _isRunning;
	boost::mutex _lock;
	boost::condition_variable _cond;
	EventLoopPtr _parentLooper;
	EventLoopPtr _loop;
	std::string _name;
	boost::scoped_ptr<boost::thread> _thread;
};

}

#endif