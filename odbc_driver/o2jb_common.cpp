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
#include "o2jb_common.h"
#include "RegistryKey.h"

#include "o2jb_logging.h"
#include "o2jb_logstream.h"

#include <dirent.h>

#include <cstdio>

#include <algorithm>
#include <fstream>
#include <sstream>

using std::ifstream;
using std::string;
using std::stringstream;
using std::transform;

namespace o2jb {

namespace {
char const * APP_PATH = "AppData\\Local\\o2jb";

char charCleanse(char c) {
  const char FILLER = '_';
  return isalnum(c) ? c : FILLER;
}

string cleanFileName(string const& userInput) {
  string rtnValue;
  transform(userInput.begin(), userInput.end(), back_inserter(rtnValue), charCleanse);
  return rtnValue;
}

}

SQLCHAR const * const DEFAULT_STATE = reinterpret_cast<SQLCHAR const *>("00000");

std::string dsnPropFile(std::string const& dsn) {
  stringstream filePath;
  filePath << getenv("USERPROFILE") << "\\" << APP_PATH << "\\" << cleanFileName(dsn) << ".properties";
  return filePath.str();
}

std::string install_path() {
  RegistryKey o2jb(HKEY_CURRENT_USER, RegistryKey::SOFTWARE_BASE + "\\AnaVation, LLC.\\Open ODBC JDBC Bridge", RegistryKey::Mode::READ);
  return o2jb.value("InstallLocation");
}

bool change_to_install_dir() {
  string o2jbLoc = install_path();

  int code = -1;
  if (!o2jbLoc.empty()) {
    code = chdir(o2jbLoc.c_str());
  }

  return 0 == code;
}


std::string replace_all(std::string const& needle, std::string const& replacement, std::string const& haystack) {
  string rtnValue(haystack);
  string::size_type needleSz = needle.size();
  string::size_type replacementSz = replacement.size();
  string::size_type start = 0;
  while ((start = rtnValue.find(needle, start)) != string::npos) {
    rtnValue.replace(start, needleSz, replacement);
    start += replacementSz;
  }

  return rtnValue;
}

namespace filesystem {
bool exist(std::string const& name) {
  LoggerPtr logger = Logger::getLogger("config");
  // TODO consider using boost::filesystem::exist
  ifstream file(name);
  bool rtnValue = file;

  if (rtnValue) {
    LOG_INFO(logger, "Found file:  " << name);
    file.close();
  } else {
    LOG_INFO(logger, "File not found:  [" << name << "]  " << errno << " " << strerror(errno));
    DIR *dir;
    if ((dir = opendir(name.c_str())) != NULL) {
      LOG_INFO(logger, "Found dir:  " << name);
      rtnValue = true;
      closedir(dir);
    }
  }

  return rtnValue;
}
}
}

