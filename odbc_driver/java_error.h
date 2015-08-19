#ifndef _INCLUDE_GUARD_JAVA_ERROR_H_
#define _INCLUDE_GUARD_JAVA_ERROR_H_ 1
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

// #include <exception>
#include <stdexcept>
#include <string>

namespace o2jb {

class java_error : public std::runtime_error {
// class java_error : public std::exception {
public:
  explicit java_error(std::string const& msgIn) : std::runtime_error(msgIn) {}
  // java_error(std::string const&) {}
  // java_error() {}
  // char const * what() const noexcept { return ""; }
};
}
#endif