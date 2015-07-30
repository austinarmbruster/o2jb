#ifndef _INCLUDE_GUARD_JSON_UTIL_H_
#define _INCLUDE_GUARD_JSON_UTIL_H_ 1
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

#include "o2jb_common.h"

#include <algorithm>
#include <map>
#include <sstream>
#include <string>

#include <iostream>

namespace o2jb {

namespace json {
  
std::map<std::string, std::string> toMap(std::string const& json);

template <typename K, typename V>
class jsonify {
public:
  jsonify(std::ostream& out) : _out(out) {}

  void operator()(std::pair<K,V> const& entry) {
    std::stringstream valueStream;
    valueStream << entry.second;
    _out << "\"" << entry.first << "\" : " << "\"" << replace_all("\"", "\\\"", replace_all("\\", "\\\\", valueStream.str())) << "\", ";
  }
private:
  std::ostream& _out;
};


template <typename Iterator>
std::string toJson(Iterator begin, Iterator end) {
  typedef typename std::iterator_traits<Iterator>::value_type iter_val_t;
  typedef typename iter_val_t::first_type key_t;
  typedef typename iter_val_t::second_type value_t;

  std::stringstream rtnValue;
  rtnValue << "{ ";
  std::for_each(begin, end, jsonify<key_t, value_t>(rtnValue));
  long pos = rtnValue.tellp();
  rtnValue.seekp(pos-2);
  rtnValue << " }";
  return rtnValue.str();
}

template <typename K, typename V>
std::string toJson(std::map<K,V> const& theMap) {
  return toJson(theMap.begin(), theMap.end());
}

} // end of json namespace

} // end of o2jb namespace

#endif