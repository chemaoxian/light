#ifndef _LIGHT_CODEC_H_
#define _LIGHT_CODEC_H_

#include <light/forward.hpp>
#include <boost/type_traits.hpp>

namespace light {
	
	typedef boost::shared_ptr<Buffer> BufferPtr;
	typedef boost::function<BufferPtr(evbuffer*)> CodecHandler;

	template <typename HeaderType>
	class DefaultCodecHandler {
	public:
		DefaultCodecHandler(){}
		~DefaultCodecHandler(){}

		 BufferPtr operator () (evbuffer* input_buffer) const {
			BOOST_STATIC_ASSERT(boost::is_integral<HeaderType>::value == true && sizeof(HeaderType) <= 4);

			size_t input_len = evbuffer_get_length(input_buffer);
			if (input_len < sizeof(HeaderType)) {
				return false;
			}

			HeaderType header_len;
			ev_ssize_t read_header_len = evbuffer_copyout(input_buffer, &header_len, sizeof(header_len)) == sizeof(header_len);
			BOOST_ASSERT(read_header_len == sizeof(header_len));

			u_long host_header_len = ntohl(header_len);

			if (input_len < host_header_len)
			{
				return NULL;
			}

			BufferPtr buffer_ptr= boost::make_shared<Buffer>(host_header_len, 0);
			buffer_ptr->EnsureWritableBytes(host_header_len);

			ev_ssize_t remove_size = evbuffer_remove(input_buffer, buffer_ptr->WriteBegin(), host_header_len);
			BOOST_ASSERT(remove_size == host_header_len);

			return buffer_ptr;
		}
	};

}


#endif