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
#include "o2jb_logging.h"
#include "o2jb_logstream.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

using std::cout;
using std::map;
using std::setfill;
using std::setw;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::time;
using std::time_t;

namespace o2jb {
namespace {
typedef map<string, shared_ptr<LogStream>> log_ctr_t;

string datedLogName(string const& name) {
	time_t t;
	time(&t);   // get time now
	struct tm * now = localtime( & t );

	stringstream ss;
	ss << name << '.' << (now->tm_year + 1900) << '-'
	   << setfill('0') << setw(2) << (now->tm_mon + 1) << '-'
	   << setfill('0') << setw(2) << now->tm_mday
	   << ".log";
	return ss.str();
}

class LogManager {
public:
	static LogManager& instance() {
		return _instance;
	}
	log_ctr_t knownLoggers;

	// ~LogManager() {
	// 	for (log_ctr_t::iterator iter = knownLoggers.begin(); iter != knownLoggers.end(); ++iter) {
	// 		delete iter->second;
	// 	}
	// }
private:
	static LogManager _instance;
};
LogManager LogManager::_instance;

shared_ptr<LogStream> configure(string const& name) {
#ifdef LOG_TO_CONSOLE
	return shared_ptr<LogStream>(new LogStream(cout));
#else
	return shared_ptr<LogStream>(new LogStream(datedLogName(name)));
#endif
}
}


LoggerPtr Logger::getRootLogger() {
	LoggerPtr logger = getLogger("_ROOT_LOGGER_");
	LOG_DEBUG(logger, "Requested the root logger");
	return logger;
}

LoggerPtr Logger::getLogger(std::string const& name) {
	LoggerPtr rtnValue = NULL;
	log_ctr_t::iterator iter = LogManager::instance().knownLoggers.find(name);
	if (LogManager::instance().knownLoggers.end() == iter) {
		log_ctr_t::value_type toAdd(name, configure(name));
		LogManager::instance().knownLoggers.insert(toAdd);
		rtnValue = toAdd.second.get();
	} else {
		rtnValue = iter->second.get();
	}
	return rtnValue;
}

}