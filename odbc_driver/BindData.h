#ifndef _INCLUDE_GUARD_BIND_DATA_H_
#define _INCLUDE_GUARD_BIND_DATA_H_ 1
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

#include <windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include <iosfwd>

namespace o2jb {
struct O2jbStmtHandle;

class BindData {
public:
  BindData();

  BindData(SQLSMALLINT targetType, SQLPOINTER targetValuePtr, SQLLEN bufferLength, SQLLEN * strLenOrInd);

  SQLRETURN update(O2jbStmtHandle * stmtHandle, const SQLUSMALLINT columnNumber);
  SQLRETURN update_param(O2jbStmtHandle * stmtHandle, const SQLUSMALLINT columnNumber);

  // TODO Address move semantics
  friend std::ostream& operator<<(std::ostream& out, const BindData& binding);
private:
  // TODO consider the pimpl idiom
  SQLSMALLINT    _targetType;
  SQLPOINTER     _targetValuePtr;
  SQLLEN         _bufferLength;
  SQLLEN *       _strLenOrInd;
};
}
#endif