#ifndef _INCLUDE_GUARD_O2JB_STATEMENT_H_
#define _INCLUDE_GUARD_O2JB_STATEMENT_H_ 1
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

#include <windows.h>
//include <sql.h>
#include <sqltypes.h>
//include <sqlext.h>

#include <jni.h>

#include <map>
#include <string>

namespace o2jb {
class BindData;

struct O2jbConnHandle;

struct O2jbStmtHandle {
public:

  typedef std::map<SQLUSMALLINT, BindData> bindings_ctr_t;

  O2jbStmtHandle(O2jbConnHandle* inConnHandle);
  ~O2jbStmtHandle();

  bool make_prepared(char const* sql);
  bool add_prepared_binding(SQLUSMALLINT const parameterNumber, BindData const& binding);

  JvmManager& jvm();
  JNIEnv * env();

  SQLCHAR const * diagState;
  std::map<std::string, std::string> properties;
  bindings_ctr_t bindings;
  jobject _stmt;
  bool _isUpdatePs;
  jobject _preparedStmt;
  bindings_ctr_t* _preparedBindingsPtr;
  jobject _resultSet;
  jobject _rsmd;
  long _numCols;
  long _numRows;
  long _lastFetchOffset;
  short _lastFetchOrientation;

  O2jbConnHandle* _connHandle;
};
}

#endif