#ifndef _LIGHT_EVENT_LOOP_THREAD_H_
#define _LIGHT_EVENT_LOOP_THREAD_H_

#include <light/forward.hpp>

namespace light {

class EventLoopThread : public boost::noncopyable
{
public:
	EventLoopThread(const std::string& name);

	~EventLoopThread();

	bool start();

	void stop(bool handlePendding=true);

	bool isRunning();

	bool isInThreadLoop();

	EventLoopPtr getEventLoop() {return _loop;}

	boost::thread::id getId();
private:
	void threadLoop();

private:
	EventLoopPtr _loop;
	std::string _name;
	boost::scoped_ptr<boost::thread> _thread;
	boost::atomic_bool _isStarted;
	boost::thread::id _tid;
};

}

#endif