#ifndef _LIGHT_LIGHT_H_
#define _LIGHT_LIGHT_H_

#include <light/forward.hpp>
#include <light/buffer.h>
#include <light/duration.h>
#include <light/codec.h>
#include <light/tcp_client.h>
#include <light/tcp_server.h>
#include <light/tcp_connection.h>
#include <light/event_loop.h>
#include <light/event_loop_thread.h>
#include <light/event_loop_thread_pool.h>
#include <light/timer_event.h>
#include <light/signal_event.hpp>
#include <light/exception.h>


namespace light {

void initilize(const std::string& filename);

}


#endif