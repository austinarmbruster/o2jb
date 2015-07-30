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

#include <string>

using std::string;
using o2jb::LoggerPtr;
using o2jb::Logger;

int main(int argc, char** argv) {
  string name = "Bob";
  LoggerPtr logger = Logger::getLogger("test");
  LOG_TRACE(logger, "this is trace:  " << name);
  LOG_DEBUG(logger, "this is debug:  " << name);
  LOG_INFO(logger, "this is info:  " << name);
  LOG_WARN(logger, "this is warn:  " << name);
  LOG_ERROR(logger, "this is error:  " << name);
  LOG_FATAL(logger, "this is fatal:  " << name);
  return 0;
}

