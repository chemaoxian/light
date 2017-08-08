#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include <event2/event.h>


namespace light {

typedef intptr_t TimerId;

class event_loop : boost::noncopyable
{
public:
	typedef boost::function<void()> Callback;
	
public:
	event_loop();
	
	~event_loop();

	void loop();

	void quit();
	
	void run(const Callback& callback);

	TimerId run_at(const Callback& callback);
	
	TimerId run_after(const Callback& callback);
	
	TimerId run_every(const Callback& callback);

	bool is_pedding(TimerId id);

	void cancel(TimerId id);

	// internal use
	event_base* get_event_base();

private:
	event_base* _event_base;
};

}

#endif