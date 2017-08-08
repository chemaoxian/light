#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <stdexcept>

namespace light {

class light_exception : public std::runtime_error 
{
public:
    light_exception(const std::string& what):std::runtime_error(what) {
    }

    ~light_exception() {
    }
};


}
#endif