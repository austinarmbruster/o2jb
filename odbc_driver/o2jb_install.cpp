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
#include "o2jb_install.h"

#include "o2jb_common.h"
#include "o2jb_logging.h"
#include "o2jb_logstream.h"

#include "RegistryKey.h"

#include <string>

using o2jb::LoggerPtr;
using o2jb::Logger;

using o2jb::RegistryKey;
using o2jb::registry_exception;
using o2jb::install_path;

using std::string;

BOOL SQLInstallDriverEx(
     LPCSTR    lpszDriver,
     LPCSTR    lpszPathIn,
     LPSTR     lpszPathOut,
     WORD      cbPathOutMax,
     WORD *    pcbPathOut,
     WORD      fRequest,
     LPDWORD   lpdwUsageCount) {
  LoggerPtr logger = Logger::getLogger("install");
  LOG_DEBUG(logger, "received a call to SQLInstallDriverEx");
  return FALSE;
}

BOOL SQLWriteDSNToIni(
     LPCSTR   lpszDSN,
     LPCSTR   lpszDriver) {
  LoggerPtr logger = Logger::getLogger("install");
  LOG_DEBUG(logger, "received a call to SQLWriteDSNtoIni");

  BOOL rtnValue = FALSE;

  try {
    RegistryKey odbcIni(HKEY_CURRENT_USER, "Software\\ODBC\\ODBC.INI");

    string dsn(lpszDSN);
    RegistryKey driver = odbcIni.merge(dsn);

    // Driver / REG_SZ / location of driver
    const string driverLoc = install_path() + "o2jb.dll";
    size_t len = driverLoc.length();
    driver.set_value("Driver", REG_SZ, (BYTE*)driverLoc.c_str(), len);

    // SafeTransactions / REG_DWORD / 0
    int zero = 0;
    driver.set_value("SafeTransactions", REG_DWORD, (BYTE*)&zero, sizeof(zero));

    // Add the data source
    RegistryKey dataSrcs = odbcIni.merge("ODBC Data Sources");
    len = strlen(lpszDriver);
    dataSrcs.set_value(dsn, REG_SZ, (BYTE*)lpszDriver, len);

    rtnValue = TRUE;
  } catch (registry_exception& e) {
    LOG_FATAL(logger, string("Failed to create the keys:  ") + e.what());
  }
  return rtnValue;
}

BOOL SQLRemoveDSNFromIni(
     LPCSTR   lpszDSN) {
  LoggerPtr logger = Logger::getLogger("install");
  LOG_DEBUG(logger, "received a call to SQLRemoveDSNFromIni");

  BOOL rtnValue = FALSE;

  try {
    RegistryKey odbcIni(HKEY_CURRENT_USER, "Software\\ODBC\\ODBC.INI");

    // remove the data source tag
    RegistryKey dataSrcs = odbcIni.merge("ODBC Data Sources");
    dataSrcs.unset_value(lpszDSN);

    // remove the DSN tree
    odbcIni.remove(lpszDSN);

    rtnValue = TRUE;
  } catch (registry_exception& e) {
    LOG_FATAL(logger, string("Failed to create the keys:  ") + e.what());
  }
  return rtnValue;
}  

