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
#include "o2jb.h"

#include "o2jb_env.h"
#include "o2jb_connection.h"
#include "o2jb_statement.h"
#include "BindData.h"
#include "o2jb_common.h"
#include "o2jb_local_ref.h"
#include "o2jb_logging.h"
#include "o2jb_logstream.h"
#include "java_error.h"

#include "properties.h"
#include "RegistryKey.h"
 
#include "o2jb_sql.h"

#include <jni.h>

#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <cstring>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <iostream>
using std::endl; using std::cout;

using std::exception;
using std::ifstream;
using std::inserter;
using std::iterator;
using std::make_pair;
using std::map;
using std::min;
using std::pair;
using std::runtime_error;
using std::set;
using std::string;
using std::strncpy;
using std::transform;
using std::unique_ptr;
using std::vector;

using o2jb::java_error;
using o2jb::local_ref;
using o2jb::properties;
using o2jb::BindData;
using o2jb::JvmManager;
using o2jb::LoggerPtr;
using o2jb::RegistryKey;
using o2jb::Logger;
using o2jb::O2jbEnvHandle;
using o2jb::O2jbConnHandle;
using o2jb::O2jbStmtHandle;
using o2jb::make_args;

#define JVM_CALL(expression) expression; if (jvm.exception_cleared()) { return SQL_ERROR; }
#define UNUSED(expression) (void)(expression)

// TODO add memory checks to the current logic - try {} catch (std::bad_alloc&)
// {release a  pre-allocated chunk of memory to account for memory burned in reporting no memory}
namespace {
void Tokenize(const string& str,
              vector<string>& tokens,
              const string& delimiters = " ")
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

class spliter {
public:
  spliter(string const& delimiters) : _delimiters(delimiters) {}
  pair<string, string> operator()(string const& val) {
    string::size_type pos = val.find_first_of(_delimiters, 0);
    if (string::npos == pos) {
      return make_pair(val, "");
    } else {
      return make_pair(val.substr(0, pos), val.substr(pos + 1));
    }
  }
private:
  string _delimiters;
};

map<string, string> parseConnectionString(char * inConnectionString, SQLSMALLINT strLen) {
  string input = SQL_NTS == strLen ? string(inConnectionString) : string(inConnectionString, strLen);
  vector<string> tokens;
  Tokenize(input, tokens, ";");
  map<string, string> rtnValue;
  transform(tokens.begin(), tokens.end(), inserter(rtnValue, rtnValue.begin()), spliter("="));
  return rtnValue;
}

jstring toJString(JNIEnv* const env, char const * const asChar, SQLLEN const strLen) {
  jstring jValue = NULL;
  if (NULL != asChar) {
    if (SQL_NTS == strLen) {
      jValue = env->NewStringUTF(asChar);
    } else {
      string cppValue(asChar, strLen);
      jValue = env->NewStringUTF(cppValue.c_str());
    }
  }
  return jValue;
}

string toCppString(char const * const asChar, SQLLEN const strLen) {
  if (NULL != asChar) {
    if (SQL_NTS == strLen) {
      return string(asChar);
    } else {
      string cppValue(asChar, strLen);
      return cppValue;
    }
  } else {
    return string("NULL");
  }
}

static SQLCHAR const * const GENERAL_ERROR     = reinterpret_cast<SQLCHAR const *>("HY000");
static SQLCHAR const * const INVALID_NULL_PTR  = reinterpret_cast<SQLCHAR const *>("HY009");
static SQLCHAR const * const INVALID_ATTR      = reinterpret_cast<SQLCHAR const *>("HY092");

O2jbEnvHandle::attr_ctr_t::mapped_type processConnPoolAttr = SQL_CP_OFF;


// TODO: make multithreaded
typedef set<O2jbEnvHandle*> env_ctr_t;
typedef set<O2jbConnHandle*> conn_ctr_t;
typedef set<O2jbStmtHandle*> stmt_ctr_t;

env_ctr_t environments;
conn_ctr_t connections;
stmt_ctr_t statements;
LoggerPtr logger = Logger::getLogger("o2jb");

} // end of anonymous namespace

SQLRETURN ADD_CALL SQLAllocHandle(SQLSMALLINT handleType, SQLHANDLE inputHandle, SQLHANDLE *outHandle) {
  cout << "in alloc handle:  " << inputHandle << endl;
  cout << "ht:  " << handleType << " " << SQL_HANDLE_ENV << " " << SQL_HANDLE_DBC << " " << SQL_HANDLE_STMT << endl;
  SQLRETURN rtnValue = SQL_ERROR;
  // TODO put the created handle into a temporary variable
  try {
    switch (handleType) {
    case SQL_HANDLE_ENV:
    {
      O2jbEnvHandle *envHandle = new O2jbEnvHandle();
      *outHandle = envHandle;
      environments.insert(envHandle);
      rtnValue = SQL_SUCCESS;
    }
    break;
    case SQL_HANDLE_DBC:
    {
      O2jbEnvHandle* envHandle = static_cast<O2jbEnvHandle*>(inputHandle);

      env_ctr_t::iterator envIter = environments.find(envHandle);
      if (envIter != environments.end() && *envIter == envHandle) {
        O2jbConnHandle* connHandle = new O2jbConnHandle(envHandle);
        *outHandle = connHandle;
        connections.insert(connHandle);

        (*envIter)->connHandleMade = true;

        rtnValue = SQL_SUCCESS;
      } else {
        rtnValue = SQL_INVALID_HANDLE;
      }
    }
    break;
    case SQL_HANDLE_STMT:
      cout << "create stmt" << endl;
      {
        O2jbConnHandle* connHandle = static_cast<O2jbConnHandle*>(inputHandle);
        cout << "s1" << endl;
        conn_ctr_t::iterator connIter = connections.find(connHandle);
        cout << "s2" << endl;
        if (connIter != connections.end() && *connIter == connHandle) {
          cout << "s3" << endl;
          O2jbStmtHandle* stmtHandle = new O2jbStmtHandle(connHandle);
          cout << "stmtHandle:  " << stmtHandle << endl;
          *outHandle = stmtHandle;
          cout << "s4" << endl;
          statements.insert(stmtHandle);
          cout << "s5" << endl;
          rtnValue = SQL_SUCCESS;
          cout << "s6" << endl;
        } else {
          cout << "s7" << endl;
          rtnValue = SQL_INVALID_HANDLE;
          cout << "s8" << endl;
        }
      }
      break;

    default:
      rtnValue = SQL_INVALID_HANDLE;
      break;
    }
    // TODO update the outHandle with the temporary variable.
  } catch (exception& e) {
    LOG_ERROR(logger, "Trapped an exception while creating the handle:  " << e.what());
  }

  return rtnValue;
}

SQLRETURN ADD_CALL SQLBindCol(
  SQLHSTMT       stmtHandle,
  SQLUSMALLINT   columnNumber,
  SQLSMALLINT    targetType,
  SQLPOINTER     targetValuePtr,
  SQLLEN         bufferLength,
  SQLLEN *       strLenOrInd) {
  SQLRETURN rtnValue = SQL_ERROR;

  stmt_ctr_t::iterator iter = statements.find(reinterpret_cast<O2jbStmtHandle*>(stmtHandle));
  if (statements.end() != iter) {
    rtnValue = SQL_SUCCESS;

    BindData binding(targetType, targetValuePtr, bufferLength, strLenOrInd);
    (*iter)->bindings[columnNumber] = binding;
  } else {
    rtnValue = SQL_INVALID_HANDLE;
  }
  return rtnValue;
}

SQLRETURN ADD_CALL SQLBindParameter(
  SQLHSTMT        stmtHandle,
  SQLUSMALLINT    parameterNumber,
  SQLSMALLINT     inputOutputType,
  SQLSMALLINT     valueType,
  SQLSMALLINT     parameterType,
  SQLULEN         columnSize,
  SQLSMALLINT     decimalDigits,
  SQLPOINTER      parameterValuePtr,
  SQLLEN          bufferLength,
  SQLLEN *        strLenOrInd) {
  UNUSED(parameterType);
  UNUSED(columnSize);
  UNUSED(decimalDigits);

  SQLRETURN rtnValue = SQL_ERROR;
  if (NULL == stmtHandle) {
    rtnValue = SQL_INVALID_HANDLE;
  } else {
    O2jbStmtHandle* specificHandle = reinterpret_cast<O2jbStmtHandle*>(stmtHandle);

    stmt_ctr_t::iterator iter = statements.find(specificHandle);
    if (statements.end() == iter) {
      rtnValue = SQL_INVALID_HANDLE;
    } else {
      if (NULL == specificHandle->_preparedStmt) {
        set_diag(specificHandle, GENERAL_ERROR);
        rtnValue = SQL_ERROR;
      } else {
        switch (valueType) {
        case SQL_C_CHAR: {
          BindData binding(inputOutputType, parameterValuePtr, bufferLength, strLenOrInd);
          bool goodAdd = specificHandle->add_prepared_binding(parameterNumber, binding);
          if (goodAdd) {
            rtnValue = SQL_SUCCESS;
          } else {
            set_diag(specificHandle, GENERAL_ERROR);
            rtnValue = SQL_ERROR;
          }
        }
        break;
        default:
          LOG_ERROR(logger, "[SQLBindParameter] Attempted to bind data of an unknown type:  " << valueType << ".");
          break;
        }
      }
    }
  }

  return rtnValue;
}

SQLRETURN ADD_CALL SQLBrowseConnect(
  SQLHDBC         ConnectionHandle,
  SQLCHAR *       InConnectionString,
  SQLSMALLINT     StringLength1,
  SQLCHAR *       OutConnectionString,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLength2Ptr) {
  UNUSED(ConnectionHandle);
  UNUSED(InConnectionString);
  UNUSED(StringLength1);
  UNUSED(OutConnectionString);
  UNUSED(BufferLength);
  UNUSED(StringLength2Ptr);

  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLCancel(
  SQLHSTMT     StatementHandle) {
  UNUSED(StatementHandle);

  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLCloseCursor(
  SQLHSTMT     StatementHandle) {
  UNUSED(StatementHandle);

  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

#ifdef __x86_64__
SQLRETURN ADD_CALL SQLColAttribute (
  SQLHSTMT        stmtHandle,
  SQLUSMALLINT    columnNumber,
  SQLUSMALLINT    fieldIdentifier,
  SQLPOINTER      characterAttributePtr,
  SQLSMALLINT     bufferLength,
  SQLSMALLINT *   stringLengthPtr,
  SQLLEN *        NumericAttributePtr) {
#else
SQLRETURN ADD_CALL SQLColAttribute (
  SQLHSTMT        stmtHandle,
  SQLUSMALLINT    columnNumber,
  SQLUSMALLINT    fieldIdentifier,
  SQLPOINTER      characterAttributePtr,
  SQLSMALLINT     bufferLength,
  SQLSMALLINT *   stringLengthPtr,
  SQLPOINTER        NumericAttributePtr) {
#endif
  SQLRETURN rtnValue = SQL_ERROR;
  bufferLength = 0;

  if (NULL != stmtHandle) {
    O2jbStmtHandle* specificHandle = reinterpret_cast<O2jbStmtHandle*>(stmtHandle);
    stmt_ctr_t::iterator iter = statements.find(specificHandle);
    if (statements.end() == iter) {
      rtnValue = SQL_INVALID_HANDLE;
    } else {
      jobject& rsmd = specificHandle->_rsmd;
      jint jColNumber = columnNumber;
      JNIEnv* env = specificHandle->env();
      JvmManager& jvm = specificHandle->jvm();

      switch (fieldIdentifier) {
      case SQL_DESC_DISPLAY_SIZE:
      {
        JVM_CALL(jint displaySize = jvm.CallIntMethodA(rsmd, "rsmd", "colDisplaySize",
                                    make_args("I", jColNumber).get()));

        if (255 == displaySize) {
          displaySize = 50;
        }
        *(reinterpret_cast<SQLLEN *>(NumericAttributePtr)) = displaySize;
        rtnValue = SQL_SUCCESS;
      }
      break;
      case SQL_DESC_CONCISE_TYPE:
      {
        // TODO Translate the java types to the SQL types
        // jint javaType = env->CallIntMethod(rsmd, specificHandle->_connHandle->_colType, jColNumber);
        // switch...
        // Assume text for now
        *(reinterpret_cast<SQLLEN *>(NumericAttributePtr)) = SQL_CHAR;
        rtnValue = SQL_SUCCESS;
      }
      break;
      case SQL_DESC_NAME:
      {
        JVM_CALL(jobject colNameObj = jvm.CallObjectMethodA(rsmd, "rsmd", "colName",
                                      make_args("I", jColNumber).get()));

        jstring colNameStr = reinterpret_cast<jstring>(colNameObj);

        jsize len = env->GetStringLength(colNameStr);
        if (JNI_FALSE == env->ExceptionCheck()) {
          *stringLengthPtr = len;
          bufferLength = abs(bufferLength);
          if (NULL != characterAttributePtr /* && bufferLength > 0*/) {
            int copyLen = len; /* min(static_cast<int>(len), bufferLength-1);*/
            char* dest = reinterpret_cast<char*>(characterAttributePtr);

            const char *nativeString = env->GetStringUTFChars(colNameStr, 0);
            if (JNI_FALSE == env->ExceptionCheck()) {
              strncpy(dest, nativeString, copyLen);
              dest[copyLen] = '\0';
              env->ReleaseStringUTFChars(colNameStr, nativeString);
              rtnValue = SQL_SUCCESS;
            }
          } else {
            rtnValue = SQL_SUCCESS;
          }
        }

        env->DeleteLocalRef(colNameObj);
      }
      break;
      default:
        *(reinterpret_cast<SQLLEN *>(NumericAttributePtr)) = 0;
      }

      if (JNI_TRUE == env->ExceptionCheck()) {
        set_diag(specificHandle, GENERAL_ERROR);
        env->ExceptionDescribe();
        env->ExceptionClear();

        rtnValue = SQL_ERROR;
      }
    }
  }

  return rtnValue;
}

SQLRETURN ADD_CALL SQLColumnPrivileges(
  SQLHSTMT      StatementHandle,
  SQLCHAR *     CatalogName,
  SQLSMALLINT   NameLength1,
  SQLCHAR *     SchemaName,
  SQLSMALLINT   NameLength2,
  SQLCHAR *     TableName,
  SQLSMALLINT   NameLength3,
  SQLCHAR *     ColumnName,
  SQLSMALLINT   NameLength4) {
  UNUSED(StatementHandle);
  UNUSED(CatalogName);
  UNUSED(NameLength1);
  UNUSED(SchemaName);
  UNUSED(NameLength2);
  UNUSED(TableName);
  UNUSED(NameLength3);
  UNUSED(ColumnName);
  UNUSED(NameLength4);

  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN SQLColumns(
  SQLHSTMT       StatementHandle,
  SQLCHAR *      CatalogName,
  SQLSMALLINT    NameLength1,
  SQLCHAR *      SchemaName,
  SQLSMALLINT    NameLength2,
  SQLCHAR *      TableName,
  SQLSMALLINT    NameLength3,
  SQLCHAR *      ColumnName,
  SQLSMALLINT    NameLength4) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLConnect(
  SQLHDBC        ConnectionHandle,
  SQLCHAR *      ServerName,
  SQLSMALLINT    NameLength1,
  SQLCHAR *      UserName,
  SQLSMALLINT    NameLength2,
  SQLCHAR *      Authentication,
  SQLSMALLINT    NameLength3) {
  return SQL_SUCCESS;
}

SQLRETURN ADD_CALL SQLConnect(
  SQLHDBC        ConnectionHandle) {
  return SQL_SUCCESS;
}

SQLRETURN ADD_CALL SQLCopyDesc(
  SQLHDESC     SourceDescHandle,
  SQLHDESC     TargetDescHandle) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLDataSources(
  SQLHENV          EnvironmentHandle,
  SQLUSMALLINT     Direction,
  SQLCHAR *        ServerName,
  SQLSMALLINT      BufferLength1,
  SQLSMALLINT *    NameLength1Ptr,
  SQLCHAR *        Description,
  SQLSMALLINT      BufferLength2,
  SQLSMALLINT *    NameLength2Ptr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLDescribeCol(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   columnNumber,
  SQLCHAR *      ColumnName,
  SQLSMALLINT    BufferLength,
  SQLSMALLINT *  NameLengthPtr,
  SQLSMALLINT *  DataTypePtr,
  SQLULEN *      ColumnSizePtr,
  SQLSMALLINT *  DecimalDigitsPtr,
  SQLSMALLINT *  NullablePtr) {
  return columnNumber < 3 ? SQL_SUCCESS : SQL_SUCCESS + 1;
}

SQLRETURN ADD_CALL SQLDescribeParam(
  SQLHSTMT        StatementHandle,
  SQLUSMALLINT    ParameterNumber,
  SQLSMALLINT *   DataTypePtr,
  SQLULEN *       ParameterSizePtr,
  SQLSMALLINT *   DecimalDigitsPtr,
  SQLSMALLINT *   NullablePtr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLDisconnect(
  SQLHDBC        ConnectionHandle) {
  return SQL_SUCCESS;
}

SQLRETURN ADD_CALL SQLDriverConnect(
  SQLHDBC         connHandle,
  SQLHWND         windowHandle,
  SQLCHAR *       inConnectionString,
  SQLSMALLINT     stringLength1,
  SQLCHAR *       outConnectionString,
  SQLSMALLINT     bufferLength,
  SQLSMALLINT *   stringLength2Ptr,
  SQLUSMALLINT    driverCompletion) {
  cout << "here" << endl;
  SQLRETURN rtnValue = SQL_ERROR;
  O2jbConnHandle* specificHandle = reinterpret_cast<O2jbConnHandle*>(connHandle);
  conn_ctr_t::iterator iter = connections.find(specificHandle);
  if (connections.end() == iter) {
    rtnValue = SQL_INVALID_HANDLE;
  } else {
    map<string, string> connectionValues = parseConnectionString(reinterpret_cast<char*>(inConnectionString),
                                           stringLength1);
    LOG_DEBUG(logger, "dsn: " << connectionValues["DSN"]);

    properties dsnProps;
    RegistryKey driverReg(HKEY_CURRENT_USER, "Software\\ODBC\\ODBC.INI\\" + connectionValues["DSN"]);
    dsnProps["cp"] = driverReg.value("cp");
    dsnProps["driver"] = driverReg.value("jdbcDriver");
    dsnProps["url"] = driverReg.value("url");
    dsnProps["user"] = driverReg.value("user");
    dsnProps["password"] = driverReg.value("password");
    cout << "dsn props:  " << dsnProps << endl;
    O2jbConnHandle& conn = *specificHandle;
    try {
      conn._envHandle->add_config(dsnProps.begin(), dsnProps.end());

      conn.populate_jvm();
      JNIEnv* env = conn.env();
      JvmManager& jvm = conn.jvm();

      string cDriver = conn._envHandle->config()["driver"];
      jstring driver = env->NewStringUTF(cDriver.c_str());

      string cUrl = conn._envHandle->config()["url"];
      jstring url = env->NewStringUTF(cUrl.c_str());

      string cUser = conn._envHandle->config()["user"];
      jstring user = env->NewStringUTF(cUser.c_str());

      string cPwd = conn._envHandle->config()["password"];
      jstring pwd = env->NewStringUTF(cPwd.c_str());

      conn._ds = jvm.NewObjectA("ds", "defCon");
      jvm.CallObjectMethodA(conn._ds, "ds", "setDriver", make_args("l", driver).get());
      jvm.CallObjectMethodA(conn._ds, "ds", "setUrl", make_args("l", url).get());
      if (!cUser.empty()) {
        jvm.CallVoidMethodA(conn._ds, "ds", "setUser", make_args("l", user).get());
      }
      if (!cPwd.empty()) {
        jvm.CallVoidMethodA(conn._ds, "ds", "setPwd", make_args("l", pwd).get());
      }

      conn._conn = jvm.CallObjectMethodA(conn._ds, "ds", "getConn");
      rtnValue = SQL_SUCCESS;
    } catch (java_error& e) {
      LOG_ERROR(logger, "Failed to connect:  " << e.what());
      set_diag(&conn, GENERAL_ERROR);
      rtnValue = SQL_ERROR;
    }
  }
  return rtnValue;
}

SQLRETURN ADD_CALL SQLEndTran(
  SQLSMALLINT   HandleType,
  SQLHANDLE     Handle,
  SQLSMALLINT   CompletionType) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

bool determine_sql_type(SQLCHAR *stmtText, SQLINTEGER textLength, JvmManager& jvm, jstring* jSqlPtr) {
  char* cStr = reinterpret_cast<char*>(stmtText);
  size_t sqlLen = textLength;
  if (SQL_NTS == textLength) {
    sqlLen = strlen(cStr);
  }

  string sql(cStr, sqlLen);
  transform(sql.begin(), sql.end(), sql.begin(), ::tolower);
  size_t selectLoc = sql.find("select");

  *jSqlPtr = jvm.toJString(cStr);
  bool isUpdate = (string::npos == selectLoc) || (selectLoc > 14);
  return isUpdate;
}

void loadMd(O2jbStmtHandle* specificHandle, JvmManager& jvm) {
  specificHandle->_rsmd = jvm.CallObjectMethodA(specificHandle->_resultSet, "rs", "metaData");
  jint providedNumCols = jvm.CallIntMethodA(specificHandle->_rsmd, "rsmd", "numCols");
  specificHandle->_numCols = reinterpret_cast<long>(providedNumCols);
  specificHandle->_numRows = -1;
}

SQLRETURN ADD_CALL doUpdate(O2jbStmtHandle* specificHandle, JvmManager& jvm, jstring jSql) {
  try {
    // update
    specificHandle->_resultSet = NULL;
    specificHandle->_rsmd = NULL;
    specificHandle->_numCols = 0;
    specificHandle->_numRows = 0;
    jint numRows = jvm.CallIntMethodA(specificHandle->_stmt, "stmt", "execUpdate", make_args("l", jSql).get());

    specificHandle->_resultSet = NULL;
    specificHandle->_rsmd = NULL;
    specificHandle->_numCols = 0;
    specificHandle->_numRows = reinterpret_cast<long>(numRows);
  } catch (java_error& e) {
    return SQL_ERROR;
  }
  return SQL_SUCCESS;
}

SQLRETURN ADD_CALL doSelect(O2jbStmtHandle* specificHandle, JvmManager& jvm, jstring jSql) {
  JVM_CALL(specificHandle->_resultSet = jvm.CallObjectMethodA(specificHandle->_stmt, "stmt", "execQuery", make_args("L", jSql).get()));
  JVM_CALL(specificHandle->_rsmd = jvm.CallObjectMethodA(specificHandle->_resultSet, "rs", "metaData"));
  JVM_CALL(jint providedNumCols = jvm.CallIntMethodA(specificHandle->_rsmd, "rsmd", "numCols"));
  specificHandle->_numCols = reinterpret_cast<long>(providedNumCols);
  specificHandle->_numRows = -1;
  return SQL_SUCCESS;
}

SQLRETURN ADD_CALL SQLExecDirect(
  SQLHSTMT     stmtHandle,
  SQLCHAR *    stmtText,
  SQLINTEGER   textLength) {
  SQLRETURN rtnValue = SQL_ERROR;
  O2jbStmtHandle* specificHandle = reinterpret_cast<O2jbStmtHandle*>(stmtHandle);
  stmt_ctr_t::iterator iter = statements.find(specificHandle);
  if (statements.end() == iter) {
    rtnValue = SQL_INVALID_HANDLE;
  } else {
    JvmManager& jvm = specificHandle->jvm();

    // determine what kind of query this is
    jstring jSql = NULL;
    bool isUpdate = determine_sql_type(stmtText, textLength, jvm, &jSql);
    local_ref jSqlRef(jvm, jSql);
    rtnValue = isUpdate ? doUpdate(specificHandle, jvm, jSql) : doSelect(specificHandle, jvm, jSql);
  }
  return rtnValue;
}

SQLRETURN ADD_CALL SQLExecute(
  SQLHSTMT     stmtHandle) {
  SQLRETURN rtnValue = SQL_ERROR;
  try {
    if (NULL == stmtHandle) {
      rtnValue = SQL_INVALID_HANDLE;
    } else {
      O2jbStmtHandle* specificHandle = static_cast<O2jbStmtHandle*>(stmtHandle);

      stmt_ctr_t::iterator iter = statements.find(specificHandle);

      if (statements.end() == iter) {
        rtnValue = SQL_INVALID_HANDLE;
      } else if (NULL == specificHandle->_preparedStmt || NULL == specificHandle->_preparedBindingsPtr) {
        rtnValue = SQL_INVALID_HANDLE;
      } else {
        JvmManager& jvm = specificHandle->jvm();
        // TODO use for_each
        for (O2jbStmtHandle::bindings_ctr_t::iterator iter = specificHandle->_preparedBindingsPtr->begin();
             iter != specificHandle->_preparedBindingsPtr->end(); ++iter) {
          iter->second.update_param(specificHandle, iter->first);
        }

        // determine what kind of query this is
        if (specificHandle->_isUpdatePs) {
          // update
          jint numRows = 0;
          JVM_CALL(numRows = jvm.CallIntMethodA(specificHandle->_preparedStmt, "ps", "execUpdate"));
          specificHandle->_resultSet = NULL;
          specificHandle->_rsmd = NULL;
          specificHandle->_numCols = 0;
          specificHandle->_numRows = reinterpret_cast<long>(numRows);
          rtnValue = SQL_SUCCESS;
        } else {
          // query
          specificHandle->_rsmd = NULL;
          specificHandle->_numCols = 0;
          specificHandle->_numRows = 0;

          JVM_CALL(specificHandle->_resultSet = jvm.CallObjectMethod(specificHandle->_preparedStmt, "ps", "execQuery"));
          JVM_CALL(specificHandle->_rsmd = jvm.CallObjectMethodA(specificHandle->_resultSet, "rs", "metaData"));
          JVM_CALL(jint providedNumCols = jvm.CallIntMethodA(specificHandle->_rsmd, "rsmd", "numCols"));
          specificHandle->_numCols = reinterpret_cast<long>(providedNumCols);
          specificHandle->_numRows = -1;
          rtnValue = SQL_SUCCESS;
        }
      }
    }
  } catch (...) {
    cout << "Caught something" << endl;
  }
  return rtnValue;
}

SQLRETURN ADD_CALL doFetch(O2jbStmtHandle* specificHandle) {
  SQLRETURN rtnValue = SQL_ERROR;
  JNIEnv* env = specificHandle->env();
  JvmManager& jvm = specificHandle->jvm();
  JVM_CALL(jboolean hasNext = jvm.CallBooleanMethodA(specificHandle->_resultSet, "rs", "next"));
  if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
    env->ExceptionClear();
    set_diag(specificHandle, GENERAL_ERROR);
  } else {
    if (JNI_TRUE == hasNext) {
      if (!specificHandle->bindings.empty()) {
        // TODO use for_each
        //foreach(bindings.begin(), bindings.end(), new PopulateBinding(specificHandle));
        for (O2jbStmtHandle::bindings_ctr_t::iterator bi = specificHandle->bindings.begin(); bi != specificHandle->bindings.end(); ++bi) {
          bi->second.update(specificHandle, bi->first);
        }
      }
      rtnValue = SQL_SUCCESS;
    } else {
      rtnValue = SQL_NO_DATA;
    }
  }
  return rtnValue;
}

SQLRETURN ADD_CALL SQLExtendedFetch(
  SQLHSTMT         stmtHandle,
  SQLUSMALLINT     fetchOrientation,
  SQLLEN           fetchOffset,
  SQLULEN *        rowCountPtr,
  SQLUSMALLINT *   rowStatusArray) {
  SQLRETURN rtnValue = SQL_ERROR;

  if (NULL == stmtHandle) {
    rtnValue = SQL_INVALID_HANDLE;
  } else {
    O2jbStmtHandle* specificHandle = static_cast<O2jbStmtHandle*>(stmtHandle);

    stmt_ctr_t::iterator iter = statements.find(specificHandle);

    if (statements.end() == iter) {
      rtnValue = SQL_INVALID_HANDLE;
    } else {
      // TODO Properly handle fetch orientations
      rtnValue = doFetch(specificHandle);
      if (SQL_SUCCESS == rtnValue) {
        specificHandle->_lastFetchOffset = fetchOffset;
        specificHandle->_lastFetchOrientation = fetchOrientation;

        if (NULL != rowCountPtr) {
          *rowCountPtr = 1;
        }
      }

      if (SQL_SUCCESS != rtnValue) {
        set_diag(specificHandle, GENERAL_ERROR);
      }
    }
  }
  return rtnValue;
}

SQLRETURN ADD_CALL SQLFetch(
  SQLHSTMT stmtHandle) {
  SQLRETURN rtnValue = SQL_ERROR;
  if (NULL == stmtHandle) {
    rtnValue = SQL_INVALID_HANDLE;
  } else {
    O2jbStmtHandle* specificHandle = static_cast<O2jbStmtHandle*>(stmtHandle);

    stmt_ctr_t::iterator iter = statements.find(specificHandle);

    if (statements.end() == iter) {
      rtnValue = SQL_INVALID_HANDLE;
    } else {
      rtnValue = doFetch(specificHandle);
    }
  }
  return rtnValue;
}

SQLRETURN ADD_CALL SQLFetchScroll(
  SQLHSTMT      StatementHandle,
  SQLSMALLINT   FetchOrientation,
  SQLLEN        FetchOffset) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLForeignKeys(
  SQLHSTMT       StatementHandle,
  SQLCHAR *      PKCatalogName,
  SQLSMALLINT    NameLength1,
  SQLCHAR *      PKSchemaName,
  SQLSMALLINT    NameLength2,
  SQLCHAR *      PKTableName,
  SQLSMALLINT    NameLength3,
  SQLCHAR *      FKCatalogName,
  SQLSMALLINT    NameLength4,
  SQLCHAR *      FKSchemaName,
  SQLSMALLINT    NameLength5,
  SQLCHAR *      FKTableName,
  SQLSMALLINT    NameLength6) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

// SQLFreeConnect
SQLRETURN ADD_CALL SQLFreeConnect(SQLHANDLE inputHandle) {
  return SQLFreeHandle(SQL_HANDLE_ENV, inputHandle);
}

// SQLFreeEnv
SQLRETURN ADD_CALL SQLFreeEnv(SQLHANDLE inputHandle) {
  return SQLFreeHandle(SQL_HANDLE_ENV, inputHandle);
}

SQLRETURN ADD_CALL SQLFreeHandle(SQLSMALLINT handleType, SQLHANDLE inputHandle) {
  SQLRETURN rtnVal = SQL_ERROR;

  if (inputHandle != NULL) {
    switch (handleType) {
    case SQL_HANDLE_ENV:
    {
      O2jbEnvHandle *envHandle = static_cast<O2jbEnvHandle*>(inputHandle);
      env_ctr_t::iterator envIter = environments.find(envHandle);
      if (envIter != environments.end() && *envIter == envHandle) {
        environments.erase(envIter);
        // TODO remove all connections (and statements) for this connection
        delete envHandle;
        rtnVal = SQL_SUCCESS;
      } else {
        rtnVal = SQL_INVALID_HANDLE;
      }
    }
    break;
    case SQL_HANDLE_DBC:
    {
      O2jbConnHandle *connHandle = static_cast<O2jbConnHandle*>(inputHandle);
      conn_ctr_t::iterator connIter = connections.find(connHandle);
      if (connIter != connections.end() && *connIter == connHandle) {
        connections.erase(connIter);
        // TODO remove all statements for this connection
        delete connHandle;
        rtnVal = SQL_SUCCESS;
      } else {
        rtnVal = SQL_INVALID_HANDLE;
      }
    }
    break;
    case SQL_HANDLE_STMT:
    {
      O2jbStmtHandle *stmtHandle = static_cast<O2jbStmtHandle*>(inputHandle);
      stmt_ctr_t::iterator stmtIter = statements.find(stmtHandle);
      if (stmtIter != statements.end() && *stmtIter == stmtHandle) {
        statements.erase(stmtIter);
        delete stmtHandle;
        rtnVal = SQL_SUCCESS;
      } else {
        rtnVal = SQL_INVALID_HANDLE;
      }
    }
    break;
    }
  }
  return rtnVal;
}

// Already Exist?
SQLRETURN ADD_CALL SQLFreeStmt(
  SQLHSTMT       inputHandle,
  SQLUSMALLINT   option) {
  switch (option) {
  case SQL_DROP:
    return SQLFreeHandle(SQL_HANDLE_ENV, inputHandle);
  default:
    return SQL_SUCCESS;
  }
}

SQLRETURN ADD_CALL SQLGetConnectAttr(
  SQLHDBC        ConnectionHandle,
  SQLINTEGER     Attribute,
  SQLPOINTER     ValuePtr,
  SQLINTEGER     BufferLength,
  SQLINTEGER *   StringLengthPtr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

// SQLGetConnectOption


SQLRETURN ADD_CALL SQLGetCursorName(
  SQLHSTMT        StatementHandle,
  SQLCHAR *       CursorName,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   NameLengthPtr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}


SQLRETURN ADD_CALL SQLGetData(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   colNum,
  SQLSMALLINT    TargetType,
  SQLPOINTER     TargetValuePtr,
  SQLLEN         BufferLength,
  SQLLEN *       outLen) {
  SQLRETURN rtnVal = colNum < 3 ? SQL_SUCCESS : SQL_SUCCESS + 1;

  if (outLen != NULL) {
    *outLen = 0;
  }

  return rtnVal;
}

SQLRETURN ADD_CALL SQLGetDescRec(
  SQLHDESC        DescriptorHandle,
  SQLSMALLINT     RecNumber,
  SQLCHAR *       Name,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   StringLengthPtr,
  SQLSMALLINT *   TypePtr,
  SQLSMALLINT *   SubTypePtr,
  SQLLEN *        LengthPtr,
  SQLSMALLINT *   PrecisionPtr,
  SQLSMALLINT *   ScalePtr,
  SQLSMALLINT *   NullablePtr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_NO_DATA;
}

SQLRETURN ADD_CALL SQLGetDiagField(
  SQLSMALLINT     handleType,
  SQLHANDLE       handle,
  SQLSMALLINT     recNumber,
  SQLSMALLINT     diagIdentifier,
  SQLPOINTER      diagInfoPtr,
  SQLSMALLINT     bufferLength,
  SQLSMALLINT *   stringLengthPtr) {
  SQLRETURN rtnValue = SQL_ERROR;
  switch (diagIdentifier) {
  case SQL_DIAG_CLASS_ORIGIN:
    if (NULL != diagInfoPtr && bufferLength > 0) {
      char const * const value = "ISO 9075";
      char* asChar = reinterpret_cast<char*>(diagInfoPtr);
      int copyLen = min(8, bufferLength - 1);
      strncpy(asChar, value, copyLen);
      asChar[copyLen] = '\0';
    }
    rtnValue = SQL_SUCCESS;
    break;
  // case SQL_DIAG_CLASS_SUBCLASS_ORIGIN:
  case SQL_DIAG_CONNECTION_NAME:
  case SQL_DIAG_MESSAGE_TEXT:
  case SQL_DIAG_NATIVE:
  case SQL_DIAG_NUMBER:
  case SQL_DIAG_RETURNCODE:
  case SQL_DIAG_SERVER_NAME:
  case SQL_DIAG_SQLSTATE:
    rtnValue = SQL_SUCCESS;
    break;
  default:
    rtnValue = SQL_ERROR;
    break;
  }
  // return SQL_NO_DATA;
  return rtnValue;
}

SQLRETURN ADD_CALL SQLGetDiagRec(
  SQLSMALLINT     HandleType,
  SQLHANDLE       Handle,
  SQLSMALLINT     RecNumber,
  SQLCHAR *       SQLState,
  SQLINTEGER *    NativeErrorPtr,
  SQLCHAR *       MessageText,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *   TextLengthPtr) {
  return SQL_SUCCESS;
}

SQLRETURN ADD_CALL SQLGetEnvAttr(
  SQLHENV        envHandle,
  SQLINTEGER     attribute,
  SQLPOINTER     valuePtr,
  SQLINTEGER     bufferLength,
  SQLINTEGER *   stringLengthPtr) {
  SQLRETURN rtnValue = SQL_ERROR;

  if (NULL != envHandle) {
    // TODO lookup the environment
    O2jbEnvHandle* specificHandle = reinterpret_cast<O2jbEnvHandle*>(envHandle);
    if (NULL == valuePtr) {
      set_diag(specificHandle, INVALID_NULL_PTR);
    } else {
      O2jbEnvHandle* specificHandle = reinterpret_cast<O2jbEnvHandle*>(envHandle);
      O2jbEnvHandle::attr_ctr_t::iterator attrIter = specificHandle->attributes.find(attribute);
      if (specificHandle->attributes.end() != attrIter) {
        if (NULL == valuePtr) {
          set_diag(specificHandle, GENERAL_ERROR);
          rtnValue = SQL_ERROR;
        } else {
          int32_t& valueRef = reinterpret_cast<int32_t&>(valuePtr);
          valueRef = specificHandle->attributes[attribute];
          rtnValue = SQL_SUCCESS;
        }
      } else {
        switch (attribute) {
        case SQL_ATTR_CONNECTION_POOLING:
        case SQL_ATTR_CP_MATCH:
        case SQL_ATTR_ODBC_VERSION:
          // TODO determine appopriate defaults
          rtnValue = SQL_SUCCESS;
          break;
        case SQL_ATTR_OUTPUT_NTS:
        {
          int32_t& valueRef = reinterpret_cast<int32_t&>(valuePtr);
          valueRef = SQL_TRUE;
          rtnValue = SQL_SUCCESS;
        }
        break;
        default:
          set_diag(specificHandle, INVALID_ATTR);
        }

      }
    }
  }
  return rtnValue;
}

// Intentionally skipped
// SQLRETURN SQLGetFunctions(
//      SQLHDBC           connectionHandle,
//      SQLUSMALLINT      functionId,
//      SQLUSMALLINT *    supportedPtr) {
// }

SQLRETURN ADD_CALL SQLGetInfo(
  SQLHDBC         connectionHandle,
  SQLUSMALLINT    infoType,
  SQLPOINTER      infoValuePtr,
  SQLSMALLINT     bufferLength,
  SQLSMALLINT *   stringLengthPtr) {
  SQLRETURN rtnValue = SQL_ERROR;
  switch (infoType) {
  case SQL_DRIVER_ODBC_VER:
    if (NULL != infoValuePtr) {
      char * dest = reinterpret_cast<char*>(infoValuePtr);
      *stringLengthPtr = min((short)5, bufferLength);
      strncpy(dest, "02.00", *stringLengthPtr);
      if (bufferLength > 5) {
        dest[5] = '\0';
      }
    }
    rtnValue = SQL_SUCCESS;
    break;
  case SQL_DATABASE_NAME:
  // TODO
  case SQL_USER_NAME:
    // TODO
    rtnValue = SQL_ERROR;
    break;
  }

  return rtnValue;
}


SQLRETURN ADD_CALL SQLGetStmtAttr(
  SQLHSTMT        StatementHandle,
  SQLINTEGER      Attribute,
  SQLPOINTER      ValuePtr,
  SQLINTEGER      BufferLength,
  SQLINTEGER *    StringLengthPtr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

// SQLGetStmtOption


SQLRETURN ADD_CALL SQLGetTypeInfo(
  SQLHSTMT      StatementHandle,
  SQLSMALLINT   DataType) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLMoreResults(
  SQLHSTMT     StatementHandle) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLNativeSql(
  SQLHDBC        ConnectionHandle,
  SQLCHAR *      InStatementText,
  SQLINTEGER     TextLength1,
  SQLCHAR *      OutStatementText,
  SQLINTEGER     BufferLength,
  SQLINTEGER *   TextLength2Ptr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}


SQLRETURN ADD_CALL SQLNumParams(
  SQLHSTMT        StatementHandle,
  SQLSMALLINT *   ParameterCountPtr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLNumResultCols(
  SQLHSTMT        stmtHandle,
  SQLSMALLINT *   colCntPtr) {
  O2jbStmtHandle* specificHandle = reinterpret_cast<O2jbStmtHandle*>(stmtHandle);
  // TODO validate the handle
  *colCntPtr = specificHandle->_numCols;
  return SQL_SUCCESS;
}

SQLRETURN ADD_CALL SQLParamData(
  SQLHSTMT       StatementHandle,
  SQLPOINTER *   ValuePtrPtr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

// SQLParamOptions

SQLRETURN ADD_CALL SQLPrepare(
  SQLHSTMT      stmtHandle,
  SQLCHAR *     sql,
  SQLINTEGER    length) {
  SQLRETURN rtnValue = SQL_ERROR;

  if (NULL == sql) {
    // TODO log that the sql was bad
  } else if (NULL == stmtHandle) {
    rtnValue = SQL_INVALID_HANDLE;
  } else {
    O2jbStmtHandle* specificHandle = reinterpret_cast<O2jbStmtHandle*>(stmtHandle);

    stmt_ctr_t::iterator iter = statements.find(specificHandle);
    if (statements.end() == iter) {
      rtnValue = SQL_INVALID_HANDLE;
    } else {
      stmt_ctr_t::iterator iter = statements.find(reinterpret_cast<O2jbStmtHandle*>(stmtHandle));
      if (statements.end() == iter) {
        rtnValue = SQL_INVALID_HANDLE;
      } else {
        JvmManager& jvm = specificHandle->jvm();
        jstring jSql;
        specificHandle->_isUpdatePs = determine_sql_type(sql, length, jvm, &jSql);
        // local_ref(jvm, jSql);
        char* cSql = reinterpret_cast<char*>(sql);
        bool goodPrep = false;
        if (SQL_NTS == length) {
          goodPrep = specificHandle->make_prepared(cSql);
        } else {
          string cppSql(cSql, length);
          goodPrep = specificHandle->make_prepared(cppSql.c_str());
        }

        if (goodPrep) {
          rtnValue = SQL_SUCCESS;
        } else {
          set_diag(specificHandle, GENERAL_ERROR);
        }
      }
    }
  }
  return rtnValue;
}

SQLRETURN ADD_CALL SQLPrimaryKeys(
  SQLHSTMT       StatementHandle,
  SQLCHAR *      CatalogName,
  SQLSMALLINT    NameLength1,
  SQLCHAR *      SchemaName,
  SQLSMALLINT    NameLength2,
  SQLCHAR *      TableName,
  SQLSMALLINT    NameLength3) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}


SQLRETURN ADD_CALL SQLProcedureColumns(
  SQLHSTMT      StatementHandle,
  SQLCHAR *     CatalogName,
  SQLSMALLINT   NameLength1,
  SQLCHAR *     SchemaName,
  SQLSMALLINT   NameLength2,
  SQLCHAR *     ProcName,
  SQLSMALLINT   NameLength3,
  SQLCHAR *     ColumnName,
  SQLSMALLINT   NameLength4) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}


SQLRETURN ADD_CALL SQLProcedures(
  SQLHSTMT       StatementHandle,
  SQLCHAR *      CatalogName,
  SQLSMALLINT    NameLength1,
  SQLCHAR *      SchemaName,
  SQLSMALLINT    NameLength2,
  SQLCHAR *      ProcName,
  SQLSMALLINT    NameLength3) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLPutData(
  SQLHSTMT     StatementHandle,
  SQLPOINTER   DataPtr,
  SQLLEN       StrLen_or_Ind) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLRowCount(
  SQLHSTMT   stmtHandle,
  SQLLEN *   rowCountPtr) {
  // TODO validate the handle
  cout << "rc1" << endl;
  O2jbStmtHandle* specificHandle = reinterpret_cast<O2jbStmtHandle*>(stmtHandle);
  cout << "rc2:  " << specificHandle << endl;
  *rowCountPtr = specificHandle->_numRows;
  cout << "rc3" << endl;
  return SQL_SUCCESS;
}

// SQLSetConnectOption

SQLRETURN ADD_CALL SQLSetCursorName(
  SQLHSTMT      StatementHandle,
  SQLCHAR *     CursorName,
  SQLSMALLINT   NameLength) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLSetDescField(
  SQLHDESC      DescriptorHandle,
  SQLSMALLINT   RecNumber,
  SQLSMALLINT   FieldIdentifier,
  SQLPOINTER    ValuePtr,
  SQLINTEGER    BufferLength) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}


SQLRETURN ADD_CALL SQLSetDescRec(
  SQLHDESC      DescriptorHandle,
  SQLSMALLINT   RecNumber,
  SQLSMALLINT   Type,
  SQLSMALLINT   SubType,
  SQLLEN        Length,
  SQLSMALLINT   Precision,
  SQLSMALLINT   Scale,
  SQLPOINTER    DataPtr,
  SQLLEN *      StringLengthPtr,
  SQLLEN *      IndicatorPtr) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL set_env_attr(
  SQLHENV      envHandle,
  SQLINTEGER   attribute,
  int32_t   value) {
  SQLRETURN rtnValue = SQL_ERROR;
  if (NULL == envHandle) {
    rtnValue = SQL_ERROR;
  } else {
    O2jbEnvHandle* specificHandle = reinterpret_cast<O2jbEnvHandle*>(envHandle);
    env_ctr_t::iterator iter = environments.find(specificHandle);
    if (environments.end() == iter) {
      rtnValue = SQL_INVALID_HANDLE;
    } else {
      specificHandle->attributes[attribute] = value;
      rtnValue = SQL_SUCCESS;
    }
  }
  return rtnValue;
}

SQLRETURN ADD_CALL SQLSetEnvAttr(
  SQLHENV      envHandle,
  SQLINTEGER   attribute,
  SQLPOINTER   valuePtr,
  SQLINTEGER   stringLength) {
  SQLRETURN rtnValue = SQL_ERROR;
  if (NULL != valuePtr) {
    O2jbEnvHandle* const specificHandle = reinterpret_cast<O2jbEnvHandle*>(envHandle);
    O2jbEnvHandle::attr_ctr_t::mapped_type& value = reinterpret_cast<O2jbEnvHandle::attr_ctr_t::mapped_type&>(valuePtr);
    switch (attribute) {
    case SQL_ATTR_CONNECTION_POOLING:
      if (NULL == envHandle) {
        processConnPoolAttr = value;
      } else {
        rtnValue = set_env_attr(envHandle, attribute, value);
      }
      break;
    case SQL_ATTR_CP_MATCH:
      switch (value) {
      case SQL_CP_STRICT_MATCH:
      case SQL_CP_RELAXED_MATCH:
        rtnValue = set_env_attr(envHandle, attribute, value);
        break;
      default:
        // TODO invalid attribute
        set_diag(specificHandle, GENERAL_ERROR);
        rtnValue = SQL_ERROR;
      }
      break;
    case SQL_ATTR_ODBC_VERSION:
      switch (value) {
      // TODO address if ODBC 3.8
      // case SQL_OV_ODBC3_80:
      case SQL_OV_ODBC3:
      case SQL_OV_ODBC2:
        rtnValue = set_env_attr(envHandle, attribute, value);
        if (SQL_SUCCESS == rtnValue) {
          specificHandle->odbcVersionKnown = true;
        }
        break;
      default:
        set_diag<O2jbEnvHandle>(specificHandle, GENERAL_ERROR);
        rtnValue = SQL_ERROR;
      }
      break;
    case SQL_ATTR_OUTPUT_NTS:
      if (SQL_TRUE == value) {
        rtnValue = SQL_SUCCESS;
      } else {
        set_diag<O2jbEnvHandle>(specificHandle, GENERAL_ERROR);
        rtnValue = SQL_ERROR;
      }
      break;
    default:
      // TODO set unknown option
      set_diag<O2jbEnvHandle>(specificHandle, GENERAL_ERROR);
      rtnValue = SQL_ERROR;
      break;
    }
  }
  return rtnValue;
}

// SQLSetParam see SQLBindParameter

SQLRETURN ADD_CALL SQLSetPos(
  SQLHSTMT        StatementHandle,
  SQLSETPOSIROW   RowNumber,
  SQLUSMALLINT    Operation,
  SQLUSMALLINT    LockType) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

// SQLSetScrollOptions see SQLGetInfo and SQLSetStmtAttr


SQLRETURN ADD_CALL SQLSetStmtAttr(
  SQLHSTMT      StatementHandle,
  SQLINTEGER    Attribute,
  SQLPOINTER    ValuePtr,
  SQLINTEGER    StringLength) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

// SQLSetStmtOption see SQLSetStmtAttr

// SQLRETURN SQLSpecialColumns(
//   SQLHSTMT      StatementHandle,
//   SQLSMALLINT   IdentifierType,
//   SQLCHAR *     CatalogName,
//   SQLSMALLINT   NameLength1,
//   SQLCHAR *     SchemaName,
//   SQLSMALLINT   NameLength2,
//   SQLCHAR *     TableName,
//   SQLSMALLINT   NameLength3,
//   SQLSMALLINT   Scope,
//   SQLSMALLINT   Nullable) {
//   LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
//   return SQL_ERROR;
// }

SQLRETURN ADD_CALL SQLStatistics(
  SQLHSTMT        StatementHandle,
  SQLCHAR *       CatalogName,
  SQLSMALLINT     NameLength1,
  SQLCHAR *       SchemaName,
  SQLSMALLINT     NameLength2,
  SQLCHAR *       TableName,
  SQLSMALLINT     NameLength3,
  SQLUSMALLINT    Unique,
  SQLUSMALLINT    Reserved) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}

SQLRETURN ADD_CALL SQLTablePrivileges(
  SQLHSTMT      StatementHandle,
  SQLCHAR *     CatalogName,
  SQLSMALLINT   NameLength1,
  SQLCHAR *     SchemaName,
  SQLSMALLINT   NameLength2,
  SQLCHAR *     TableName,
  SQLSMALLINT   NameLength3) {
  LOG_ERROR(logger, "returning error in " << __FILE__ << ":" << __LINE__ << " due to unimplemented function.");
  return SQL_ERROR;
}


SQLRETURN ADD_CALL SQLTables(
  SQLHSTMT       stmtHandle,
  SQLCHAR *      catalogName,
  SQLSMALLINT    catalogNameLength,
  SQLCHAR *      schemaName,
  SQLSMALLINT    schemaNameLength,
  SQLCHAR *      tableName,
  SQLSMALLINT    tableNameLength,
  SQLCHAR *      tableType,
  SQLSMALLINT    tableTypeLength) {
  LOG_DEBUG(logger, "[" << toCppString((char*)catalogName, catalogNameLength) << "]["
            << toCppString((char*)schemaName, schemaNameLength) << "]["
            << toCppString((char*)tableName, tableNameLength) << "]["
            << toCppString((char*)tableType, tableTypeLength) << "]");

  SQLRETURN rtnValue = SQL_ERROR;

  if (NULL == stmtHandle) {
    rtnValue = SQL_INVALID_HANDLE;
  } else {
    O2jbStmtHandle* specificHandle = reinterpret_cast<O2jbStmtHandle*>(stmtHandle);

    stmt_ctr_t::iterator iter = statements.find(specificHandle);
    if (statements.end() == iter) {
      rtnValue = SQL_INVALID_HANDLE;
    } else {
      stmt_ctr_t::iterator iter = statements.find(reinterpret_cast<O2jbStmtHandle*>(stmtHandle));
      if (statements.end() == iter) {
        rtnValue = SQL_INVALID_HANDLE;
      } else {
        JNIEnv* env = specificHandle->env();
        JvmManager& jvm = specificHandle->jvm();

        JVM_CALL(jobject dbmd = jvm.CallObjectMethodA(specificHandle->_connHandle->_conn, "conn", "getDbmd"));
        string sCatalog = toCppString(reinterpret_cast<char*>(catalogName), catalogNameLength);
        string sSchema = toCppString(reinterpret_cast<char*>(schemaName), schemaNameLength);
        string sTable = toCppString(reinterpret_cast<char*>(tableName), tableNameLength);

        if (SQL_ALL_CATALOGS == sCatalog && sSchema.empty() && sTable.empty()) {
          // TODO address the redaction of data to make all columns except TABLE_CAT null
          jstring catalog = toJString(env, reinterpret_cast<char*>(catalogName), catalogNameLength);
          JVM_CALL(specificHandle->_resultSet = jvm.CallObjectMethodA(dbmd, "dbmd", "getTables",
                                                make_args("LLLL", catalog, NULL, NULL, NULL).get()));
        } else if (SQL_ALL_SCHEMAS == sSchema && sCatalog.empty() && sTable.empty()) {
          // TODO address the redaction of data to make all columns except TABLE_SCHEM null
          jstring schemaPattern = toJString(env, reinterpret_cast<char*>(catalogName), catalogNameLength);
          JVM_CALL(specificHandle->_resultSet = jvm.CallObjectMethodA(dbmd, "dbmd", "getTables",
                                                make_args("LLLL", NULL, schemaPattern, NULL, NULL).get()));
        } else if (SQL_ALL_TABLE_TYPES == sTable && sCatalog.empty() && sSchema.empty()) {
          // TODO address the redaction of data to make all columns except TABLE_TYPE null
          jstring tableNamePattern = toJString(env, reinterpret_cast<char*>(catalogName), catalogNameLength);
          JVM_CALL(specificHandle->_resultSet = jvm.CallObjectMethodA(dbmd, "dbmd", "getTables",
                                                make_args("LLLL", NULL, NULL, tableNamePattern, NULL).get()));
        } else {
          jstring catalog = toJString(env, reinterpret_cast<char*>(catalogName), catalogNameLength);
          jstring schemaPattern = toJString(env, reinterpret_cast<char*>(schemaName), schemaNameLength);
          jstring tableNamePattern = toJString(env, reinterpret_cast<char*>(tableName), tableNameLength);
          jobjectArray types = NULL;
          if (NULL != tableType) {
            vector<string> tokens;

            Tokenize(toCppString(reinterpret_cast<char*>(tableType), tableTypeLength), tokens, ",");
            size_t numTypes = tokens.size();
            types = env->NewObjectArray(numTypes, specificHandle->_connHandle->_stringCls,
                                        toJString(env, "", SQL_NTS));
            for (size_t i = 0; i < numTypes; ++i) {
              env->SetObjectArrayElement(types, i, toJString(env, tokens[i].c_str(), SQL_NTS));
            }
          }
          JVM_CALL(specificHandle->_resultSet = jvm.CallObjectMethodA(dbmd, "dbmd",  "getTables",
                                                make_args("LLLL", catalog, schemaPattern, tableNamePattern,
                                                    types).get()));
        }

        if (NULL == specificHandle->_resultSet || env->ExceptionCheck()) {
          env->ExceptionDescribe();
          env->ExceptionClear();
          set_diag(specificHandle, GENERAL_ERROR);
        } else {
          rtnValue = SQL_SUCCESS;
        }
      }
    }
  }

  return rtnValue;
}



// SQLTransact see SQLEndTran
