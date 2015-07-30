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
  _dsCls = env->FindClass("org/apache/commons/dbcp/BasicDataSource");
  if (NULL == _dsCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the data source class");
  }

  _dmCls = env->FindClass("java/sql/DriverManager");
  if (NULL == _dmCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the driver manager class");
  }

  _stringCls = env->FindClass("java/lang/String");
  if (NULL == _stringCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the string class");
  }

  _clsCls = env->FindClass("java/lang/Class");
  if (NULL == _clsCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the Class class");
  }

  _connCls = env->FindClass("java/sql/Connection");
  if (NULL == _connCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the connection class");
  }

  _stmtCls = env->FindClass("java/sql/Statement");
  if (NULL == _stmtCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the statement class");
  }

  _rsCls = env->FindClass("java/sql/ResultSet");
  if (NULL == _rsCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the result set class");
  }

  _rsmdCls = env->FindClass("java/sql/ResultSetMetaData");
  if (NULL == _rsmdCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the result set metadata class");
  }

  _dbmdCls = env->FindClass("java/sql/DatabaseMetaData");
  if (NULL == _dbmdCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the database metadata class");
  }

  _preparedStmtCls = env->FindClass("java/sql/PreparedStatement");
  if (NULL == _preparedStmtCls || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the prepared statement class");
  }

  _createDsMid = env->GetMethodID(_dsCls, "<init>", "()V");
  if (NULL == _createDsMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the basic data source constructor method");
  }


  // _forNameMid = env->GetMethodID(_clsCls, "forName", "(Ljava/lang/String;)Ljava/lang/Class;");
  // if (NULL == _forNameMid || env->ExceptionCheck()) {
  //   env->ExceptionDescribe();
  //   throw runtime_error("Could not find the for name method");
  // }

  // _getConnUrlOnlyMid = env->GetMethodID(_dmCls, "getConnection", "(Ljava/lang/String;)Ljava/sql/Connection;");
  // if (NULL == _getConnUrlOnlyMid || env->ExceptionCheck()) {
  //   env->ExceptionDescribe();
  //   throw runtime_error("Could not find the get connection method");
  // }

  // _getConnUPMid = env->GetMethodID(_dmCls, "getConnection", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/sql/Connection;");
  // if (NULL == _getConnUPMid || env->ExceptionCheck()) {
  //   env->ExceptionDescribe();
  //   throw runtime_error("Could not find the get connection with user and password method");
  // }

  // _getConnIMid = env->GetMethodID(_dmCls, "getConnection", "(Ljava/lang/String;Ljava/util/Properties)Ljava/sql/Connection;");
  // if (NULL == _getConnIMid || env->ExceptionCheck()) {
  //   env->ExceptionDescribe();
  //   throw runtime_error("Could not find the get connection with info method");
  // }

  _setDriverMid = env->GetMethodID(_dsCls, "setDriverClassName", "(Ljava/lang/String;)V");
  if (NULL == _setDriverMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the set driver method");
  }

  _setUrlMid = env->GetMethodID(_dsCls, "setUrl", "(Ljava/lang/String;)V");
  if (NULL == _setUrlMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the set url method");
  }

  _setUserMid = env->GetMethodID(_dsCls, "setUsername", "(Ljava/lang/String;)V");
  if (NULL == _setUserMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the set user method");
  }

  _setPwdMid = env->GetMethodID(_dsCls, "setPassword", "(Ljava/lang/String;)V");
  if (NULL == _setPwdMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the set password method");
  }

  _getConnMid = env->GetMethodID(_dsCls, "getConnection", "()Ljava/sql/Connection;");
  if (NULL == _getConnMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the get connection method");
  }

  _createStmtMid = env->GetMethodID(_connCls, "createStatement", "()Ljava/sql/Statement;");
  if (NULL == _createStmtMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the createStatement method on connection");
  }

  _prepStmtMid = env->GetMethodID(_connCls, "prepareStatement", "(Ljava/lang/String;)Ljava/sql/PreparedStatement;");
  if (NULL == _prepStmtMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the preparedStatement method on connection");
  }

  _getDbmdMid = env->GetMethodID(_connCls, "getMetaData", "()Ljava/sql/DatabaseMetaData;");
  if (NULL == _getDbmdMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the getMetaData method on connection");
  }

  _execUpdateMid = env->GetMethodID(_stmtCls, "executeUpdate", "(Ljava/lang/String;)I");
  if (NULL == _execUpdateMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the executeUpdate method on statements");
  }

  _execQueryMid = env->GetMethodID(_stmtCls, "executeQuery", "(Ljava/lang/String;)Ljava/sql/ResultSet;");
  if (NULL == _execQueryMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the executeQuery method on statements");
  }

  _nextMid = env->GetMethodID(_rsCls, "next", "()Z");
  if (NULL == _nextMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the next method on result set");
  }

  _strMid = env->GetMethodID(_rsCls, "getString", "(I)Ljava/lang/String;");
  if (NULL == _strMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the getString method on result set");
  }

  _metaDataMid = env->GetMethodID(_rsCls, "getMetaData", "()Ljava/sql/ResultSetMetaData;");
  if (NULL == _metaDataMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the getMetaData method on result set");
  }

  _numColsMid = env->GetMethodID(_rsmdCls, "getColumnCount", "()I");
  if (NULL == _numColsMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the getColumnCount method on result set metadata");
  }

  _colDisplaySizeMid = env->GetMethodID(_rsmdCls, "getColumnDisplaySize", "(I)I");
  if (NULL == _colDisplaySizeMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the getColumnDisplaySize method on result set metadata");
  }

  _colType = env->GetMethodID(_rsmdCls, "getColumnType", "(I)I");
  if (NULL == _colType || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the getColumnType method on result set metadata");
  }

  _colName = env->GetMethodID(_rsmdCls, "getColumnName", "(I)Ljava/lang/String;");
  if (NULL == _colName || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the getColumnName method on result set metadata");
  }

  _psCloseMid = env->GetMethodID(_preparedStmtCls, "close", "()V");
  if (NULL == _psCloseMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the close method on prepared statement");
  }

  _psStrMid = env->GetMethodID(_preparedStmtCls, "setString", "(ILjava/lang/String;)V");
  if (NULL == _psStrMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the getColumnName method on prepared statement");
  }

  _psExecUpdateMid = env->GetMethodID(_preparedStmtCls, "executeUpdate", "()I");
  if (NULL == _psExecUpdateMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the executeUpdate method on prepared statements");
  }

  _psExecQueryMid = env->GetMethodID(_preparedStmtCls, "executeQuery", "()Ljava/sql/ResultSet;");
  if (NULL == _psExecQueryMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the executeQuery method on prepared statements");
  }

  _getTablesMid = env->GetMethodID(_dbmdCls, "getTables",
                                   "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;"
                                   "[Ljava/lang/String;)Ljava/sql/ResultSet;");
  if (NULL == _psExecQueryMid || env->ExceptionCheck()) {
    env->ExceptionDescribe();
    throw runtime_error("Could not find the executeQuery method on prepared statements");
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