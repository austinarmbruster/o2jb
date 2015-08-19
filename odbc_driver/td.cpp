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
    TCHAR *pszConnStr = (TCHAR *)"DSN=5a32";

    HINSTANCE hinstLib = LoadLibrary(TEXT(".\\o2jb.dll"));
    cout << "Library Loaded" << endl;
    PtrSQLAllocHandle ptrAlloc = (PtrSQLAllocHandle)GetProcAddress(hinstLib, "SQLAllocHandle");
    PtrSQLSetEnvAttr ptrSetEnv = (PtrSQLSetEnvAttr)GetProcAddress(hinstLib, "SQLSetEnvAttr");
    PtrSQLDriverConnect ptrConn = (PtrSQLDriverConnect)GetProcAddress(hinstLib, "SQLDriverConnect");
    PtrSQLExecDirect ptrExecDirect = (PtrSQLExecDirect)GetProcAddress(hinstLib, "SQLExecDirect");
    PtrSQLFreeStmt ptrFreeStmt = (PtrSQLFreeStmt)GetProcAddress(hinstLib, "SQLFreeStmt");
    PtrSQLRowCount ptrRowCount = (PtrSQLRowCount)GetProcAddress(hinstLib, "SQLRowCount");

    HWND win = GetDesktopWindow();

    cout << "1" << endl;
    ptrAlloc(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &lpEnv);
    cout << "2" << endl;
    ptrSetEnv(lpEnv,
              SQL_ATTR_ODBC_VERSION,
              (SQLPOINTER)SQL_OV_ODBC2,
              0);
    cout << "3" << endl;
    ptrAlloc(SQL_HANDLE_DBC, lpEnv, &lpDbc);
    cout << "lpDbc:  " << lpDbc << endl;
    cout << "4" << endl;
    ptrConn(lpDbc,
            win,
            (SQLCHAR*) pszConnStr,
            SQL_NTS,
            NULL,
            0,
            NULL,
            SQL_DRIVER_COMPLETE);
    cout << "5" << endl;
    ptrAlloc(SQL_HANDLE_STMT, lpDbc, &lpStmt);
    cout << "6" << endl;
    char const* input = "select * from data";
    cout << "lpStmt:  " << lpStmt << endl;
    ptrExecDirect(lpStmt, (SQLCHAR*) input, SQL_NTS);
    SQLLEN    siRowCount = 0;
    cout << "7" << endl;
    cout << "p: " << ptrExecDirect << " " << ptrRowCount << " r:  " << siRowCount << endl;
    cout << "lpStmt:  " << lpStmt << endl;
    ptrRowCount(lpStmt, &siRowCount);

    cout << "Affected rows:  " << siRowCount << endl;
    ptrFreeStmt(lpStmt, SQL_CLOSE);
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