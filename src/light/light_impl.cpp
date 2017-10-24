#include <light/light.h>
#include <log4cplus/config.hxx>
#include <log4cplus/configurator.h>
#include <event2/event.h>
#include <event2/thread.h>

namespace light {

void initilize(const std::string& filename)
{
	log4cplus::initialize();

	if (!filename.empty()) {
		log4cplus::PropertyConfigurator::doConfigure(filename);
	} else {
		log4cplus::BasicConfigurator::doConfigure();
	}
	
#ifdef WIN32
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif

}

}
