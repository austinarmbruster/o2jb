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

#include <ctime>
#include <ostream>
#include <iomanip>

using std::setfill;
using std::setw;
using std::time;
using std::time_t;

namespace o2jb {

std::ostream& operator<<(std::ostream& out, TimeReporter const& timeReporter) {
  time_t t;
  time(&t);   // get time now
  struct tm * now = localtime( & t );
  out << (now->tm_year + 1900) << '-'
      << setfill('0') << setw(2) << (now->tm_mon + 1) << '-'
      << setfill('0') << setw(2) << now->tm_mday << "T"
      << setfill('0') << setw(2) << now->tm_hour << ":"
      << setfill('0') << setw(2) << now->tm_min << ":"
      << setfill('0') << setw(2) << now->tm_sec;
  return out;
}
}