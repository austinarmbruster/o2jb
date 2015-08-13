#ifndef _INCLUDE_GUARD_O2JB_LOGGING_H_
#define _INCLUDE_GUARD_O2JB_LOGGING_H_ 1
/*
 * Copyright 2015 AnaVation, LLC. 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TimeReporter.h"

/**
 * Use LOG_TO_CONSOLE to log to the console.
 */

#include <string>
#include <ostream>

#include <iostream>

#ifndef LOG_LEVEL
#define LOG_LEVEL 5
#endif

#if LOG_LEVEL > 0
static o2jb::TimeReporter now;

#define LOG_FATAL(logger, expression) std::ostream(logger) << now << " FATAL " << __FILE__ << ":" << __LINE__ << " {" << expression << "}\n" << std::flush
#else
#define LOG_FATAL(logger, expression) 
#endif

#if LOG_LEVEL > 1
#define LOG_ERROR(logger, expression) (*logger).stream() << now << " ERROR " << __FILE__ << ":" << __LINE__ << " {" << expression << "}\n" << std::flush
#else
#define LOG_ERROR(logger, expression) 
#endif

#if LOG_LEVEL > 2
#define LOG_WARN(logger, expression) std::ostream(logger) << now << " WARN " << __FILE__ << ":" << __LINE__ << " {" << expression << "}\n" << std::flush
#else
#define LOG_WARN(logger, expression) 
#endif

#if LOG_LEVEL > 3
#define LOG_INFO(logger, expression) std::ostream(logger) << now << " INFO " << __FILE__ << ":" << __LINE__ << " {" << expression << "}\n" << std::flush
#else
#define LOG_INFO(logger, expression) 
#endif

#if LOG_LEVEL > 4
#define LOG_DEBUG(logger, expression) std::ostream(logger) << now << " DEBUG " << __FILE__ << ":" << __LINE__ << " {" << expression << "}\n" << std::flush
#else
#define LOG_DEBUG(logger, expression) 
#endif

#if LOG_LEVEL > 5
#define LOG_TRACE(logger, expression) std::ostream(logger) << now << " TRACE " << __FILE__ << ":" << __LINE__ << " {" << expression << "}\n" << std::flush
#else
#define LOG_TRACE(logger, expression) 
#endif


namespace o2jb {
class LogStream;

typedef LogStream* LoggerPtr;
class Logger {
public:
	static LoggerPtr getRootLogger();
	static LoggerPtr getLogger(std::string const& name);
};
}
#endif
