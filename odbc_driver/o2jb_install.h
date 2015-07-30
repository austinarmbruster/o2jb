#ifndef _INCLUDE_GUARD_O2JB_INSTALL_H_
#define _INCLUDE_GUARD_O2JB_INSTALL_H_ 1
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

#ifdef BUILDING_O2JB_DLL
#ifdef __cplusplus
#define O2JB_DLL extern "C" __declspec(dllexport)
#else
#define O2JB_DLL __declspec(dllexport)
#endif
#else
#define O2JB_DLL __declspec(dllimport)
#endif

O2JB_DLL BOOL SQLConfigDataSource(
     HWND     hwndParent,
     WORD     fRequest,
     LPCSTR   lpszDriver,
     LPCSTR   lpszAttributes);

O2JB_DLL BOOL SQLConfigDriver(
     HWND     hwndParent,
     WORD     fRequest,
     LPCSTR   lpszDriver,
     LPCSTR   lpszArgs,
     LPSTR    lpszMsg,
     WORD     cbMsgMax,
     WORD *   pcbMsgOut);

O2JB_DLL BOOL SQLCreateDataSource(
     HWND    hwnd,
     LPSTR   lpszDS);

O2JB_DLL BOOL SQLGetConfigMode(
     UWORD *   pwConfigMode);

O2JB_DLL BOOL SQLGetInstalledDrivers(
     LPSTR   lpszBuf,
     WORD    cbBufMax,
     WORD *  pcbBufOut);

O2JB_DLL int SQLGetPrivateProfileString(
     LPCSTR   lpszSection,
     LPCSTR   lpszEntry,
     LPCSTR   lpszDefault,
     LPCSTR   RetBuffer,
     INT      cbRetBuffer,
     LPCSTR   lpszFilename);

O2JB_DLL BOOL SQLGetTranslator(
     HWND      hwndParent,
     LPSTR     lpszName,
     WORD      cbNameMax,
     WORD *    pcbNameOut,
     LPSTR     lpszPath,
     WORD      cbPathMax,
     WORD *    pcbPathOut,
     DWORD *   pvOption);

O2JB_DLL BOOL SQLInstallDriverEx(
     LPCSTR    lpszDriver,
     LPCSTR    lpszPathIn,
     LPSTR     lpszPathOut,
     WORD      cbPathOutMax,
     WORD *    pcbPathOut,
     WORD      fRequest,
     LPDWORD   lpdwUsageCount);

O2JB_DLL BOOL SQLInstallDriverManager(
     LPSTR    lpszPath,
     WORD     cbPathMax,
     WORD *   pcbPathOut);

O2JB_DLL RETCODE SQLInstallerError(
     WORD      iError,
     DWORD *   pfErrorCode,
     LPSTR     lpszErrorMsg,
     WORD      cbErrorMsgMax,
     WORD *    pcbErrorMsg);

O2JB_DLL BOOL SQLInstallTranslatorEx(
     LPCSTR    lpszTranslator,
     LPCSTR    lpszPathIn,
     LPSTR     lpszPathOut,
     WORD      cbPathOutMax,
     WORD *    pcbPathOut,
     WORD      fRequest,
     LPDWORD   lpdwUsageCount);

O2JB_DLL BOOL SQLManageDataSources(
     HWND     hwnd);


O2JB_DLL BOOL SQLInstallDriverEx(
     LPCSTR    lpszDriver,
     LPCSTR    lpszPathIn,
     LPSTR     lpszPathOut,
     WORD      cbPathOutMax,
     WORD *    pcbPathOut,
     WORD      fRequest,
     LPDWORD   lpdwUsageCount);

// skipped some
O2JB_DLL BOOL SQLWriteDSNToIni(
     LPCSTR   lpszDSN,
     LPCSTR   lpszDriver);

O2JB_DLL BOOL SQLRemoveDSNFromIni(
     LPCSTR   lpszDSN);

#endif