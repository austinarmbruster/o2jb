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
#include <sqlext.h>
#include <strsafe.h>

// simple helper functions
int MySQLSuccess(SQLRETURN rc) {
   return (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO);
}

struct DataBinding {
   SQLSMALLINT TargetType;
   SQLPOINTER TargetValuePtr;
   SQLINTEGER BufferLength;
   SQLLEN StrLen_or_Ind;
};

void printCatalog(const struct DataBinding* catalogResult) {
   if (catalogResult[0].StrLen_or_Ind != SQL_NULL_DATA)
      printf("Catalog Name = %s\n", (char *)catalogResult[0].TargetValuePtr);
}

void printSchema(const struct DataBinding* catalogResult) {
   if (catalogResult[1].StrLen_or_Ind != SQL_NULL_DATA)
      printf("Schema Name = %s\n", (char *)catalogResult[1].TargetValuePtr);
}

void printTable(const struct DataBinding* catalogResult) {
   if (catalogResult[2].StrLen_or_Ind != SQL_NULL_DATA)
      printf("Table Name = %s\n", (char *)catalogResult[2].TargetValuePtr);
}

// remember to disconnect and free memory, and free statements and handles
int main() {
   int bufferSize = 1024, i, numCols = 5;
   struct DataBinding* catalogResult = (struct DataBinding*) malloc( numCols * sizeof(struct DataBinding) );
   wchar_t* dbName = (wchar_t *)malloc( sizeof(wchar_t) * bufferSize );
   wchar_t* userName = (wchar_t *)malloc( sizeof(wchar_t) * bufferSize );

   // declare and initialize the environment, connection, statement handles
   SQLHENV henv = NULL;   // Environment
   SQLHDBC hdbc = NULL;   // Connection handle
   SQLHSTMT hstmt = NULL;   // Statement handle

   SQLRETURN retCode;

   retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
   retCode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, -1);
   retCode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
   retCode = SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)10, 0);
//   retCode = SQLDriverConnect(hdbc, desktopHandle, (SQLCHAR*)"Driver={SQL Server}", SQL_NTS, (SQLCHAR*)connStrbuffer, 1024 + 1, &connStrBufferLen, SQL_DRIVER_PROMPT);
   retCode = SQLDriverConnect(hdbc, GetDesktopWindow(), NULL, SQL_NTS, NULL, 0, NULL,
                              SQL_DRIVER_COMPLETE);

   retCode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
   retCode = SQLGetInfo(hdbc, SQL_DATABASE_NAME, dbName, (SQLSMALLINT)bufferSize, (SQLSMALLINT *)&bufferSize);
   retCode = SQLGetInfo(hdbc, SQL_USER_NAME, userName, (SQLSMALLINT)bufferSize, (SQLSMALLINT *)&bufferSize);

   bufferSize = 1024;

   // allocate memory for the binding
   // free this memory when done
   for ( i = 0 ; i < numCols ; i++ ) {
      catalogResult[i].TargetType = SQL_C_CHAR;
      catalogResult[i].BufferLength = (bufferSize + 1);
      catalogResult[i].TargetValuePtr = malloc( sizeof(unsigned char) * catalogResult[i].BufferLength );
   }

   // setup the binding (can be used even if the statement is closed by closeStatementHandle)
   for ( i = 0 ; i < numCols ; i++ )
      retCode = SQLBindCol(hstmt, (SQLUSMALLINT)i + 1, catalogResult[i].TargetType, catalogResult[i].TargetValuePtr, catalogResult[i].BufferLength, &(catalogResult[i].StrLen_or_Ind));

   // all catalogs query
   printf( "A list of names of all catalogs\n" );
   retCode = SQLTables( hstmt, (SQLCHAR*)SQL_ALL_CATALOGS, SQL_NTS, (SQLCHAR*)"", SQL_NTS, (SQLCHAR*)"", SQL_NTS, (SQLCHAR*)"", SQL_NTS );
   for ( retCode = SQLFetch(hstmt) ;  MySQLSuccess(retCode) ; retCode = SQLFetch(hstmt) ) {
      printCatalog( catalogResult );
   }

   printf( "A list of names of all tables in catalog test\n" );
   retCode = SQLTables( hstmt, (SQLCHAR*)"TEST", SQL_NTS, NULL, SQL_NTS, (SQLCHAR*)NULL, SQL_NTS, (SQLCHAR*)"TABLE", SQL_NTS );
   for ( retCode = SQLFetch(hstmt) ;  MySQLSuccess(retCode) ; retCode = SQLFetch(hstmt) ) {
      printTable( catalogResult );
   }


   printf( "A list of names of all tables\n" );
   retCode = SQLTables( hstmt, NULL, SQL_NTS, NULL, SQL_NTS, (SQLCHAR*)NULL, SQL_NTS, (SQLCHAR*)"TABLE", SQL_NTS );
   for ( retCode = SQLFetch(hstmt) ;  MySQLSuccess(retCode) ; retCode = SQLFetch(hstmt) ) {
      printTable( catalogResult );
   }

}