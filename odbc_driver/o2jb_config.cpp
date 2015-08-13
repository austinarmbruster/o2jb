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
#include "o2jb_config.h"

#include "properties.h"
#include "o2jb_common.h"
#include "o2jb_logging.h"
#include "o2jb_logstream.h"
#include "JvmManager.h"
#include "java_error.h"
#include "json_util.h"
#include "RegistryKey.h"
#include "o2jb_install.h"

#include <direct.h>

#include <cstdio>
#include <iterator>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

using o2jb::LoggerPtr;
using o2jb::Logger;
using o2jb::JvmManager;
using o2jb::RegistryKey;
using o2jb::change_to_install_dir;
using o2jb::filesystem::exist;
using o2jb::properties;
using o2jb::dsnPropFile;
using o2jb::json::toJson;

using std::back_inserter;
using std::boolalpha;
using std::ifstream;
using std::remove;
using std::make_pair;
using std::map;
using std::ofstream;
using std::string;
using std::stringstream;

#define  ODBC_ADD_DSN     1
#define  ODBC_CONFIG_DSN  2
#define  ODBC_REMOVE_DSN  3

namespace {
typedef map<string, string> attr_ctr_t;

char const * APP_PATH = "AppData\\Local\\o2jb";

attr_ctr_t toMap(char const* attributes) {
  LoggerPtr logger = Logger::getLogger("config");
  map<string, string> rtnValue;

  if (NULL != attributes) {
    int attrLen = strlen(attributes);
    while (attrLen > 0) {
      char const* loc = strstr(attributes, "=");
      if (NULL != loc) {
        int keyLen = loc - attributes;
        string key(attributes, 0, keyLen);
        string value(loc + 1, attrLen - keyLen - 1);
        LOG_DEBUG(logger, "adding mapping:  " << key << "=>" << value);
        rtnValue.insert(make_pair(key, value));
      }
      attributes += attrLen + 1;
      attrLen = strlen(attributes);
    }
  }
  return rtnValue;
}

char charCleanse(char c) {
  const char FILLER = '_';
  return isalnum(c) ? c : FILLER;
}

string cleanFileName(string const& userInput) {
  string rtnValue;
  transform(userInput.begin(), userInput.end(), back_inserter(rtnValue), charCleanse);
  return rtnValue;
}

bool showDriverConfig(string const& inputConfig, string& outputConfig) {
  LoggerPtr logger = Logger::getLogger("config");
  LOG_INFO(logger, "input config:  " << inputConfig);
  try {
    string home = JvmManager::javaHome();
    if (exist(home)) {
      string jfxPath = home + "\\lib\\jfxrt.jar";
      if (!exist(jfxPath)) {
        jfxPath = home + "\\jre\\lib\\jfxrt.jar";
        if (!exist(jfxPath)) {
          LOG_ERROR(logger, "Failed to find the jfxrt.jar file.");
          return false;
        }
      }

      string classPath = "o2jb-gui-0.0.1-SNAPSHOT.jar;" + jfxPath;
      LOG_INFO(logger, "Config classpath:  " << classPath);
      JvmManager jvm(classPath, JvmManager::NO_OPTIONS, JvmManager::registryPath());
      jvm.loadConfig("configGui.properties");

      jclass stringCls = jvm.env()->FindClass("java/lang/String");
      jobjectArray args = jvm.env()->NewObjectArray(1, stringCls, NULL);
      jvm.env()->SetObjectArrayElement(args, 0, jvm.env()->NewStringUTF(inputConfig.c_str()));
      jvalue zargs[1];
      zargs[0].l = args;//jvm.env()->NewStringUTF("{\"accept\":\"true\"}");
      jstring response = (jstring)jvm.CallStaticObjectMethodA("guiApp", "show", zargs);
      if (jvm.env()->ExceptionCheck()) {
        jvm.env()->ExceptionDescribe();
        jvm.env()->ExceptionClear();
        return false;
      }
      char const* tmpReport = jvm.env()->GetStringUTFChars(response, 0);
      outputConfig = tmpReport;
      jvm.env()->ReleaseStringUTFChars(response, tmpReport);
      return true;
    } else {
      LOG_ERROR(logger, "Failed to find Java Home");
      return false;
    }
  } catch (o2jb::java_error &e) {
    LOG_ERROR(logger, "caught java_error exception:  " << e.what());
    return false;
  } catch (std::runtime_error &e) {
    LOG_ERROR(logger, "caught runtime_error exception:  " << e.what());
    return false;
  }
}


bool removeDriver(HWND hwndParent, LPCSTR lpszDriver, attr_ctr_t const& attrs) {
  LoggerPtr logger = Logger::getLogger("config");

  BOOL goodRemoval = FALSE;
  attr_ctr_t::const_iterator iter = attrs.find("DSN");
  if (attrs.end() != iter) {
    LOG_INFO(logger, "Removing " << iter->second);
    goodRemoval = SQLRemoveDSNFromIni(iter->second.c_str());

    if (TRUE == goodRemoval) {

      stringstream filePath;
      filePath << getenv("USERPROFILE") << "\\" << APP_PATH << "\\" << cleanFileName(iter->second) << ".properties";
      int result = remove(filePath.str().c_str());

      if (0 != result) {
        if (NULL != hwndParent) {
          MessageBox(hwndParent, "Could not remove the configuration file.  JC0004",
                     "Incomplete Removal", MB_OK | MB_ICONINFORMATION);
        }
        LOG_ERROR(logger, "Failed to remove the configuration file for the dsn " << iter->second << ".  Expected to remove "
                  << filePath.str());
      }
      LOG_INFO(logger, "Status of registry removal:  " << boolalpha << (TRUE == goodRemoval) << " and file removal:  " << result);
    }
  } else {
    if (NULL != hwndParent) {
      MessageBox(hwndParent, "Missing the DSN Name.  Please contact the application owner.  JC0005",
                 "Missing DSN, Cannot Remove", MB_OK | MB_ICONINFORMATION);
    }
    LOG_ERROR(logger, "Failed to find the DSN in the attributes.");
  }
  return TRUE == goodRemoval;
}

bool addDriver(HWND hwndParent, LPCSTR lpszDriver, string const& oldDsn = "", string const& existingConfig = "") {
  LoggerPtr logger = Logger::getLogger("config");
  LOG_DEBUG(logger, "received a call to add a DSN");

  string result;
  bool goodConfig = showDriverConfig(existingConfig, result);
  if (!goodConfig) {
    return goodConfig;
  }

  map<string, string> userConfig = o2jb::json::toMap(result);
  string action = userConfig["action"];
  string dsn = userConfig["dsn"];
  string cp = userConfig["cp"];
  string driver = userConfig["driver"];
  string url = userConfig["url"];
  string user = userConfig["user"];
  string pwd = userConfig["password"];
  BOOL goodWrite = FALSE;
  if ("accepted" == action && !dsn.empty() && !url.empty()) {
    if (!oldDsn.empty() && oldDsn != dsn) {
      attr_ctr_t attrs;
      attrs.insert(make_pair("DSN", oldDsn));
      removeDriver(hwndParent, lpszDriver, attrs);
    }

    char* userDir = getenv("USERPROFILE");
    if (exist(userDir)) {
      stringstream filePath;
      filePath << userDir << "\\" << APP_PATH << "\\" << cleanFileName(dsn) << ".properties";
      ofstream dsnConfigFile(filePath.str());
      if (dsnConfigFile.good()) {
        goodWrite = dsn.empty() ? FALSE : SQLWriteDSNToIni(dsn.c_str(), lpszDriver);

        if (TRUE == goodWrite) {
          properties dsnProps;
          dsnProps.insert(userConfig.begin(), userConfig.end());
          dsnConfigFile << dsnProps;
        }
        dsnConfigFile.close();
      } else {
        if (NULL != hwndParent) {
          MessageBox(hwndParent, "Could not create the configuration file.  JC0003", "Failed Configuration Writing",
                     MB_OK | MB_ICONEXCLAMATION);
        }
        LOG_ERROR(logger, "Could not write to " << filePath.str());
      }
    } else {
      if (NULL != hwndParent) {
        MessageBox(hwndParent, "Configuration failed.  The configuration directory could not be found.  "
                   "Please contact the application owner. JC0002",
                   "Failed Configuration", MB_OK | MB_ICONEXCLAMATION);
      }
      LOG_ERROR(logger, "Could not find user propfile [" << userDir << "]");
    }
  } else if ("accepted" == action) {
    if (NULL != hwndParent) {
      MessageBox(hwndParent, "Configuration failed.  Please contact the application owner. JC0001",
                 "Failed Configuration", MB_OK | MB_ICONEXCLAMATION);
    }
    LOG_ERROR(logger, "Either the url=[" << url << "] or dsn=[" << dsn
              << "] is blank after receiving an accept from the Configuration GUI.");
  }
  return TRUE == goodWrite;
}

bool configDriverInternal(HWND hwndParent, LPCSTR lpszDriver, attr_ctr_t const& attrs) {
  LoggerPtr logger = Logger::getLogger("config");

  BOOL goodConfig = FALSE;
  attr_ctr_t::const_iterator iter = attrs.find("DSN");
  if (attrs.end() != iter) {
    LOG_INFO(logger, "Configuring " << iter->second);

    properties currentProps;
    ifstream dsnProps(dsnPropFile(iter->second));
    dsnProps >> currentProps;
    dsnProps.close();

    string dsnJson = toJson(currentProps.begin(), currentProps.end());

    bool goodAdd = addDriver(hwndParent, lpszDriver, iter->second, dsnJson);
    goodConfig = goodAdd ? TRUE : FALSE;
    LOG_INFO(logger, "Status of configure:  " << boolalpha << (TRUE == goodConfig));
  } else {
    LOG_ERROR(logger, "Failed to find the DSN in the attributes. "
              "No configuration possible");
  }
  return TRUE == goodConfig;
}

} // end of anonymous namespace

BOOL ConfigDriver(
  HWND    hwndParent,
  WORD    fRequest,
  LPCSTR  lpszDriver,
  LPCSTR  lpszArgs,
  LPSTR   lpszMsg,
  WORD    cbMsgMax,
  WORD *  pcbMsgOut) {
  LoggerPtr logger = Logger::getLogger("config");
  LOG_DEBUG(logger, "received a call to ConfigDriver");

  return TRUE;
}

BOOL ConfigDSN(
  HWND     hwndParent,
  WORD     fRequest,
  LPCSTR   lpszDriver,
  LPCSTR   lpszAttributes) {
  BOOL rtnValue = FALSE;

  change_to_install_dir();

  LoggerPtr logger = Logger::getLogger("config");
  LOG_DEBUG(logger, "received a call to ConfigDSN");

  switch (fRequest) {
  case ODBC_ADD_DSN:
    LOG_DEBUG(logger, "need to add a dsn, " << lpszDriver << ", " << lpszAttributes);
    rtnValue = addDriver(hwndParent, lpszDriver) ? TRUE : FALSE;
    break;
  case ODBC_CONFIG_DSN:
    LOG_DEBUG(logger, "need to config a dsn");
    rtnValue = configDriverInternal(hwndParent, lpszDriver, toMap(lpszAttributes)) ? TRUE : FALSE;
    break;
  case ODBC_REMOVE_DSN:
    LOG_DEBUG(logger, "need to remove a dsn");
    rtnValue = removeDriver(hwndParent, lpszDriver, toMap(lpszAttributes)) ? TRUE : FALSE;
    break;
  default:
    LOG_WARN(logger, "Uknown operation requested for configuring a DSN:  " << fRequest);
    break;
  }
  return rtnValue;
}

// BOOL ConfigTranslator(
//   HWND     hwndParent,
//   DWORD *  pvOption) {
//   LoggerPtr logger = Logger::getLogger("config");
//   LOG_DEBUG(logger, "received a call to ConfigTranslator");

//   return TRUE;
// }