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
#include "jclass_metadata.h"

#include <algorithm>
#include <functional>
#include <ostream>

using std::for_each;
using std::make_pair;
using std::ostream;
using std::string;

using std::placeholders::_1;

namespace o2jb {
jclass_metadata::jclass_metadata(jclass theClass) : _theClass(theClass) {}

jmethodID& jclass_metadata::operator[](string const& methodName) {
  return _methods[methodName];
}

jmethodID const * jclass_metadata::get(string const& methodName) const {
  method_ctr_t::const_iterator iter = _methods.find(methodName);
  if (_methods.end() == iter) {
    return NULL;
  } else {
    return &(iter->second);
  }
}

bool jclass_metadata::contains(string const& methodName) const {
  method_ctr_t::const_iterator iter = _methods.find(methodName);
  return _methods.end() != iter;
}

jclass jclass_metadata::theClass() {
  return _theClass;
}

void jclass_metadata::add_static(std::string const& methodName, jmethodID method) {
  _staticMethods.insert(make_pair(methodName, method));
}

jmethodID jclass_metadata::get_static(std::string const& methodName) {
  return _staticMethods[methodName];
}

bool jclass_metadata::contains_static(std::string const& methodName) const {
  method_ctr_t::const_iterator iter = _staticMethods.find(methodName);
  return _staticMethods.end() != iter;
}

void jclass_metadata::add_constructor(std::string const& methodName, jmethodID method) {
  _initMethods.insert(make_pair(methodName, method));
}

jmethodID jclass_metadata::get_constructor(std::string const& methodName) {
  return _initMethods[methodName];
}

bool jclass_metadata::contains_constructor(std::string const& methodName) const {
  method_ctr_t::const_iterator iter = _initMethods.find(methodName);
  return _initMethods.end() != iter;
}

template <typename I>
void print_keys(ostream& out, I begin, I end, string sep = ",") {
  bool needSep;
  for (I i = begin; i != end; ++i) {
    if (needSep) {
      out << sep;
    }
    out << i->first;
    needSep = true;
  }
}

ostream& operator<<(ostream& out, jclass_metadata const& md) {
  out << "{ \"methods\" : [\"";
  print_keys(out, md._methods.begin(), md._methods.end(), "\",\"");
  out << "\" ], \"staticMethods\" : [ ";
  print_keys(out, md._staticMethods.begin(), md._staticMethods.end(), "\",\"");
  out << "\" ], \"initMethods\" : [ ";
  print_keys(out, md._initMethods.begin(), md._initMethods.end(), "\",\"");
  out << "\" ] }";
  return out;
}

}