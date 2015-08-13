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
#include "java_error.h"
#include "o2jb_logging.h"
#include "o2jb_logstream.h"
#include "o2jb_statement.h"
#include "o2jb_connection.h"
#include "o2jb_env.h"
#include "BindData.h"
#include "o2jb_common.h"

namespace o2jb {

namespace {
LoggerPtr logger = Logger::getLogger("o2jb");
}

O2jbStmtHandle::O2jbStmtHandle(O2jbConnHandle* inConnHandle) : diagState(o2jb::DEFAULT_STATE),
  _stmt(NULL), _isUpdatePs(false), _preparedStmt(NULL), _preparedBindingsPtr(NULL), _resultSet(NULL), _rsmd(NULL),
  _numCols(0), _numRows(-1), _lastFetchOffset(-1), _lastFetchOrientation(-1), _connHandle(inConnHandle) {
  JvmManager& jvm = this->jvm();
  _stmt = jvm.CallObjectMethod(_connHandle->_conn, "conn", "createStmt");
}

O2jbStmtHandle::~O2jbStmtHandle() {
  if (NULL != _preparedBindingsPtr) {
    delete _preparedBindingsPtr;
  }

  JvmManager& jvm = this->jvm();

  if (NULL != _resultSet) {
    try {
      jvm.CallVoidMethodA(_resultSet, "rs", "close");
    } catch (java_error& e) {
      LOG_ERROR(logger, "Failed to close the result set");
    }

    env()->DeleteLocalRef(_resultSet);
  }

  if (NULL != _stmt) {
    try {
      jvm.CallVoidMethodA(_stmt, "stmt", "close");
    } catch (java_error& e) {
      LOG_ERROR(logger, "Failed to close the statement");
    }

    env()->DeleteLocalRef(_stmt);
  }
}

JvmManager& O2jbStmtHandle::jvm() {
  return _connHandle->jvm();
}

JNIEnv * O2jbStmtHandle::env() {
  return _connHandle->env();
}

bool O2jbStmtHandle::make_prepared(char const* sql) {
  bool rtnValue = false;
  if (NULL != _preparedBindingsPtr) {
    delete _preparedBindingsPtr;
  }

  JNIEnv* env = this->env();
  JvmManager& jvm = this->jvm();

  if (NULL != _preparedStmt) {
    jvm.CallVoidMethodA(_preparedStmt, "ps", "close");
  }

  jstring jSql = env->NewStringUTF(sql);
  if (NULL != jSql) {
    _preparedStmt = jvm.CallObjectMethodA(_connHandle->_conn, "conn", "prepareStatement", make_args("L", jSql).get());
    rtnValue = true;
  }

  return rtnValue;
}

bool O2jbStmtHandle::add_prepared_binding(SQLUSMALLINT const parameterNumber, BindData const& binding) {
  bool rtnValue = false;
  if (NULL != _preparedStmt && NULL != _preparedBindingsPtr) {
    (*_preparedBindingsPtr)[parameterNumber] = binding;
    rtnValue = true;
  }
  return rtnValue;
}

} // end namespace