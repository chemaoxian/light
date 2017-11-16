#ifndef _LIGHT_LOGGER_H_
#define _LIGHT_LOGGER_H_

#include <log4cplus/loggingmacros.h>

#include <log4cplus/loggingmacros.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

#define LT_LOGGER_CLASS_DECL(logger) static log4cplus::Logger logger
#define LT_LOGGER_CLASS_IMPL(logger, classname) log4cplus::Logger classname::logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(#classname))

#define LT_LOGGER_EXTERN_DECL(logger)  extern log4cplus::Logger logger
#define LT_LOGGER_IMPL(logger, name)   log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(name))

#define LT_LOGGER_STATIC_DECL_IMPL(logger, name) static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(name))
#define LT_LOGGER_STATIC_DECL_IMPL_ROOT(logger)  static log4cplus::Logger logger = log4cplus::Logger::getRoot()

#define LT_LOG_TRACE(logger,e)  LOG4CPLUS_TRACE(logger,e)
#define LT_LOG_DEBUG(logger,e)  LOG4CPLUS_DEBUG(logger,e)
#define LT_LOG_INFO(logger,e)   LOG4CPLUS_INFO(logger,e)
#define LT_LOG_WARN(logger,e)   LOG4CPLUS_WARN(logger,e)
#define LT_LOG_ERROR(logger,e)  LOG4CPLUS_ERROR(logger,e)
#define LT_LOG_FATAL(logger,e)  LOG4CPLUS_FATAL(logger,e)

#endif