#ifndef _INCLUDE_GUARD_O2JB_CONFIG_H_
#define _INCLUDE_GUARD_O2JB_CONFIG_H_ 1
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

O2JB_DLL BOOL ConfigDriver(
  HWND    hwndParent,
  WORD    fRequest,
  LPCSTR  lpszDriver,
  LPCSTR  lpszArgs,
  LPSTR   lpszMsg,
  WORD    cbMsgMax,
  WORD *  pcbMsgOut);

O2JB_DLL BOOL ConfigDSN(
  HWND     hwndParent,
  WORD     fRequest,
  LPCSTR   lpszDriver,
  LPCSTR   lpszAttributes);

O2JB_DLL BOOL ConfigTranslator(
  HWND     hwndParent,
  DWORD *  pvOption);

#endif