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
#include "o2jb_local_ref.h"

namespace o2jb {
local_ref::local_ref(JvmManager& jvm, jobject obj) : _jvm(jvm), _obj(obj) {}

local_ref::~local_ref() {
  this->release();
}

jobject local_ref::get() {
  return _obj;
}

void local_ref::release() {
  if (NULL != _obj) {
    _jvm.DeleteLocalRef(_obj);
  }
  _obj = NULL;
}

} // end of namespace