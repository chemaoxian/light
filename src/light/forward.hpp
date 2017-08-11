#ifndef _LIGHT_FORWARD_DECLEAR_
#define _LIGHT_FORWARD_DECLEAR_

#include <stdint.h>

#include <boost/function.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>

#include <event2/event.h>

namespace light {

class Timer;
class EventLoop;

typedef boost::function<void()> Handler;

typedef boost::shared_ptr<Timer> TimerPtr;

typedef boost::shared_ptr<EventLoop> EventLoopPtr;

}

#endif