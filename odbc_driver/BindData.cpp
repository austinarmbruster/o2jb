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
#include "BindData.h"
#include "o2jb_statement.h"
#include "o2jb_connection.h"
#include "o2jb_common.h"

#include <sql.h>
 
#include <cmath>
#include <ostream>
#include <string>

using std::min;
using std::string;

namespace {
SQLCHAR const * const GENERAL_ERROR = reinterpret_cast<SQLCHAR const *>("HY000");
}

namespace o2jb {
BindData::BindData() : _targetType(SQL_CHAR), _targetValuePtr(NULL), _bufferLength(0), _strLenOrInd(NULL) {
}

BindData::BindData(SQLSMALLINT targetType, SQLPOINTER targetValuePtr, SQLLEN bufferLength, SQLLEN * strLenOrInd)
  : _targetType(targetType), _targetValuePtr(targetValuePtr), _bufferLength(bufferLength),
    _strLenOrInd(strLenOrInd)
{}

SQLRETURN BindData::update(O2jbStmtHandle * stmtHandle, const SQLUSMALLINT columnNumber) {
  SQLRETURN rtnValue = SQL_ERROR;
  switch (_targetType) {
  case SQL_CHAR:
  {
    JNIEnv* env = stmtHandle->env();
    JvmManager& jvm = stmtHandle->jvm();
    jstring jValue = reinterpret_cast<jstring>(jvm.CallObjectMethodA(stmtHandle->_resultSet, "rs", "str",
                     make_args("I", columnNumber).get()));
    jsize jSize = env->GetStringUTFLength(jValue);
    const char *nativeValue = env->GetStringUTFChars(jValue, 0);

    *_strLenOrInd = min(static_cast<SQLLEN>(jSize), _bufferLength);
    char* asChar = reinterpret_cast<char*>(_targetValuePtr);
    strncpy(asChar, nativeValue, *_strLenOrInd);
    env->ReleaseStringUTFChars(jValue, nativeValue);
    asChar[*_strLenOrInd] = '\0';
  }
  break;
  default:
    rtnValue = SQL_SUCCESS;
    break;
  }
  return rtnValue;
}

SQLRETURN BindData::update_param(O2jbStmtHandle * stmtHandle, const SQLUSMALLINT columnNumber) {
  SQLRETURN rtnValue = SQL_ERROR;
  switch (_targetType) {
  case SQL_CHAR:
  {
    JNIEnv* env = stmtHandle->env();
    JvmManager& jvm = stmtHandle->jvm();
    char* asChar = reinterpret_cast<char*>(_targetValuePtr);

    jstring jValue;
    if (SQL_NTS == *_strLenOrInd) {
      jValue = env->NewStringUTF(asChar);
    } else {
      string cppValue(asChar, *_strLenOrInd);
      jValue = env->NewStringUTF(cppValue.c_str());
    }
    jvm.CallVoidMethodA(stmtHandle->_preparedStmt, "ps", "str",
                        make_args("IL", columnNumber, jValue).get());
    rtnValue = SQL_SUCCESS;
  }
  break;
  default:
    rtnValue = SQL_SUCCESS;
    break;
  }
  return rtnValue;
}

std::ostream& operator<<(std::ostream& out, const BindData& binding) {
  out << "{\"targetType\":\"" << binding._targetType
      << "\", \"targetValuePtr\":\"" << binding._targetValuePtr
      << "\", \"bufferLength\":\"" << binding._bufferLength
      << "\", \"strLenOrInd\":\"" << binding._strLenOrInd << "\"}";
  return out;
}
} // end namespace