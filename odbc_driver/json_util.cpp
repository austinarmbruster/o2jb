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
#include "json_util.h"

#include <cctype>

using std::isspace;
using std::map;
using std::string;
using std::stringstream;

namespace o2jb {

namespace json {

namespace {
const char OPEN_BRACKET = '{';
const char COLON = ':';
const char COMMA = ',';
const char QUOTE = '\"';
const char CLOSE_BRACKET = '}';
const char BKSLASH = '\\';

bool expect_more(std::string const& value) {
  bool rtnValue = false;
  if (!value.empty()) {
    rtnValue = ((value.size() > 1) && (BKSLASH == value[value.size() - 1]) && (BKSLASH != value[value.size() - 2]))
               || ((value.size() == 1) && (BKSLASH == value[0]));
  }
  return rtnValue;
}

} // end of anonymous namespace

std::map<std::string, std::string> toMap(std::string const& json) {
  map<string, string> rtnValue;
  stringstream ss(json);
  string temp;
  string key;
  string value;

  if (getline(ss, temp, OPEN_BRACKET) && getline(ss, temp, QUOTE)) {
    bool theEnd = false;
    while (!theEnd && getline(ss, key, QUOTE) && getline(ss, temp, COLON) && getline(ss, temp, QUOTE)
           && getline(ss, value, QUOTE)) {
      bool hadEscapes = false;
      while (expect_more(value) && getline(ss, temp, QUOTE)) {
        value += QUOTE;
        value += temp;
        hadEscapes = true;
      }

      if (hadEscapes) {
        value = replace_all("\\\"", "\"", value);
      }

      rtnValue.insert(make_pair(key, value));

      int c = ss.peek();
      while ((EOF != c) && (isspace(c) || (QUOTE == c) || (COMMA == c) || (CLOSE_BRACKET == c))) {
        ss.get();
        c = ss.peek();
      }
      theEnd = EOF == c;
    }
  }
  return rtnValue;
}

} // end of json namespace

} // end of o2jb namespace