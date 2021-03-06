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
#include "RegistryKey.h"

#include "o2jb_logging.h"
#include "o2jb_logstream.h"

#include <winreg.h>

#include <algorithm>
#include <memory>

using std::find_if;
using std::not1;
using std::ptr_fun;

#ifndef __x86_64__
#include <iomanip>
#include <sstream>

using std::setw;
using std::stringstream;
#endif

using std::string;

namespace o2jb {
namespace {
#ifdef __x86_64__
const size_t BUFF_SIZE = 1024;
#else
const DWORD BUFF_SIZE = 1024u;
#endif

namespace {
  static inline string &ltrim(string &s) {
  s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
  return s;
}

static inline string &rtrim(string &s) {
  s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
  return s;
}

static inline string &trim(string &s) {
  return ltrim(rtrim(s));
}

}
}

#ifdef __x86_64__
string RegistryKey::SOFTWARE_BASE = "Software";
#else
string RegistryKey::SOFTWARE_BASE = "Software\\Wow6432Node";
#endif

RegistryKey::RegistryKey(HKEY const& baseKey, std::string const& subKey) throw(registry_exception) : _mode(Mode::WRITE) {
  DWORD disposition;
  LONG result = RegCreateKeyEx(baseKey, subKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkKey, &disposition);
  if (ERROR_SUCCESS != result) {
    throw registry_exception("Failed to create the " + subKey + " registry entry.");
  }
  createdKey = REG_OPENED_EXISTING_KEY != disposition;
}


RegistryKey::RegistryKey(HKEY const& baseKey, std::string const& subKey, RegistryKey::Mode const& mode) throw(registry_exception) : _mode(mode){
  if (RegistryKey::Mode::WRITE == mode) {
    DWORD disposition;
    LONG result = RegCreateKeyEx(baseKey, subKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkKey, &disposition);
    if (ERROR_SUCCESS != result) {
      throw registry_exception("Failed to create the " + subKey + " registry entry.");
    }
    createdKey = REG_OPENED_EXISTING_KEY != disposition;
  } else if (RegistryKey::Mode::READ == mode) {
    LONG result = RegOpenKeyEx(baseKey, subKey.c_str(), 0, KEY_READ, &hkKey);
    if (ERROR_SUCCESS != result) {
      throw registry_exception("Failed to open the " + subKey + " registry entry.");
    }
    createdKey = false;
  } else if (RegistryKey::Mode::APPEND == mode) {
    LONG result = RegOpenKeyEx(baseKey, subKey.c_str(), 0, KEY_ALL_ACCESS, &hkKey);
    if (ERROR_SUCCESS != result) {
      throw registry_exception("Failed to open the " + subKey + " registry entry for modification.");
    }
    createdKey = false;
  } else {
    throw registry_exception("Unsupported Mode");
  }
}

RegistryKey::~RegistryKey() {
  RegCloseKey(hkKey);
}

bool RegistryKey::isNew() const {
  return createdKey;
}

RegistryKey RegistryKey::merge(std::string const& subKey) throw(registry_exception) {
  return RegistryKey(hkKey, subKey, _mode);
}

void RegistryKey::remove(std::string const& subKey) throw(registry_exception) {
  LONG result = RegDeleteKey(hkKey, subKey.c_str());
  if (ERROR_SUCCESS != result) {
    throw registry_exception("Failed to delete the subkey:  " + subKey);
  }
}

void RegistryKey::set_value(std::string const& name, DWORD type, BYTE* value, int len) throw(registry_exception) {
  LONG result = RegSetValueEx(hkKey, name.c_str(), 0, REG_SZ, value, len);
  if (ERROR_SUCCESS != result) {
    throw registry_exception("Failed to create the value named " + name);
  }
}

void RegistryKey::unset_value(std::string const& name) throw(registry_exception) {
  LONG result = RegDeleteValue(hkKey, name.c_str());
  if (ERROR_SUCCESS != result) {
    throw registry_exception("Failed to unset the value named " + name);
  }
}

std::string RegistryKey::value(std::string const& name) const throw(registry_exception) {
#ifdef __x86_64__
  DWORD len = BUFF_SIZE;
  char value[BUFF_SIZE];
  LONG result = RegGetValue(hkKey, NULL, name.c_str(), RRF_RT_REG_SZ, NULL, value, &len);
#else
  DWORD len = BUFF_SIZE;
  LPDWORD ptrLen = & len;
  BYTE value[BUFF_SIZE];
  LONG result = RegQueryValueEx(hkKey, name.c_str(), NULL, /*REG_SZ*/ NULL, value, ptrLen);
#endif
  if (ERROR_SUCCESS != result) {
    LoggerPtr logger = Logger::getLogger("RegistryKey");
    LOG_INFO(logger, "Failed to find the value for " << name);
    return "";
  }
#ifdef __x86_64__
  return string(value, 0, len);
#else
  stringstream ss;
  ss << setw(len) << value;
  string strVal = ss.str();
  return trim(strVal);
#endif
}


RegistryKey::Mode RegistryKey::Mode::READ(1);
RegistryKey::Mode RegistryKey::Mode::WRITE(2);
RegistryKey::Mode RegistryKey::Mode::APPEND(3);
RegistryKey::Mode::Mode(int val) : _val(val) {}

bool operator==(RegistryKey::Mode const& lhs, RegistryKey::Mode const& rhs) {
  return lhs._val == rhs._val;
}

}