#ifndef _LIGHT_TCP_CLIENT_H_
#define _LIGHT_TCP_CLIENT_H_

#include <stdint.h>
#include <string>
#include <boost/noncopyable.hpp>
#include <event2/event.h>

namespace light {

	class TcpClient : public boost::noncopyable {
	public:
		TcpClient();

		~TcpClient();

		bool start(const std::string& host, uint16_t port);
	};


}


#endif