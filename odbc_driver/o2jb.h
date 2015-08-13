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

#define ADD_CALL __cdecl

O2JB_DLL SQLRETURN ADD_CALL SQLAllocHandle(SQLSMALLINT handleType, SQLHANDLE inputHandle, SQLHANDLE *outHandle);

O2JB_DLL SQLRETURN ADD_CALL SQLBindCol(
      SQLHSTMT       StatementHandle,
      SQLUSMALLINT   ColumnNumber,
      SQLSMALLINT    TargetType,
      SQLPOINTER     TargetValuePtr,
      SQLLEN         BufferLength,
      SQLLEN *       StrLen_or_Ind);

O2JB_DLL SQLRETURN ADD_CALL SQLBindParameter(
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

O2JB_DLL SQLRETURN ADD_CALL SQLBrowseConnect(
     SQLHDBC         ConnectionHandle,
     SQLCHAR *       InConnectionString,
     SQLSMALLINT     StringLength1,
     SQLCHAR *       OutConnectionString,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   StringLength2Ptr);

// O2JB_DLL SQLRETURN ADD_CALL SQLBulkOperations(
//      SQLHSTMT       StatementHandle,
//      SQLUSMALLINT   Operation);

O2JB_DLL SQLRETURN ADD_CALL SQLCancel(
     SQLHSTMT     StatementHandle);

O2JB_DLL SQLRETURN ADD_CALL SQLCloseCursor(
     SQLHSTMT     StatementHandle);

O2JB_DLL SQLRETURN ADD_CALL SQLColAttribute (
      SQLHSTMT        StatementHandle,
      SQLUSMALLINT    ColumnNumber,
      SQLUSMALLINT    FieldIdentifier,
      SQLPOINTER      CharacterAttributePtr,
      SQLSMALLINT     BufferLength,
      SQLSMALLINT *   StringLengthPtr,
      SQLLEN *        NumericAttributePtr);

O2JB_DLL SQLRETURN ADD_CALL SQLColumnPrivileges(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CatalogName,
     SQLSMALLINT   NameLength1,
     SQLCHAR *     SchemaName,
     SQLSMALLINT   NameLength2,
     SQLCHAR *     TableName,
     SQLSMALLINT   NameLength3,
     SQLCHAR *     ColumnName,
     SQLSMALLINT   NameLength4);

O2JB_DLL SQLRETURN ADD_CALL SQLColumns(
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
O2JB_DLL SQLRETURN ADD_CALL SQLConnect(
     SQLHDBC        ConnectionHandle,
     SQLCHAR *      ServerName,
     SQLSMALLINT    NameLength1,
     SQLCHAR *      UserName,
     SQLSMALLINT    NameLength2,
     SQLCHAR *      Authentication,
     SQLSMALLINT    NameLength3);

O2JB_DLL SQLRETURN ADD_CALL SQLCopyDesc(
     SQLHDESC     SourceDescHandle,
     SQLHDESC     TargetDescHandle);

O2JB_DLL SQLRETURN ADD_CALL SQLDataSources(
     SQLHENV          EnvironmentHandle,
     SQLUSMALLINT     Direction,
     SQLCHAR *        ServerName,
     SQLSMALLINT      BufferLength1,
     SQLSMALLINT *    NameLength1Ptr,
     SQLCHAR *        Description,
     SQLSMALLINT      BufferLength2,
     SQLSMALLINT *    NameLength2Ptr);

O2JB_DLL SQLRETURN ADD_CALL SQLDescribeCol(
      SQLHSTMT       StatementHandle,
      SQLUSMALLINT   ColumnNumber,
      SQLCHAR *      ColumnName,
      SQLSMALLINT    BufferLength,
      SQLSMALLINT *  NameLengthPtr,
      SQLSMALLINT *  DataTypePtr,
      SQLULEN *      ColumnSizePtr,
      SQLSMALLINT *  DecimalDigitsPtr,
      SQLSMALLINT *  NullablePtr);


O2JB_DLL SQLRETURN ADD_CALL SQLDescribeParam(
      SQLHSTMT        StatementHandle,
      SQLUSMALLINT    ParameterNumber,
      SQLSMALLINT *   DataTypePtr,
      SQLULEN *       ParameterSizePtr,
      SQLSMALLINT *   DecimalDigitsPtr,
      SQLSMALLINT *   NullablePtr);

// Already Exist?
O2JB_DLL SQLRETURN ADD_CALL SQLDisconnect(
     SQLHDBC     ConnectionHandle);

// Already Exist?
O2JB_DLL SQLRETURN ADD_CALL SQLDriverConnect(
     SQLHDBC         ConnectionHandle,
     SQLHWND         WindowHandle,
     SQLCHAR *       InConnectionString,
     SQLSMALLINT     StringLength1,
     SQLCHAR *       OutConnectionString,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   StringLength2Ptr,
     SQLUSMALLINT    DriverCompletion);

O2JB_DLL SQLRETURN ADD_CALL SQLEndTran(
     SQLSMALLINT   HandleType,
     SQLHANDLE     Handle,
     SQLSMALLINT   CompletionType);

// Already Exist?
O2JB_DLL SQLRETURN ADD_CALL SQLExecDirect(
     SQLHSTMT     StatementHandle,
     SQLCHAR *    StatementText,
     SQLINTEGER   TextLength);

O2JB_DLL SQLRETURN ADD_CALL SQLExecute(
     SQLHSTMT     StatementHandle);

O2JB_DLL SQLRETURN ADD_CALL SQLExtendedFetch(
      SQLHSTMT         StatementHandle,
      SQLUSMALLINT     FetchOrientation,
      SQLLEN           FetchOffset,
      SQLULEN *        RowCountPtr,
      SQLUSMALLINT *   RowStatusArray);

// Already Exist?
O2JB_DLL SQLRETURN ADD_CALL SQLFetch(
     SQLHSTMT     StatementHandle);

O2JB_DLL SQLRETURN ADD_CALL SQLFetchScroll(
      SQLHSTMT      StatementHandle,
      SQLSMALLINT   FetchOrientation,
      SQLLEN        FetchOffset);

O2JB_DLL SQLRETURN ADD_CALL SQLForeignKeys(
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
O2JB_DLL SQLRETURN ADD_CALL SQLFreeHandle(
     SQLSMALLINT   HandleType,
     SQLHANDLE     Handle);

// Already Exist?
O2JB_DLL SQLRETURN ADD_CALL SQLFreeStmt(
     SQLHSTMT       StatementHandle,
     SQLUSMALLINT   Option);


O2JB_DLL SQLRETURN ADD_CALL SQLGetConnectAttr(
     SQLHDBC        ConnectionHandle,
     SQLINTEGER     Attribute,
     SQLPOINTER     ValuePtr,
     SQLINTEGER     BufferLength,
     SQLINTEGER *   StringLengthPtr);

// SQLGetConnectOption


O2JB_DLL SQLRETURN ADD_CALL SQLGetCursorName(
     SQLHSTMT        StatementHandle,
     SQLCHAR *       CursorName,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   NameLengthPtr);

O2JB_DLL SQLRETURN ADD_CALL SQLGetData(
      SQLHSTMT       StatementHandle,
      SQLUSMALLINT   Col_or_Param_Num,
      SQLSMALLINT    TargetType,
      SQLPOINTER     TargetValuePtr,
      SQLLEN         BufferLength,
      SQLLEN *       StrLen_or_IndPtr);

O2JB_DLL SQLRETURN ADD_CALL SQLGetData(
      SQLHSTMT       StatementHandle,
      SQLUSMALLINT   Col_or_Param_Num,
      SQLSMALLINT    TargetType,
      SQLPOINTER     TargetValuePtr,
      SQLLEN         BufferLength,
      SQLLEN *       StrLen_or_IndPtr);

O2JB_DLL SQLRETURN ADD_CALL SQLGetDescRec(
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

O2JB_DLL SQLRETURN ADD_CALL SQLGetDiagField(
     SQLSMALLINT     HandleType,
     SQLHANDLE       Handle,
     SQLSMALLINT     RecNumber,
     SQLSMALLINT     DiagIdentifier,
     SQLPOINTER      DiagInfoPtr,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   StringLengthPtr);

O2JB_DLL SQLRETURN ADD_CALL SQLGetDiagRec(
     SQLSMALLINT     HandleType,
     SQLHANDLE       Handle,
     SQLSMALLINT     RecNumber,
     SQLCHAR *       SQLState,
     SQLINTEGER *    NativeErrorPtr,
     SQLCHAR *       MessageText,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   TextLengthPtr);

O2JB_DLL SQLRETURN ADD_CALL SQLGetEnvAttr(
     SQLHENV        EnvironmentHandle,
     SQLINTEGER     Attribute,
     SQLPOINTER     ValuePtr,
     SQLINTEGER     BufferLength,
     SQLINTEGER *   StringLengthPtr);

O2JB_DLL SQLRETURN ADD_CALL SQLGetFunctions(
     SQLHDBC           ConnectionHandle,
     SQLUSMALLINT      FunctionId,
     SQLUSMALLINT *    SupportedPtr);

O2JB_DLL SQLRETURN ADD_CALL SQLGetInfo(
     SQLHDBC         ConnectionHandle,
     SQLUSMALLINT    InfoType,
     SQLPOINTER      InfoValuePtr,
     SQLSMALLINT     BufferLength,
     SQLSMALLINT *   StringLengthPtr);


O2JB_DLL SQLRETURN ADD_CALL SQLGetStmtAttr(
     SQLHSTMT        StatementHandle,
     SQLINTEGER      Attribute,
     SQLPOINTER      ValuePtr,
     SQLINTEGER      BufferLength,
     SQLINTEGER *    StringLengthPtr);

// SQLGetStmtOption


O2JB_DLL SQLRETURN ADD_CALL SQLGetTypeInfo(
     SQLHSTMT      StatementHandle,
     SQLSMALLINT   DataType);

O2JB_DLL SQLRETURN ADD_CALL SQLMoreResults(
     SQLHSTMT     StatementHandle);

O2JB_DLL SQLRETURN ADD_CALL SQLNativeSql(
     SQLHDBC        ConnectionHandle,
     SQLCHAR *      InStatementText,
     SQLINTEGER     TextLength1,
     SQLCHAR *      OutStatementText,
     SQLINTEGER     BufferLength,
     SQLINTEGER *   TextLength2Ptr);


O2JB_DLL SQLRETURN ADD_CALL SQLNumParams(
     SQLHSTMT        StatementHandle,
     SQLSMALLINT *   ParameterCountPtr);

O2JB_DLL SQLRETURN ADD_CALL SQLNumResultCols(
     SQLHSTMT        StatementHandle,
     SQLSMALLINT *   ColumnCountPtr);

O2JB_DLL SQLRETURN ADD_CALL SQLParamData(
     SQLHSTMT       StatementHandle,
     SQLPOINTER *   ValuePtrPtr);

// SQLParamOptions 


O2JB_DLL SQLRETURN ADD_CALL SQLPrepare(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     StatementText,
     SQLINTEGER    TextLength);

O2JB_DLL SQLRETURN ADD_CALL SQLPrimaryKeys(
     SQLHSTMT       StatementHandle,
     SQLCHAR *      CatalogName,
     SQLSMALLINT    NameLength1,
     SQLCHAR *      SchemaName,
     SQLSMALLINT    NameLength2,
     SQLCHAR *      TableName,
     SQLSMALLINT    NameLength3);


O2JB_DLL SQLRETURN ADD_CALL SQLProcedureColumns(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CatalogName,
     SQLSMALLINT   NameLength1,
     SQLCHAR *     SchemaName,
     SQLSMALLINT   NameLength2,
     SQLCHAR *     ProcName,
     SQLSMALLINT   NameLength3,
     SQLCHAR *     ColumnName,
     SQLSMALLINT   NameLength4);


O2JB_DLL SQLRETURN ADD_CALL SQLProcedures(
     SQLHSTMT       StatementHandle,
     SQLCHAR *      CatalogName,
     SQLSMALLINT    NameLength1,
     SQLCHAR *      SchemaName,
     SQLSMALLINT    NameLength2,
     SQLCHAR *      ProcName,
     SQLSMALLINT    NameLength3);

O2JB_DLL SQLRETURN ADD_CALL SQLPutData(
      SQLHSTMT     StatementHandle,
      SQLPOINTER   DataPtr,
      SQLLEN       StrLen_or_Ind);

O2JB_DLL SQLRETURN ADD_CALL SQLRowCount(
      SQLHSTMT   StatementHandle,
      SQLLEN *   RowCountPtr);

// SQLSetConnectOption 

O2JB_DLL SQLRETURN ADD_CALL SQLSetCursorName(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CursorName,
     SQLSMALLINT   NameLength);

O2JB_DLL SQLRETURN ADD_CALL SQLSetDescField(
     SQLHDESC      DescriptorHandle,
     SQLSMALLINT   RecNumber,
     SQLSMALLINT   FieldIdentifier,
     SQLPOINTER    ValuePtr,
     SQLINTEGER    BufferLength);

O2JB_DLL SQLRETURN ADD_CALL SQLSetDescRec(
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


O2JB_DLL SQLRETURN ADD_CALL SQLSetEnvAttr(
     SQLHENV      EnvironmentHandle,
     SQLINTEGER   Attribute,
     SQLPOINTER   ValuePtr,
     SQLINTEGER   StringLength);

// SQLSetParam see SQLBindParameter

O2JB_DLL SQLRETURN ADD_CALL SQLSetPos(
      SQLHSTMT        StatementHandle,
      SQLSETPOSIROW   RowNumber,
      SQLUSMALLINT    Operation,
      SQLUSMALLINT    LockType);

// SQLSetScrollOptions see SQLGetInfo and SQLSetStmtAttr


O2JB_DLL SQLRETURN ADD_CALL SQLSetStmtAttr(
     SQLHSTMT      StatementHandle,
     SQLINTEGER    Attribute,
     SQLPOINTER    ValuePtr,
     SQLINTEGER    StringLength);

// SQLSetStmtOption see SQLSetStmtAttr

// O2JB_DLL SQLRETURN ADD_CALL SQLSpecialColumns(
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

O2JB_DLL SQLRETURN ADD_CALL SQLStatistics(
     SQLHSTMT        StatementHandle,
     SQLCHAR *       CatalogName,
     SQLSMALLINT     NameLength1,
     SQLCHAR *       SchemaName,
     SQLSMALLINT     NameLength2,
     SQLCHAR *       TableName,
     SQLSMALLINT     NameLength3,
     SQLUSMALLINT    Unique,
     SQLUSMALLINT    Reserved);

O2JB_DLL SQLRETURN ADD_CALL SQLTablePrivileges(
     SQLHSTMT      StatementHandle,
     SQLCHAR *     CatalogName,
     SQLSMALLINT   NameLength1,
     SQLCHAR *     SchemaName,
     SQLSMALLINT   NameLength2,
     SQLCHAR *     TableName,
     SQLSMALLINT   NameLength3);


O2JB_DLL SQLRETURN ADD_CALL SQLTables(
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