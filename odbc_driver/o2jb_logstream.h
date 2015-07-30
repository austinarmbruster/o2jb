#ifndef _INCLUDE_GUARD_O2JB_LOG_STREAM_H_
#define _INCLUDE_GUARD_O2JB_LOG_STREAM_H_ 1
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

#include <ostream>
#include <string>
#include <vector>

namespace o2jb {

class LogStream : public std::streambuf {
public:
  LogStream(std::ostream& sink, std::size_t buff_sz = 256);
  LogStream(std::string const& sink, std::size_t buff_sz = 256);
  ~LogStream();

protected:
  bool doOutput();
  virtual int sync();

private:
  //copying not allowed.
  LogStream(const LogStream &) {};
  LogStream &operator=(const LogStream &) { return *this; };

  std::ostream& stream();
  std::ostream* _theStream;
  bool _ownTheStream;
  std::vector<char> _buffer;
};
}

#endif
