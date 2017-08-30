#ifndef _LIGHT_CODEC_H_
#define _LIGHT_CODEC_H_

#include <light/forward.hpp>
#include <boost/asio/detail/socket_ops.hpp>

namespace light {


	enum CodecResult {
		kSuccess,
		kImcomplete,
		kBadPacket
	};

	typedef boost::function<CodecResult(evbuffer*, Buffer*)> CodecHandler;

	template <typename HeaderType>
	class DefaultCodecHandler {
	public:
		DefaultCodecHandler(){}
		~DefaultCodecHandler(){}

		CodecResult operator () (evbuffer* input_buffer, Buffer* out_message) const {
			BOOST_STATIC_ASSERT(boost::is_integral<HeaderType>::value == true && sizeof(HeaderType) <= 4);

			int input_len = evbuffer_get_length(input_buffer);
			if (input_len < sizeof(HeaderType)) {
				return kImcomplete;
			}

			HeaderType header_len;
			ev_ssize_t read_header_len = evbuffer_copyout(input_buffer, &header_len, sizeof(header_len)) == sizeof(header_len);
			BOOST_ASSERT(read_header_len == sizeof(header_len));

			u_long host_header_len = ntohl(header_len);

			if (input_len < host_header_len)
			{
				return kImcomplete;
			}

			out_message->EnsureWritableBytes(host_header_len);

			ev_ssize_t remove_size = evbuffer_remove(input_buffer, out_message->WriteBegin(), host_header_len);
			BOOST_ASSERT(remove_size == host_header_len);

			return kSuccess;
		}
	};

}


#endif