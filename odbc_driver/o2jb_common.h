#ifndef _INCLUDE_GUARD_O2JB_COMMON_H_
#define _INCLUDE_GUARD_O2JB_COMMON_H_ 1
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

#include <windows.h>
//include <sql.h>
#include <sqltypes.h>
//include <sqlext.h>

#include <string>

template<typename T>
void set_diag(T* const handle, SQLCHAR const * const diagState) {
  if (NULL != handle) {
    T* specificHandle = reinterpret_cast<T*>(handle);
    specificHandle->diagState = diagState;
  }
}

namespace o2jb {
extern SQLCHAR const * const DEFAULT_STATE;
std::string install_path();

bool change_to_install_dir();

std::string replace_all(std::string const& needle, std::string const& replacement, std::string const& haystack);


namespace filesystem {
bool exist(std::string const& name);
}
}

#endif