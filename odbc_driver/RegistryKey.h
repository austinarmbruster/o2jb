#ifndef _INCLUDE_GUARD_REGISTRY_KEY_H_
#define _INCLUDE_GUARD_REGISTRY_KEY_H_ 1
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

#include "registry_exception.h"

#include <windows.h>
#include <tchar.h>

#include <string>

namespace o2jb{

class RegistryKey {
public:
  static std::string SOFTWARE_BASE;

  class Mode;
  RegistryKey(HKEY const& key, std::string const& subKey) throw(registry_exception);
  RegistryKey(HKEY const& key, std::string const& subKey, Mode const&) throw(registry_exception);
  ~RegistryKey();
  RegistryKey merge(std::string const& subkey) throw(registry_exception);
  void remove(std::string const& subKey) throw(registry_exception);
  void set_value(std::string const& name, DWORD type, BYTE* value, int len) throw(registry_exception);
  void unset_value(std::string const& name) throw(registry_exception);

  std::string value(std::string const& name) const throw(registry_exception);

  bool isNew() const;

  class Mode {
  public:
    static Mode READ;
    static Mode WRITE;
    static Mode APPEND;
  private:
    Mode(int val);
    friend bool operator==(Mode const& lhs, Mode const& rhs);
    int _val;
  };

private:
  HKEY hkKey;
  bool createdKey;
  Mode _mode;
};

bool operator==(RegistryKey::Mode const& lhs, RegistryKey::Mode const& rhs);
}

#endif