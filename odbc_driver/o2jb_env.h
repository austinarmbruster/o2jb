#ifndef _INCLUDE_GUARD_O2JB_ENV_H_
#define _INCLUDE_GUARD_O2JB_ENV_H_ 1
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

#include "JvmManager.h"
#include "properties.h"

#include <windows.h>
//include <sql.h>
#include <sqltypes.h>
//include <sqlext.h>

#include <jni.h>

#include <map>

namespace o2jb {
class O2jbEnvHandle {
public:
  // TODO use boost_typeof(sql_cp_off)
  typedef long unsigned mapped_type;
  typedef std::map<SQLINTEGER, mapped_type> attr_ctr_t;

  O2jbEnvHandle();
  ~O2jbEnvHandle();

  O2jbEnvHandle(O2jbEnvHandle&& other) = default;
  O2jbEnvHandle(O2jbEnvHandle const& other);

  properties const& config() const;
  void add_config(properties::const_iterator begin, properties::const_iterator end);

  void ensureJvm();
  inline JvmManager& jvm() {
    ensureJvm();
    return *_pJvmMgr;
  };

  inline JNIEnv * env() {
    ensureJvm();
    return NULL == _pJvmMgr ? NULL : _pJvmMgr->env();
  };

  SQLCHAR const * diagState;
  bool connHandleMade;
  bool odbcVersionKnown;
  attr_ctr_t attributes;

private:
  void load_config();

  JvmManager* _pJvmMgr;
  properties _properties;
};
}
#endif
