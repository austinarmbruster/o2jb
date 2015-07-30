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
#include "o2jb_env.h"
#include "o2jb_common.h"

#include "RegistryKey.h"

#include <fstream>
#include <string>
#include <vector>

using std::ifstream;
using std::string;
using std::vector;

using o2jb::filesystem::exist;

namespace o2jb {
O2jbEnvHandle::O2jbEnvHandle() : diagState(o2jb::DEFAULT_STATE), connHandleMade(false), odbcVersionKnown(false),
  _pJvmMgr(NULL) {
  load_config();
}


O2jbEnvHandle::~O2jbEnvHandle() {
  if (NULL != _pJvmMgr) {
    delete _pJvmMgr;
  }
}

O2jbEnvHandle::O2jbEnvHandle(O2jbEnvHandle const& other)
  : diagState(other.diagState), connHandleMade(other.connHandleMade),
    odbcVersionKnown(other.odbcVersionKnown), _pJvmMgr(NULL) {
  attributes.insert(other.attributes.begin(), other.attributes.end());
  load_config();
}

void O2jbEnvHandle::ensureJvm() {
  if (NULL == _pJvmMgr) {
    string classPath = _properties["cp"];
    string baseCp = replace_all("<install_path>", install_path(), _properties["base.cp"]);
    if (!baseCp.empty()) {
      if (!classPath.empty()) {
        classPath += ";";
      }
      classPath += baseCp;
    }

    vector<string> jvmOptions;
    string driver = _properties["driver"];
    if (!driver.empty()) {
      jvmOptions.push_back("-Djdbc.drivers=" + driver);
    }

    properties::return_type jvmPath = _properties.getProperty("jvmPath");

    if (jvmPath.first && exist(*jvmPath.second)) {
      _pJvmMgr = new JvmManager(classPath, jvmOptions, *jvmPath.second);
    } else {
      string registryPath = JvmManager::registryPath();
      if (registryPath.empty()) {
        // Hope we have compiled for the correct default location
        _pJvmMgr = new JvmManager(classPath,  jvmOptions);
      } else {
        _pJvmMgr = new JvmManager(classPath, jvmOptions, registryPath);
      }
      _pJvmMgr->loadConfig("jvmManager.properties");
    }
  }
}

properties const& O2jbEnvHandle::config() const {
  return _properties;
}

void O2jbEnvHandle::add_config(properties::const_iterator begin, properties::const_iterator end) {
  _properties.insert(begin, end);
}

void O2jbEnvHandle::load_config() {
  ifstream currentIn("current.properties");
  currentIn >> _properties;
  currentIn.close();
}
} // end namespace