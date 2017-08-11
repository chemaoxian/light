#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <stdint.h>
#include <string>
#include <boost/noncopyable.hpp>
#include <event2/event.h>


namespace light {

class tcp_server : public boost::noncopyable {
public:
    tcp_server();
    
	~tcp_server();

    bool start(const std::string& host, uint16_t port);
};


}

#endif