#ifndef _INCLUDE_GUARD_O2JB_SQL_H_
#define _INCLUDE_GUARD_O2JB_SQL_H_ 1

#include <sqltypes.h>

//undef SQL_API
//ifdef BUILDING_O2JB_DLL
//define SQL_API __declspec(dllexport)
//else
//define SQL_API __declspec(dllimport)
//endif

//include <sql.h>

#define SQL_DRIVER_ODBC_VER 77

#define SQL_NTS (-3)
#define SQL_HANDLE_ENV 1
#define SQL_HANDLE_DBC 2
#define SQL_HANDLE_STMT 3
#define SQL_HANDLE_DESC 4

#define SQL_CLOSE 0
#define SQL_DROP 1
#define SQL_UNBIND 2
#define SQL_RESET_PARAMS 3

#define SQL_FETCH_NEXT 1
#define SQL_FETCH_FIRST 2

#define SQL_FETCH_LAST 3
#define SQL_FETCH_PRIOR 4
#define SQL_FETCH_ABSOLUTE 5
#define SQL_FETCH_RELATIVE 6

#define SQL_COMMIT 0
#define SQL_ROLLBACK 1

#define SQL_NULL_HENV 0
#define SQL_NULL_HDBC 0
#define SQL_NULL_HSTMT 0

#define SQL_SUCCESS 0
#define SQL_SUCCESS_WITH_INFO 1
#define SQL_NO_DATA 100
#define SQL_ERROR (-1)
#define SQL_INVALID_HANDLE (-2)

#define SQL_COLUMN_COUNT 0
#define SQL_COLUMN_NAME 1
#define SQL_COLUMN_TYPE 2
#define SQL_COLUMN_LENGTH 3
#define SQL_COLUMN_PRECISION 4
#define SQL_COLUMN_SCALE 5
#define SQL_COLUMN_DISPLAY_SIZE 6
#define SQL_COLUMN_NULLABLE 7
#define SQL_COLUMN_UNSIGNED 8
#define SQL_COLUMN_MONEY 9
#define SQL_COLUMN_UPDATABLE 10
#define SQL_COLUMN_AUTO_INCREMENT 11
#define SQL_COLUMN_CASE_SENSITIVE 12
#define SQL_COLUMN_SEARCHABLE 13
#define SQL_COLUMN_TYPE_NAME 14
#define SQL_COLUMN_TABLE_NAME 15
#define SQL_COLUMN_OWNER_NAME 16
#define SQL_COLUMN_QUALIFIER_NAME 17
#define SQL_COLUMN_LABEL 18
#define SQL_COLATT_OPT_MAX SQL_COLUMN_LABEL

#define SQL_DIAG_RETURNCODE 1
#define SQL_DIAG_NUMBER 2
#define SQL_DIAG_ROW_COUNT 3
#define SQL_DIAG_SQLSTATE 4
#define SQL_DIAG_NATIVE 5
#define SQL_DIAG_MESSAGE_TEXT 6
#define SQL_DIAG_DYNAMIC_FUNCTION 7
#define SQL_DIAG_CLASS_ORIGIN 8
#define SQL_DIAG_SUBCLASS_ORIGIN 9
#define SQL_DIAG_CONNECTION_NAME 10
#define SQL_DIAG_SERVER_NAME 11
#define SQL_DIAG_DYNAMIC_FUNCTION_CODE 12

#define SQL_FALSE 0
#define SQL_TRUE 1

#define SQL_UNKNOWN_TYPE 0
#define SQL_CHAR 1
#define SQL_NUMERIC 2
#define SQL_DECIMAL 3
#define SQL_INTEGER 4
#define SQL_SMALLINT 5
#define SQL_FLOAT 6
#define SQL_REAL 7
#define SQL_DOUBLE 8

#define SQL_C_CHAR SQL_CHAR
#define SQL_C_LONG SQL_INTEGER
#define SQL_C_SHORT SQL_SMALLINT
#define SQL_C_FLOAT SQL_REAL
#define SQL_C_DOUBLE SQL_DOUBLE

#define SQL_DESC_CONCISE_TYPE SQL_COLUMN_TYPE
#define SQL_DESC_DATETIME_INTERVAL_PRECISION 26
#define SQL_DESC_DISPLAY_SIZE SQL_COLUMN_DISPLAY_SIZE
#define SQL_DESC_COUNT 1001
#define SQL_DESC_TYPE 1002
#define SQL_DESC_LENGTH 1003
#define SQL_DESC_OCTET_LENGTH_PTR 1004
#define SQL_DESC_PRECISION 1005
#define SQL_DESC_SCALE 1006
#define SQL_DESC_DATETIME_INTERVAL_CODE 1007
#define SQL_DESC_NULLABLE 1008
#define SQL_DESC_INDICATOR_PTR 1009
#define SQL_DESC_DATA_PTR 1010
#define SQL_DESC_NAME 1011
#define SQL_DESC_UNNAMED 1012
#define SQL_DESC_OCTET_LENGTH 1013
#define SQL_DESC_ALLOC_TYPE 1099

#define SQL_ATTR_OUTPUT_NTS 10001

#define SQL_ATTR_ODBC_VERSION 200
#define SQL_ATTR_CONNECTION_POOLING 201
#define SQL_ATTR_CP_MATCH 202

#define SQL_CP_OFF __MSABI_LONG(0U)
#define SQL_CP_ONE_PER_DRIVER __MSABI_LONG(1U)
#define SQL_CP_ONE_PER_HENV __MSABI_LONG(2U)
#define SQL_CP_DEFAULT SQL_CP_OFF

#define SQL_CP_STRICT_MATCH __MSABI_LONG(0U)
#define SQL_CP_RELAXED_MATCH __MSABI_LONG(1U)
#define SQL_CP_MATCH_DEFAULT SQL_CP_STRICT_MATCH

#define SQL_OV_ODBC2 __MSABI_LONG(2U)
#define SQL_OV_ODBC3 __MSABI_LONG(3U)

#define SQL_DATABASE_NAME 16
#define SQL_FD_FETCH_PREV SQL_FD_FETCH_PRIOR
#define SQL_FETCH_PREV SQL_FETCH_PRIOR
#define SQL_CONCUR_TIMESTAMP SQL_CONCUR_ROWVER
#define SQL_SCCO_OPT_TIMESTAMP SQL_SCCO_OPT_ROWVER
#define SQL_CC_DELETE SQL_CB_DELETE
#define SQL_CR_DELETE SQL_CB_DELETE
#define SQL_CC_CLOSE SQL_CB_CLOSE
#define SQL_CR_CLOSE SQL_CB_CLOSE
#define SQL_CC_PRESERVE SQL_CB_PRESERVE
#define SQL_CR_PRESERVE SQL_CB_PRESERVE

#define SQL_SCROLL_FORWARD_ONLY __MSABI_LONG(0)
#define SQL_SCROLL_KEYSET_DRIVEN (__MSABI_LONG(-1))
#define SQL_SCROLL_DYNAMIC (__MSABI_LONG(-2))
#define SQL_SCROLL_STATIC (__MSABI_LONG(-3))

#define SQL_MAX_DRIVER_CONNECTIONS 0
#define SQL_MAXIMUM_DRIVER_CONNECTIONS SQL_MAX_DRIVER_CONNECTIONS
#define SQL_MAX_CONCURRENT_ACTIVITIES 1
#define SQL_MAXIMUM_CONCURRENT_ACTIVITIES SQL_MAX_CONCURRENT_ACTIVITIES
#define SQL_DATA_SOURCE_NAME 2
#define SQL_FETCH_DIRECTION 8
#define SQL_SERVER_NAME 13
#define SQL_SEARCH_PATTERN_ESCAPE 14
#define SQL_DBMS_NAME 17
#define SQL_DBMS_VER 18
#define SQL_ACCESSIBLE_TABLES 19
#define SQL_ACCESSIBLE_PROCEDURES 20
#define SQL_CURSOR_COMMIT_BEHAVIOR 23
#define SQL_DATA_SOURCE_READ_ONLY 25
#define SQL_DEFAULT_TXN_ISOLATION 26
#define SQL_IDENTIFIER_CASE 28
#define SQL_IDENTIFIER_QUOTE_CHAR 29
#define SQL_MAX_COLUMN_NAME_LEN 30
#define SQL_MAXIMUM_COLUMN_NAME_LENGTH SQL_MAX_COLUMN_NAME_LEN
#define SQL_MAX_CURSOR_NAME_LEN 31
#define SQL_MAXIMUM_CURSOR_NAME_LENGTH SQL_MAX_CURSOR_NAME_LEN
#define SQL_MAX_SCHEMA_NAME_LEN 32
#define SQL_MAXIMUM_SCHEMA_NAME_LENGTH SQL_MAX_SCHEMA_NAME_LEN
#define SQL_MAX_CATALOG_NAME_LEN 34
#define SQL_MAXIMUM_CATALOG_NAME_LENGTH SQL_MAX_CATALOG_NAME_LEN
#define SQL_MAX_TABLE_NAME_LEN 35
#define SQL_SCROLL_CONCURRENCY 43
#define SQL_TXN_CAPABLE 46
#define SQL_TRANSACTION_CAPABLE SQL_TXN_CAPABLE
#define SQL_USER_NAME 47
#define SQL_TXN_ISOLATION_OPTION 72
#define SQL_TRANSACTION_ISOLATION_OPTION SQL_TXN_ISOLATION_OPTION
#define SQL_INTEGRITY 73
#define SQL_GETDATA_EXTENSIONS 81
#define SQL_NULL_COLLATION 85
#define SQL_ALTER_TABLE 86
#define SQL_ORDER_BY_COLUMNS_IN_SELECT 90
#define SQL_SPECIAL_CHARACTERS 94
#define SQL_MAX_COLUMNS_IN_GROUP_BY 97
#define SQL_MAXIMUM_COLUMNS_IN_GROUP_BY SQL_MAX_COLUMNS_IN_GROUP_BY
#define SQL_MAX_COLUMNS_IN_INDEX 98
#define SQL_MAXIMUM_COLUMNS_IN_INDEX SQL_MAX_COLUMNS_IN_INDEX
#define SQL_MAX_COLUMNS_IN_ORDER_BY 99
#define SQL_MAXIMUM_COLUMNS_IN_ORDER_BY SQL_MAX_COLUMNS_IN_ORDER_BY
#define SQL_MAX_COLUMNS_IN_SELECT 100
#define SQL_MAXIMUM_COLUMNS_IN_SELECT SQL_MAX_COLUMNS_IN_SELECT
#define SQL_MAX_COLUMNS_IN_TABLE 101
#define SQL_MAX_INDEX_SIZE 102
#define SQL_MAXIMUM_INDEX_SIZE SQL_MAX_INDEX_SIZE
#define SQL_MAX_ROW_SIZE 104
#define SQL_MAXIMUM_ROW_SIZE SQL_MAX_ROW_SIZE
#define SQL_MAX_STATEMENT_LEN 105
#define SQL_MAXIMUM_STATEMENT_LENGTH SQL_MAX_STATEMENT_LEN
#define SQL_MAX_TABLES_IN_SELECT 106
#define SQL_MAXIMUM_TABLES_IN_SELECT SQL_MAX_TABLES_IN_SELECT
#define SQL_MAX_USER_NAME_LEN 107
#define SQL_MAXIMUM_USER_NAME_LENGTH SQL_MAX_USER_NAME_LEN
#define SQL_OJ_CAPABILITIES 115
#define SQL_OUTER_JOIN_CAPABILITIES SQL_OJ_CAPABILITIES

#define SQL_ALL_CATALOGS "%"
#define SQL_ALL_SCHEMAS "%"
#define SQL_ALL_TABLE_TYPES "%"

#endif