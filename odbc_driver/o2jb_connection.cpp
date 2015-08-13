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
#include "o2jb_connection.h"
#include "o2jb.h"
#include "o2jb_env.h"
#include "o2jb_common.h"

#include <stdexcept>

using std::runtime_error;

namespace o2jb {
O2jbConnHandle::O2jbConnHandle(O2jbEnvHandle* envHandle) : diagState(o2jb::DEFAULT_STATE), _envHandle(envHandle) {
}

void O2jbConnHandle::populate_jvm() {
  // TODO push all of this to config
  JNIEnv* env = _envHandle->env();
  _stringCls = env->FindClass("java/lang/String");
  if (NULL == _stringCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the string class");
  }
}

O2jbConnHandle::~O2jbConnHandle() {
  // TODO close the connection
}

JvmManager& O2jbConnHandle::jvm() {
  return _envHandle->jvm();
}

JNIEnv * O2jbConnHandle::env() {
  return _envHandle->env();
}

} // end namespace