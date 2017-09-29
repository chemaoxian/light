#ifndef _LIGHT_CODEC_H_
#define _LIGHT_CODEC_H_

#include <light/forward.hpp>
#include <boost/type_traits.hpp>

namespace light {
	
namespace codec {

	template <typename HeaderType>
	class DefaultPacketCodecHandler {
	public:
		DefaultPacketCodecHandler(){}
		~DefaultPacketCodecHandler(){}

		 codec::CodecStatus operator () (evbuffer* inputBuffer, BufferPtr& outBuffer) const {
			BOOST_STATIC_ASSERT(boost::is_integral<HeaderType>::value == true && sizeof(HeaderType) <= 4);

			size_t input_len = evbuffer_get_length(inputBuffer);
			if (input_len < sizeof(HeaderType)) {
				return kNeedMore;
			}

			HeaderType header_len;
			ev_ssize_t read_header_len = evbuffer_copyout(inputBuffer, &header_len, sizeof(header_len)) == sizeof(header_len);
			BOOST_ASSERT(read_header_len == sizeof(header_len));

			u_long host_header_len = ntohl(header_len);

			if (input_len < host_header_len)
			{
				return kNeedMore;
			}

			outBuffer->EnsureWritableBytes(host_header_len);

			ev_ssize_t remove_size = evbuffer_remove(inputBuffer, outBuffer->WriteBegin(), host_header_len);
			BOOST_ASSERT(remove_size == host_header_len);

			return kComplete;
		}
	};

	class DefaultStringCoderHandler {
	public:
		DefaultStringCoderHandler(){}
		~DefaultStringCoderHandler(){}

		codec::CodecStatus operator () (evbuffer* inputBuffer, BufferPtr& outBuffer) const {
			size_t readLen = 0;
			char* readLine = evbuffer_readln(inputBuffer, &readLen, EVBUFFER_EOL_CRLF);
			if (readLine == NULL) {
				free(readLine);
				return kNeedMore;
			}

			outBuffer->EnsureWritableBytes(readLen + 1);
			outBuffer->Write(readLine, readLen+1);

			free(readLine);

			return kComplete;
		}
	};

}
}


#endif
