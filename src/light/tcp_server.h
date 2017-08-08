#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <string>
#include <event2/event.h>
#include <boost/noncopyable.hpp>

namespace light {

class tcp_server : public boost::noncopyable {
public:
    tcp_server();
    ~tcp_server();

    bool start(const std::string& host, uint16_t port)
};


}

#endif