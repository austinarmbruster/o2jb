#include <windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

#include <iostream>
#include <string>

#include <fstream>

#include <cstdlib>

using std::ifstream;
using std::string; using std::cout; using std::endl;

// using o2jb::filesystem::exist;

typedef BOOL (*PtrCfgDriver)(
  HWND    hwndParent,
  WORD    fRequest,
  LPCSTR  lpszDriver,
  LPCSTR  lpszArgs,
  LPSTR   lpszMsg,
  WORD    cbMsgMax,
  WORD *  pcbMsgOut);

typedef BOOL (*PtrCfgDsn)(
  HWND     hwndParent,
  WORD     fRequest,
  LPCSTR   lpszDriver,
  LPCSTR   lpszAttributes);

typedef SQLRETURN (*PtrSQLDriverConnect)(
  SQLHDBC         connHandle,
  SQLHWND         windowHandle,
  SQLCHAR *       inConnectionString,
  SQLSMALLINT     stringLength1,
  SQLCHAR *       outConnectionString,
  SQLSMALLINT     bufferLength,
  SQLSMALLINT *   stringLength2Ptr,
  SQLUSMALLINT    driverCompletion);

typedef SQLRETURN (*PtrSQLSetEnvAttr)(
  SQLHENV      envHandle,
  SQLINTEGER   attribute,
  SQLPOINTER   valuePtr,
  SQLINTEGER   stringLength);

typedef SQLRETURN (* PtrSQLAllocHandle)(SQLSMALLINT handleType, SQLHANDLE inputHandle, SQLHANDLE *outHandle);

typedef SQLRETURN (* PtrSQLFreeStmt)(
  SQLHSTMT       StatementHandle,
  SQLUSMALLINT   Option);

typedef SQLRETURN (* PtrSQLExecDirect)(
  SQLHSTMT     StatementHandle,
  SQLCHAR *    StatementText,
  SQLINTEGER   TextLength);

typedef SQLRETURN (* PtrSQLRowCount)(
  SQLHSTMT   StatementHandle,
  SQLLEN *   RowCountPtr);

typedef SQLRETURN (* PtrSQLNumResultCols)(
     SQLHSTMT        StatementHandle,
     SQLSMALLINT *   ColumnCountPtr);

void addDsn() {
  HINSTANCE hinstLib = LoadLibrary(TEXT(".\\o2jb.dll"));
  cout << "Library Loaded" << endl;

  PtrCfgDsn ptrCfg = (PtrCfgDsn)GetProcAddress(hinstLib, "ConfigDSN");
  HWND win = GetDesktopWindow();
  ptrCfg(win, ODBC_ADD_DSN, "O2JB Driver 32", NULL);
}

void connectDll() {
  try {
    HENV  lpEnv = NULL;
    HDBC  lpDbc = NULL;
    HSTMT lpStmt = NULL;
    TCHAR *pszConnStr = (TCHAR *)"DSN=0.32";

    HINSTANCE hinstLib = LoadLibrary(TEXT(".\\o2jb.dll"));
    cout << "Library Loaded" << endl;
    PtrSQLAllocHandle ptrAlloc = (PtrSQLAllocHandle)GetProcAddress(hinstLib, "SQLAllocHandle");
    PtrSQLSetEnvAttr ptrSetEnv = (PtrSQLSetEnvAttr)GetProcAddress(hinstLib, "SQLSetEnvAttr");
    PtrSQLDriverConnect ptrConn = (PtrSQLDriverConnect)GetProcAddress(hinstLib, "SQLDriverConnect");
    PtrSQLExecDirect ptrExecDirect = (PtrSQLExecDirect)GetProcAddress(hinstLib, "SQLExecDirect");
    PtrSQLFreeStmt ptrFreeStmt = (PtrSQLFreeStmt)GetProcAddress(hinstLib, "SQLFreeStmt");
    PtrSQLRowCount ptrRowCount = (PtrSQLRowCount)GetProcAddress(hinstLib, "SQLRowCount");
    PtrSQLNumResultCols ptrNumResultCols = (PtrSQLNumResultCols)GetProcAddress(hinstLib, "SQLNumResultCols");

    HWND win = GetDesktopWindow();

    cout << "1" << endl;
    SQLRETURN funcRet = ptrAlloc(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &lpEnv);
    if (SQL_SUCCESS != funcRet) {
      cout << "failed to " << "alloc env" << endl;
      return;
    }
    cout << "2" << endl;
    funcRet = ptrSetEnv(lpEnv,
                        SQL_ATTR_ODBC_VERSION,
                        (SQLPOINTER)SQL_OV_ODBC2,
                        0);
    if (SQL_SUCCESS != funcRet) {
      cout << "failed to " << "set env" << endl;
      return;
    }
    cout << "3" << endl;
    funcRet = ptrAlloc(SQL_HANDLE_DBC, lpEnv, &lpDbc);
    if (SQL_SUCCESS != funcRet) {
      cout << "failed to " << "alloc dbc" << endl;
      return;
    }
    cout << "lpDbc:  " << lpDbc << endl;
    cout << "4" << endl;
    funcRet = ptrConn(lpDbc,
                      win,
                      (SQLCHAR*) pszConnStr,
                      SQL_NTS,
                      NULL,
                      0,
                      NULL,
                      SQL_DRIVER_COMPLETE);
    if (SQL_SUCCESS != funcRet) {
      cout << "failed to " << "connect" << endl;
      return;
    }
    cout << "5" << endl;
    funcRet = ptrAlloc(SQL_HANDLE_STMT, lpDbc, &lpStmt);
    if (SQL_SUCCESS != funcRet) {
      cout << "failed to " << "alloc stmt" << endl;
      return;
    }
    cout << "6" << endl;
    char const* input = "select * from data";
    cout << "lpStmt:  " << lpStmt << endl;
    funcRet = ptrExecDirect(lpStmt, (SQLCHAR*) input, SQL_NTS);
    if (SQL_SUCCESS != funcRet) {
      cout << "failed to " << "exec query:  " << funcRet << " " << SQL_SUCCESS << " " << SQL_ERROR << endl;
      return;
    }
    SQLLEN    siRowCount = 0;
    cout << "7" << endl;
    cout << "p: " << ptrExecDirect << " " << ptrRowCount << " r:  " << siRowCount << endl;
    cout << "lpStmt:  " << lpStmt << endl;
    funcRet = ptrRowCount(lpStmt, &siRowCount);
    if (SQL_SUCCESS != funcRet) {
      cout << "failed to " << "get row count" << endl;
      return;
    }
    cout << "Affected rows:  " << siRowCount << endl;

    SQLSMALLINT sNumCols;
    funcRet = ptrNumResultCols(lpStmt, &sNumCols);
    if (SQL_SUCCESS != funcRet) {
      cout << "failed to " << "get num result cols" << endl;
      return;
    }
    cout << "Num cols:  " << sNumCols << endl;

    funcRet = ptrFreeStmt(lpStmt, SQL_CLOSE);
    if (SQL_SUCCESS != funcRet) {
      cout << "failed to " << "free stmt" << endl;
      return;
    }
    cout << "10" << endl;
  } catch (std::exception& e) {
    cout << "Caught:  " << e.what() << endl;
  }
}

int main(int argc, char** args) {

  if (argc > 1) {
    addDsn();
  } else {
    connectDll();
  }

  return 0;

}