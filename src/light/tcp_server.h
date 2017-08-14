#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <stdint.h>
#include <string>
#include <boost/noncopyable.hpp>
#include <event2/event.h>


namespace light {

class TcpServer : public boost::noncopyable {
public:
    TcpServer();
    
	~TcpServer();

    bool start(const std::string& host, uint16_t port);
};


}

#endif