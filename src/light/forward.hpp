#ifndef _LIGHT_FORWARD_H_
#define _LIGHT_FORWARD_H_

#include <stdint.h>
#include <string>

#include <boost/function.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/any.hpp>
#include <boost/unordered_map.hpp>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/buffer.h>
#include <event2/dns.h>

#include <light/buffer.h>
#include <light/duration.h>

namespace light {

	namespace codec {

		enum CodecStatus {
			kNeedMore,
			kComplete,
			kError
		};


	}

class TimerEvent;

class EventLoop;

class SignalEvent;

class TcpConnection;

class EventLoopThread;

class EventLoopThreadPool;

typedef boost::shared_ptr<Buffer> BufferPtr;

typedef boost::shared_ptr<TimerEvent> TimerEventPtr;

typedef boost::shared_ptr<SignalEvent> SignalEventPtr;

typedef boost::shared_ptr<EventLoop> EventLoopPtr;

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef boost::shared_ptr<EventLoopThreadPool>	EventLoopThreadPoolPtr;

typedef boost::function<void()> Handler;

typedef boost::function<void(TcpConnectionPtr&, const BufferPtr&)> MessageHandler;

typedef boost::function<void(TcpConnectionPtr&)> ConnectionHandler;
}

#endif
