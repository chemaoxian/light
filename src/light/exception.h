#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <stdexcept>

namespace light {

class LightException : public std::runtime_error
{
public:
    LightException(const std::string& what):std::runtime_error(what) {
    }

    ~LightException() throw() {
    }
};


}
#endif
