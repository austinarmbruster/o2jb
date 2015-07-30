#ifndef _INCLUDE_GUARD_JCLASS_METDATA_H_
#define _INCLUDE_GUARD_JCLASS_METDATA_H_ 1
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

#include <jni.h>

#include <iosfwd>
#include <map>
#include <string>

namespace o2jb {

class jclass_metadata {
public:
  jclass_metadata(jclass theClass = NULL);

  jmethodID& operator[](std::string const& methodName);

  ///
  /// Allows for getting the method from a const object.
  /// @return a pointer to the method if it was previously added via operator[]; otherwise NULL.
  jmethodID const * get(std::string const& methodName) const;

  bool contains(std::string const& methodName) const;

  void add_static(std::string const& methodName, jmethodID method);
  jmethodID get_static(std::string const& methodName);
  bool contains_static(std::string const& methodName) const;

  void add_constructor(std::string const& methodName, jmethodID method);
  jmethodID get_constructor(std::string const& methodName);
  bool contains_constructor(std::string const& methodName) const;

  jclass theClass();

  friend std::ostream& operator<<(std::ostream& out, jclass_metadata const& md);
private:
  typedef std::map<std::string, jmethodID> method_ctr_t;

  jclass _theClass;
  method_ctr_t _methods;
  method_ctr_t _staticMethods;
  method_ctr_t _initMethods;
};

std::ostream& operator<<(std::ostream& out, jclass_metadata const& md);

}
#endif