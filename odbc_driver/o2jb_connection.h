#ifndef _INCLUDE_GUARD_O2JB_CONNECTION_H_
#define _INCLUDE_GUARD_O2JB_CONNECTION_H_ 1
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

//include "o2jb.h"

#include "JvmManager.h"

#include <windows.h>
//include <sql.h>
#include <sqltypes.h>
//include <sqlext.h>

#include <jni.h>

namespace o2jb {
class O2jbEnvHandle;

struct O2jbConnHandle {
public:
  O2jbConnHandle(O2jbEnvHandle* envHandle);
  ~O2jbConnHandle();

  void populate_jvm();

  SQLCHAR const * diagState;

  JvmManager& jvm();

  JNIEnv * env();

  jclass _stringCls;

  jobject _conn;
  jobject _ds;

  O2jbEnvHandle* _envHandle;
};
} // end namespace
#endif