#ifndef _INCLUDE_GUARD_O2JB_H_
#define _INCLUDE_GUARD_O2JB_H_ 1
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

#ifdef __cplusplus
extern "C" {
#endif
  
#ifdef BUILDING_O2JB_DLL
#define O2JB_DLL __declspec(dllexport)
#else
#define O2JB_DLL __declspec(dllimport)
#endif

SQLRETURN O2JB_DLL SQLAllocHandle(SQLSMALLINT handleType, SQLHANDLE inputHandle, SQLHANDLE *outHandle);

SQLRETURN O2JB_DLL SQLBindCol(
      SQLHSTMT       StatementHandle,
      SQLUSMALLINT   ColumnNumber,
      SQLSMALLINT    TargetType,
      SQLPOINTER     TargetValuePtr,
      SQLLEN         BufferLength,
      SQLLEN *       StrLen_or_Ind);

SQLRETURN O2JB_DLL SQLBindParameter(
      SQLHSTMT        StatementHandle,
      SQLUSMALLINT    ParameterNumber,
      SQLSMALLINT     InputOutputType,
      SQLSMALLINT     ValueType,
      SQLSMALLINT     ParameterType,
      SQLULEN         ColumnSize,
      SQLSMALLINT     DecimalDigits,
      SQLPOINTER      ParameterValuePtr,
      SQLLEN          BufferLength,
      SQLLEN *        StrLen_or_IndPtr);

SQLRETURN O2JB_DLL SQLBrowseConnect(
     SQLHDBC         ConnectionHandle,
     SQLCHAR *       InConnectionString,
     SQLSMALLINT     StringLength1,
     SQLCHAR *       OutConnectionString,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   StringLength2Ptr);

// SQLRETURN O2JB_DLL SQLBulkOperations(
//      SQLHSTMT       StatementHandle,
//      SQLUSMALLINT   Operation);

SQLRETURN O2JB_DLL SQLCancel(
     SQLHSTMT     StatementHandle);

SQLRETURN O2JB_DLL SQLCloseCursor(
     SQLHSTMT     StatementHandle);

SQLRETURN O2JB_DLL SQLColAttribute (
      SQLHSTMT        StatementHandle,
      SQLUSMALLINT    ColumnNumber,
      SQLUSMALLINT    FieldIdentifier,
      SQLPOINTER      CharacterAttributePtr,
      SQLSMALLINT     BufferLength,
      SQLSMALLINT *   StringLengthPtr,
      SQLLEN *        NumericAttributePtr);

SQLRETURN O2JB_DLL SQLColumnPrivileges(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CatalogName,
     SQLSMALLINT   NameLength1,
     SQLCHAR *     SchemaName,
     SQLSMALLINT   NameLength2,
     SQLCHAR *     TableName,
     SQLSMALLINT   NameLength3,
     SQLCHAR *     ColumnName,
     SQLSMALLINT   NameLength4);

SQLRETURN O2JB_DLL SQLColumns(
     SQLHSTMT       StatementHandle,
     SQLCHAR *      CatalogName,
     SQLSMALLINT    NameLength1,
     SQLCHAR *      SchemaName,
     SQLSMALLINT    NameLength2,
     SQLCHAR *      TableName,
     SQLSMALLINT    NameLength3,
     SQLCHAR *      ColumnName,
     SQLSMALLINT    NameLength4);


// Already Exist?
SQLRETURN O2JB_DLL SQLConnect(
     SQLHDBC        ConnectionHandle,
     SQLCHAR *      ServerName,
     SQLSMALLINT    NameLength1,
     SQLCHAR *      UserName,
     SQLSMALLINT    NameLength2,
     SQLCHAR *      Authentication,
     SQLSMALLINT    NameLength3);

SQLRETURN O2JB_DLL SQLCopyDesc(
     SQLHDESC     SourceDescHandle,
     SQLHDESC     TargetDescHandle);

SQLRETURN O2JB_DLL SQLDataSources(
     SQLHENV          EnvironmentHandle,
     SQLUSMALLINT     Direction,
     SQLCHAR *        ServerName,
     SQLSMALLINT      BufferLength1,
     SQLSMALLINT *    NameLength1Ptr,
     SQLCHAR *        Description,
     SQLSMALLINT      BufferLength2,
     SQLSMALLINT *    NameLength2Ptr);

SQLRETURN O2JB_DLL SQLDescribeCol(
      SQLHSTMT       StatementHandle,
      SQLUSMALLINT   ColumnNumber,
      SQLCHAR *      ColumnName,
      SQLSMALLINT    BufferLength,
      SQLSMALLINT *  NameLengthPtr,
      SQLSMALLINT *  DataTypePtr,
      SQLULEN *      ColumnSizePtr,
      SQLSMALLINT *  DecimalDigitsPtr,
      SQLSMALLINT *  NullablePtr);


SQLRETURN O2JB_DLL SQLDescribeParam(
      SQLHSTMT        StatementHandle,
      SQLUSMALLINT    ParameterNumber,
      SQLSMALLINT *   DataTypePtr,
      SQLULEN *       ParameterSizePtr,
      SQLSMALLINT *   DecimalDigitsPtr,
      SQLSMALLINT *   NullablePtr);

// Already Exist?
SQLRETURN O2JB_DLL SQLDisconnect(
     SQLHDBC     ConnectionHandle);

// Already Exist?
SQLRETURN O2JB_DLL SQLDriverConnect(
     SQLHDBC         ConnectionHandle,
     SQLHWND         WindowHandle,
     SQLCHAR *       InConnectionString,
     SQLSMALLINT     StringLength1,
     SQLCHAR *       OutConnectionString,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   StringLength2Ptr,
     SQLUSMALLINT    DriverCompletion);

SQLRETURN O2JB_DLL SQLEndTran(
     SQLSMALLINT   HandleType,
     SQLHANDLE     Handle,
     SQLSMALLINT   CompletionType);

// Already Exist?
SQLRETURN O2JB_DLL SQLExecDirect(
     SQLHSTMT     StatementHandle,
     SQLCHAR *    StatementText,
     SQLINTEGER   TextLength);

SQLRETURN O2JB_DLL SQLExecute(
     SQLHSTMT     StatementHandle);

SQLRETURN O2JB_DLL SQLExtendedFetch(
      SQLHSTMT         StatementHandle,
      SQLUSMALLINT     FetchOrientation,
      SQLLEN           FetchOffset,
      SQLULEN *        RowCountPtr,
      SQLUSMALLINT *   RowStatusArray);

// Already Exist?
SQLRETURN O2JB_DLL SQLFetch(
     SQLHSTMT     StatementHandle);

SQLRETURN O2JB_DLL SQLFetchScroll(
      SQLHSTMT      StatementHandle,
      SQLSMALLINT   FetchOrientation,
      SQLLEN        FetchOffset);

SQLRETURN O2JB_DLL SQLForeignKeys(
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
     SQLSMALLINT    NameLength6);

// SQLFreeConnect

// SQLFreeEnv

// Already Exist?
SQLRETURN O2JB_DLL SQLFreeHandle(
     SQLSMALLINT   HandleType,
     SQLHANDLE     Handle);

// Already Exist?
SQLRETURN O2JB_DLL SQLFreeStmt(
     SQLHSTMT       StatementHandle,
     SQLUSMALLINT   Option);


SQLRETURN O2JB_DLL SQLGetConnectAttr(
     SQLHDBC        ConnectionHandle,
     SQLINTEGER     Attribute,
     SQLPOINTER     ValuePtr,
     SQLINTEGER     BufferLength,
     SQLINTEGER *   StringLengthPtr);

// SQLGetConnectOption


SQLRETURN O2JB_DLL SQLGetCursorName(
     SQLHSTMT        StatementHandle,
     SQLCHAR *       CursorName,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   NameLengthPtr);

SQLRETURN O2JB_DLL SQLGetData(
      SQLHSTMT       StatementHandle,
      SQLUSMALLINT   Col_or_Param_Num,
      SQLSMALLINT    TargetType,
      SQLPOINTER     TargetValuePtr,
      SQLLEN         BufferLength,
      SQLLEN *       StrLen_or_IndPtr);

SQLRETURN O2JB_DLL SQLGetData(
      SQLHSTMT       StatementHandle,
      SQLUSMALLINT   Col_or_Param_Num,
      SQLSMALLINT    TargetType,
      SQLPOINTER     TargetValuePtr,
      SQLLEN         BufferLength,
      SQLLEN *       StrLen_or_IndPtr);

SQLRETURN O2JB_DLL SQLGetDescRec(
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
      SQLSMALLINT *   NullablePtr);

SQLRETURN O2JB_DLL SQLGetDiagField(
     SQLSMALLINT     HandleType,
     SQLHANDLE       Handle,
     SQLSMALLINT     RecNumber,
     SQLSMALLINT     DiagIdentifier,
     SQLPOINTER      DiagInfoPtr,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   StringLengthPtr);

SQLRETURN O2JB_DLL SQLGetDiagRec(
     SQLSMALLINT     HandleType,
     SQLHANDLE       Handle,
     SQLSMALLINT     RecNumber,
     SQLCHAR *       SQLState,
     SQLINTEGER *    NativeErrorPtr,
     SQLCHAR *       MessageText,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   TextLengthPtr);

SQLRETURN O2JB_DLL SQLGetEnvAttr(
     SQLHENV        EnvironmentHandle,
     SQLINTEGER     Attribute,
     SQLPOINTER     ValuePtr,
     SQLINTEGER     BufferLength,
     SQLINTEGER *   StringLengthPtr);

SQLRETURN O2JB_DLL SQLGetFunctions(
     SQLHDBC           ConnectionHandle,
     SQLUSMALLINT      FunctionId,
     SQLUSMALLINT *    SupportedPtr);

SQLRETURN O2JB_DLL SQLGetInfo(
     SQLHDBC         ConnectionHandle,
     SQLUSMALLINT    InfoType,
     SQLPOINTER      InfoValuePtr,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   StringLengthPtr);


SQLRETURN O2JB_DLL SQLGetStmtAttr(
     SQLHSTMT        StatementHandle,
     SQLINTEGER      Attribute,
     SQLPOINTER      ValuePtr,
     SQLINTEGER      BufferLength,
     SQLINTEGER *    StringLengthPtr);

// SQLGetStmtOption


SQLRETURN O2JB_DLL SQLGetTypeInfo(
     SQLHSTMT      StatementHandle,
     SQLSMALLINT   DataType);

SQLRETURN O2JB_DLL SQLMoreResults(
     SQLHSTMT     StatementHandle);

SQLRETURN O2JB_DLL SQLNativeSql(
     SQLHDBC        ConnectionHandle,
     SQLCHAR *      InStatementText,
     SQLINTEGER     TextLength1,
     SQLCHAR *      OutStatementText,
     SQLINTEGER     BufferLength,
     SQLINTEGER *   TextLength2Ptr);


SQLRETURN O2JB_DLL SQLNumParams(
     SQLHSTMT        StatementHandle,
     SQLSMALLINT *   ParameterCountPtr);

SQLRETURN O2JB_DLL SQLNumResultCols(
     SQLHSTMT        StatementHandle,
     SQLSMALLINT *   ColumnCountPtr);

SQLRETURN O2JB_DLL SQLParamData(
     SQLHSTMT       StatementHandle,
     SQLPOINTER *   ValuePtrPtr);

// SQLParamOptions 


SQLRETURN O2JB_DLL SQLPrepare(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     StatementText,
     SQLINTEGER    TextLength);

SQLRETURN O2JB_DLL SQLPrimaryKeys(
     SQLHSTMT       StatementHandle,
     SQLCHAR *      CatalogName,
     SQLSMALLINT    NameLength1,
     SQLCHAR *      SchemaName,
     SQLSMALLINT    NameLength2,
     SQLCHAR *      TableName,
     SQLSMALLINT    NameLength3);


SQLRETURN O2JB_DLL SQLProcedureColumns(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CatalogName,
     SQLSMALLINT   NameLength1,
     SQLCHAR *     SchemaName,
     SQLSMALLINT   NameLength2,
     SQLCHAR *     ProcName,
     SQLSMALLINT   NameLength3,
     SQLCHAR *     ColumnName,
     SQLSMALLINT   NameLength4);


SQLRETURN O2JB_DLL SQLProcedures(
     SQLHSTMT       StatementHandle,
     SQLCHAR *      CatalogName,
     SQLSMALLINT    NameLength1,
     SQLCHAR *      SchemaName,
     SQLSMALLINT    NameLength2,
     SQLCHAR *      ProcName,
     SQLSMALLINT    NameLength3);

SQLRETURN O2JB_DLL SQLPutData(
      SQLHSTMT     StatementHandle,
      SQLPOINTER   DataPtr,
      SQLLEN       StrLen_or_Ind);

SQLRETURN O2JB_DLL SQLRowCount(
      SQLHSTMT   StatementHandle,
      SQLLEN *   RowCountPtr);

// SQLSetConnectOption 

SQLRETURN O2JB_DLL SQLSetCursorName(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CursorName,
     SQLSMALLINT   NameLength);

SQLRETURN O2JB_DLL SQLSetDescField(
     SQLHDESC      DescriptorHandle,
     SQLSMALLINT   RecNumber,
     SQLSMALLINT   FieldIdentifier,
     SQLPOINTER    ValuePtr,
     SQLINTEGER    BufferLength);

SQLRETURN O2JB_DLL SQLSetDescRec(
      SQLHDESC      DescriptorHandle,
      SQLSMALLINT   RecNumber,
      SQLSMALLINT   Type,
      SQLSMALLINT   SubType,
      SQLLEN        Length,
      SQLSMALLINT   Precision,
      SQLSMALLINT   Scale,
      SQLPOINTER    DataPtr,
      SQLLEN *      StringLengthPtr,
      SQLLEN *      IndicatorPtr);


SQLRETURN O2JB_DLL SQLSetEnvAttr(
     SQLHENV      EnvironmentHandle,
     SQLINTEGER   Attribute,
     SQLPOINTER   ValuePtr,
     SQLINTEGER   StringLength);

// SQLSetParam see SQLBindParameter

SQLRETURN O2JB_DLL SQLSetPos(
      SQLHSTMT        StatementHandle,
      SQLSETPOSIROW   RowNumber,
      SQLUSMALLINT    Operation,
      SQLUSMALLINT    LockType);

// SQLSetScrollOptions see SQLGetInfo and SQLSetStmtAttr


SQLRETURN O2JB_DLL SQLSetStmtAttr(
     SQLHSTMT      StatementHandle,
     SQLINTEGER    Attribute,
     SQLPOINTER    ValuePtr,
     SQLINTEGER    StringLength);

// SQLSetStmtOption see SQLSetStmtAttr

// SQLRETURN O2JB_DLL SQLSpecialColumns(
//      SQLHSTMT      StatementHandle,
//      SQLSMALLINT   IdentifierType,
//      SQLCHAR *     CatalogName,
//      SQLSMALLINT   NameLength1,
//      SQLCHAR *     SchemaName,
//      SQLSMALLINT   NameLength2,
//      SQLCHAR *     TableName,
//      SQLSMALLINT   NameLength3,
//      SQLSMALLINT   Scope,
//      SQLSMALLINT   Nullable);

SQLRETURN O2JB_DLL SQLStatistics(
     SQLHSTMT        StatementHandle,
     SQLCHAR *       CatalogName,
     SQLSMALLINT     NameLength1,
     SQLCHAR *       SchemaName,
     SQLSMALLINT     NameLength2,
     SQLCHAR *       TableName,
     SQLSMALLINT     NameLength3,
     SQLUSMALLINT    Unique,
     SQLUSMALLINT    Reserved);

SQLRETURN O2JB_DLL SQLTablePrivileges(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CatalogName,
     SQLSMALLINT   NameLength1,
     SQLCHAR *     SchemaName,
     SQLSMALLINT   NameLength2,
     SQLCHAR *     TableName,
     SQLSMALLINT   NameLength3);


SQLRETURN O2JB_DLL SQLTables(
     SQLHSTMT       StatementHandle,
     SQLCHAR *      CatalogName,
     SQLSMALLINT    NameLength1,
     SQLCHAR *      SchemaName,
     SQLSMALLINT    NameLength2,
     SQLCHAR *      TableName,
     SQLSMALLINT    NameLength3,
     SQLCHAR *      TableType,
     SQLSMALLINT    NameLength4);

// SQLTransact see SQLEndTran
#ifdef __cplusplus
}
#endif

#endif