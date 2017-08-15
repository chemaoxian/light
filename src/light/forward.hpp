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
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/util.h>

#include <light/buffer.h>

namespace light {

class TimerEvent;

class EventLoop;

class SignalEvent;

class TcpConnection;

typedef boost::shared_ptr<TimerEvent> TimerEventPtr;

typedef boost::shared_ptr<SignalEvent> SignalEventPtr;

typedef boost::shared_ptr<EventLoop> EventLoopPtr;

typedef boost::shared_ptr<TcpConnection> TcpconnectionPtr;


typedef boost::function<void()> Handler;

typedef boost::function<void(TcpconnectionPtr, evbuffer* input)> MessageHandler;

typedef boost::function<void(TcpconnectionPtr)> ConnectionHandler;
}

#endif