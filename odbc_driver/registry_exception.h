#ifndef _INCLUDE_GUARD_REGISTRY_EXCEPTION_H_
#define _INCLUDE_GUARD_REGISTRY_EXCEPTION_H_ 1
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

#include <stdexcept>

namespace o2jb {
class registry_exception : public std::runtime_error {
public:
  registry_exception(std::string const& message) : std::runtime_error(message) {}
};
}

#endif