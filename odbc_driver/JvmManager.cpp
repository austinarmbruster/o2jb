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
#include <cstdarg>

#include "JvmManager.h"

#include "java_error.h"
#include "o2jb_common.h"
#include "o2jb_logging.h"
#include "o2jb_logstream.h"
#include "properties.h"
#include "RegistryKey.h"

#include <windows.h>
#include <sql.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <vector>
#include <iterator>

#include <iostream>
using std::endl; using std::cout;

using std::unique_ptr;
using std::for_each;
using std::getline;
using std::ifstream;
using std::ostream;
using std::set;
using std::string;
using std::stringstream;
using std::vector;

using std::find_if;
using std::not1;
using std::ptr_fun;

using o2jb::properties;
using o2jb::RegistryKey;
using o2jb::filesystem::exist;

typedef jint (JNICALL * PtrCreateJavaVM)(JavaVM **, void **, void *);
typedef set<string> jre_ctr_t;


namespace o2jb {

namespace {
static inline string &ltrim(string &s) {
  s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
  return s;
}

static inline string &rtrim(string &s) {
  s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
  return s;
}

static inline string &trim(string &s) {
  return ltrim(rtrim(s));
}

const string CLASS_TAG("class.");
const string CONSTRUCTOR_TAG("init.");
const string METHOD_TAG("method.");
const string STATIC_METHOD_TAG("staticMethod.");
const size_t CLASS_TAG_SIZE = CLASS_TAG.size();
const size_t CONSTRUCTOR_TAG_SIZE = CONSTRUCTOR_TAG.size();
const size_t METHOD_TAG_SIZE = METHOD_TAG.size();
const size_t STATIC_METHOD_TAG_SIZE = STATIC_METHOD_TAG.size();

LoggerPtr logger = Logger::getLogger("jvm");

unique_ptr<char[]> toSafeCharPtr(string const& str) {
  unique_ptr<char[]> rtnValue(new char[str.size() + 1]);
  strcpy(rtnValue.get(), str.c_str());
  rtnValue[str.size()] = '\0';
  return rtnValue;
}

JavaVMOption set_option_string(unique_ptr<char[]> const& str) {
  JavaVMOption rtnVal;
  rtnVal.optionString = str.get();
  return rtnVal;
}

properties load_app_properties() {
  LoggerPtr logger = Logger::getLogger("config");
  properties props;
  RegistryKey o2jb(HKEY_CURRENT_USER, RegistryKey::SOFTWARE_BASE + "\\AnaVation, LLC.\\Open ODBC JDBC Bridge", RegistryKey::Mode::READ);
  ifstream currentIn;
  string o2jbLoc = o2jb.value("InstallLocation");
  LOG_DEBUG(logger, "Discovered o2jbLoc:  [" << o2jbLoc << "]");
  string appPropFile = "current.properties";
  if (o2jbLoc.empty() && exist(appPropFile)) {
    LOG_INFO(logger, "Could not find installation location, using local path");
    currentIn.open(appPropFile);
  } else {
    if ('\\' != o2jbLoc[o2jbLoc.length()-1]) {
      o2jbLoc += "\\";
    }
    o2jbLoc += appPropFile;
     if (exist(o2jbLoc)) {
      LOG_DEBUG(logger, "Using the registry based location:  " << o2jbLoc);
      currentIn.open(o2jbLoc, ifstream::in);
     } else {
       LOG_WARN(logger, "Could not find a configuration file:  " << o2jbLoc);
     }
  }
   if (currentIn.is_open()) {
     currentIn >> props;
     currentIn.close();
   } else {
     LOG_WARN(logger, "Could not load the configuration, so we will not know the valid JVMs:  " << currentIn.rdstate());
   }
  //props["base.cp"]="<install_path>\\commons-dbcp-1.4.jar;<install_path>\\commons-pool-1.5.4.jar";
  //props["jre.versions"]="1.7,1.8";
  return props;
}

} // end of anonymous namespace


unique_ptr<jvalue[]> make_args(char const * fmt, ...) {
  int numArgs = strlen(fmt);
  va_list args;
  va_start(args, fmt);
  unique_ptr<jvalue[]> argsArr(new jvalue[numArgs]);
  for (int i = 0; i < numArgs; ++i) {
    // TODO account for the other types of args.
    if ('I' == fmt[i]) {
      argsArr[i].i = va_arg(args, jint);
    } else {
      argsArr[i].l = va_arg(args, jobject);
    }
  }
  va_end(args);
  return argsArr;
}

vector<string> JvmManager::NO_OPTIONS;

JvmManager::JvmManager(string const& classPath, vector<string> const& additionalJvmOptions, string const& jvmPath) :
  _jvm(NULL), _env(NULL), _exceptionCleared(false), _throws(false) {
  LoggerPtr logger = Logger::getLogger("config");
  vector<unique_ptr<char[]> >::size_type numOptions = 1 + additionalJvmOptions.size();

  vector<unique_ptr<char[]> > optionsStringsHolder;
  vector<JavaVMOption> options;

  optionsStringsHolder.reserve(numOptions);
  options.reserve(numOptions);

  string cpOption("-Djava.class.path=");
  cpOption += classPath;
  LOG_INFO(logger, "JVM Classpath " << cpOption << "  [" << jvmPath << "]");

  optionsStringsHolder.push_back(toSafeCharPtr(cpOption));
  transform(additionalJvmOptions.begin(), additionalJvmOptions.end(), back_inserter(optionsStringsHolder), toSafeCharPtr);
  transform(optionsStringsHolder.begin(), optionsStringsHolder.end(), back_inserter(options), set_option_string);

  JavaVMInitArgs vm_args;
  vm_args.version = JNI_VERSION_1_6;
  vm_args.nOptions = numOptions;
  vm_args.options = &options[0];
  vm_args.ignoreUnrecognized = false;

cout << "jvmPath:  [" << jvmPath << "]" << endl;
  if (!exist(jvmPath)) {
cout << "jvmPath:  doesn't exist" << endl;
    LOG_ERROR(logger, "The jvmPath does not exist:  " << jvmPath);
    throw java_error("Could not find the JVM.");
  }

  LOG_INFO(logger, "About to load library at " << jvmPath);
cout << "loading lib" << endl;
  HINSTANCE hinstLib = LoadLibrary(TEXT(jvmPath.c_str()));
  LOG_DEBUG(logger, "Library Loaded");
cout << "lib loaded" << endl;

  PtrCreateJavaVM ptrCreateJavaVM = (PtrCreateJavaVM)GetProcAddress(hinstLib, "JNI_CreateJavaVM");
  cout << "have handle:  " << ptrCreateJavaVM << endl;
  jint res = ptrCreateJavaVM(&_jvm, (void**)&_env, &vm_args);
  cout << "called jvm create" << endl;
  if (res < 0) {
    stringstream msg;
    msg << "Failed to create the JVM " << res;
    LOG_ERROR(logger, msg.str());
    throw java_error(msg.str());
  }
  LOG_DEBUG(logger, "JVM Created");
  cout << "jvm create" << endl;
}

JNIEnv* JvmManager::env() const {
  return _env;
}

JvmManager::~JvmManager() {
  if (NULL != _jvm) {
    _jvm->DestroyJavaVM();
  }
}

jre_ctr_t parseJres(string const& csv) {
  LoggerPtr logger = Logger::getLogger("config");
  jre_ctr_t rtnValue;

  stringstream lineStream(csv);
  string val;
  while (getline(lineStream, val, ',')) {
    LOG_DEBUG(logger, "jre val:  [" << val << "]");
    rtnValue.insert(val);
  }
  LOG_DEBUG(logger, "no more jre vals");
  return rtnValue;
}

jobject JvmManager::NewObjectA(std::string const& classTag, std::string const& methodTag, jvalue const * args) {
  mc_ctr_t::iterator iter = _managedClasses.find(classTag);

  if (_managedClasses.end() == iter) {
    throw java_error(("Attempting to call a constructor on an unknown class:  " + classTag));
  }

  if (!iter->second.contains_constructor(methodTag)) {
    throw java_error("Attempting to call a unknown constrcutor:  " + methodTag);
  }

  jobject rtnValue = _env->NewObjectA(iter->second.theClass(), iter->second.get_constructor(methodTag), args);
  exception_check(classTag, methodTag);

  return rtnValue;
}

jobject JvmManager::CallStaticObjectMethodA(std::string const& classTag, std::string const& methodTag, jvalue const * args) {
  mc_ctr_t::iterator iter = _managedClasses.find(classTag);

  if (_managedClasses.end() == iter) {
    throw java_error("Attempting to call a static method on an unknown class:  " + classTag);
  }

  if (!iter->second.contains_static(methodTag)) {
    throw java_error("Attempting to call a unknown static method:  " + methodTag);
  }

  jobject rtnValue = _env->CallStaticObjectMethodA(iter->second.theClass(), iter->second.get_static(methodTag), args);
  exception_check(classTag, methodTag);

  return rtnValue;
}

jobject JvmManager::CallObjectMethodA(jobject obj, std::string const& classTag, std::string const& methodTag, jvalue const * args) {
  mc_ctr_t::iterator iter = _managedClasses.find(classTag);

  if (_managedClasses.end() == iter) {
    throw java_error("Attempting to call a method on an unknown class:  " + classTag);
  }

  if (!iter->second.contains(methodTag)) {
    throw java_error("Attempting to call a unknown method:  " + methodTag);
  }
  jobject rtnValue = _env->CallObjectMethodA(obj, iter->second[methodTag], args);
  exception_check(classTag, methodTag);

  return rtnValue;
}

jobject JvmManager::CallObjectMethod(jobject obj, std::string const& classTag, std::string const& methodTag) {
  mc_ctr_t::iterator iter = _managedClasses.find(classTag);

  if (_managedClasses.end() == iter) {
    cout << "unknown class" << endl;
    throw java_error("Attempting to call a method on an unknown class:  " + classTag);
  }

  if (!iter->second.contains(methodTag)) {
    cout << "unknown method" << endl;
    throw java_error("Attempting to call a unknown method:  " + methodTag);
  }
  cout << "about to call " << classTag << "." << methodTag << endl;
  cout << "env " << _env << endl;
  cout << "obj:  " << obj << endl;
  cout << "method:  " <<  iter->second[methodTag] << endl;
  if ("conn" == classTag && "createStatement" == methodTag) {
    cout << "testing env with new string" << endl;
    _env->NewStringUTF("Hello World");
    cout << "got a new string" << endl;

    jmethodID closed = iter->second["isClosed"];
    cout << "hi" << endl;
    exception_check(classTag, methodTag);
    cout << "checked" << endl;
    cout << "conn is closed:  " << _env->CallBooleanMethod(obj, closed) << endl;
  }
  cout << "calling" <<endl;
  jobject rtnValue = _env->CallObjectMethod(obj, iter->second[methodTag]);
  cout << "finished call" << endl;
  exception_check(classTag, methodTag);
  return rtnValue;
}

jint JvmManager::CallIntMethodA(jobject obj, std::string const& classTag, std::string const& methodTag, jvalue const * args) {
  mc_ctr_t::iterator iter = _managedClasses.find(classTag);

  if (_managedClasses.end() == iter) {
    throw java_error(("Attempting to call a method on an unknown class:  " + classTag).c_str());
  }

  if (!iter->second.contains(methodTag)) {
    throw java_error("Attempting to call a unknown method:  " + methodTag);
  }

  jint rtnValue = 0;
  if (NULL == args) {
    rtnValue = _env->CallIntMethod(obj, iter->second[methodTag]);
  } else {
    rtnValue = _env->CallIntMethodA(obj, iter->second[methodTag], args);
  }
  exception_check(classTag, methodTag);

  return rtnValue;
}

jboolean JvmManager::CallBooleanMethodA(jobject obj, std::string const& classTag, std::string const& methodTag, jvalue const * args) {
  mc_ctr_t::iterator iter = _managedClasses.find(classTag);

  if (_managedClasses.end() == iter) {
    throw java_error(("Attempting to call a method on an unknown class:  " + classTag).c_str());
  }

  if (!iter->second.contains(methodTag)) {
    throw java_error("Attempting to call a unknown method:  " + methodTag);
  }

  jboolean rtnValue = 0;
  if (NULL == args) {
    rtnValue = _env->CallBooleanMethod(obj, iter->second[methodTag]);
  } else {
    rtnValue = _env->CallBooleanMethodA(obj, iter->second[methodTag], args);
  }
  exception_check(classTag, methodTag);

  return rtnValue;
}

void JvmManager::CallVoidMethodA(jobject obj, std::string const& classTag, std::string const& methodTag, jvalue const * args) {
  mc_ctr_t::iterator iter = _managedClasses.find(classTag);

  if (_managedClasses.end() == iter) {
    throw java_error("Attempting to call a method on an unknown class:  " + classTag);
  }

  if (!iter->second.contains(methodTag)) {
    throw java_error("Attempting to call a unknown method:  " + methodTag);
  }

  _env->CallVoidMethodA(obj, iter->second[methodTag], args);
  exception_check(classTag, methodTag);
}

string JvmManager::javaHome() {
  char* pPath = getenv("JAVA_HOME");
  if (NULL == pPath) {
    properties props = load_app_properties();

    string allowedJres = props["jre.versions"];
    LOG_DEBUG(logger, "allowed jres:  " << allowedJres);
    jre_ctr_t validJres = parseJres(allowedJres);

    try {
      RegistryKey jre(HKEY_LOCAL_MACHINE, RegistryKey::SOFTWARE_BASE + "\\JavaSoft\\Java Runtime Environment", RegistryKey::Mode::READ);
      string currentVersion = jre.value("CurrentVersion");
      jre_ctr_t::iterator iter = validJres.find(currentVersion);
      if (validJres.end() != iter) {
        RegistryKey jreVerKey = jre.merge(currentVersion);
        return jreVerKey.value("JavaHome");
      }
      return "";
    } catch (registry_exception& e) {
      LoggerPtr logger = Logger::getLogger("config");
      LOG_ERROR(logger, "failed to extract the JVM from registry. " << e.what());
      return "";
    }
  } else {
    return string(pPath);
  }
}

string JvmManager::registryPath() {
  LoggerPtr logger = Logger::getLogger("config");
  properties props = load_app_properties();

  string allowedJres = props["jre.versions"];
  LOG_DEBUG(logger, "allowed jres:  " << allowedJres);
  jre_ctr_t validJres = parseJres(allowedJres);
  try {
    cout << "registry path:  [" << RegistryKey::SOFTWARE_BASE + "\\JavaSoft\\Java Runtime Environment" << "]" <<endl;
    RegistryKey jre(HKEY_LOCAL_MACHINE, RegistryKey::SOFTWARE_BASE + "\\JavaSoft\\Java Runtime Environment", RegistryKey::Mode::READ);

    string currentVersion = jre.value("CurrentVersion");
    currentVersion = trim(currentVersion);
    jre_ctr_t::iterator iter = validJres.find(currentVersion);
    if (validJres.end() != iter) {
      RegistryKey jreVerKey = jre.merge(currentVersion);
      LOG_DEBUG(logger, "merged jre key");
      string javaHome = jreVerKey.value("JavaHome");
      LOG_DEBUG(logger, "java home:  " << javaHome);
      javaHome.replace(3,strlen("Program Files (x86)"), "progra~2");
      string javaLoc(javaHome);
      javaLoc += "\\bin\\java.exe";
      string javaDllLoc(javaHome);
#ifdef __x86_64__
      javaDllLoc += "\\bin\\server\\jvm.dll";
#else
      javaDllLoc += "\\bin\\client\\jvm.dll";
#endif
      return javaDllLoc;
    } else {
      LOG_INFO(logger, "current version is not valid:  [" << currentVersion << "]");
    }
  } catch (registry_exception& e) {
    LoggerPtr logger = Logger::getLogger("config");
    LOG_ERROR(logger, "failed to extract the JVM from registry. " << e.what());
  }
  LOG_WARN(logger, "Failed to find a registry path entry");
  return "";
}

bool JvmManager::loadConfig(string const& propFileName) {
  bool rtnValue = false;

  properties configProps;

  if (exist(propFileName)) {
    ifstream src(propFileName);
    src >> configProps;
    src.close();
  } else {
    // RegistryKey o2jb(HKEY_CURRENT_USER, RegistryKey::SOFTWARE_BASE + "\\AnaVation, LLC.\\Open ODBC JDBC Bridge", RegistryKey::Mode::READ);
    string o2jbLoc = install_path();
    string installLoc = o2jbLoc + "\\" + propFileName;
    if (exist(installLoc)) {
      ifstream src(installLoc);
      src >> configProps;
      src.close();
    }
  }

  for_each(configProps.begin(), configProps.end(), std::bind(&JvmManager::loadClasses, this, std::placeholders::_1));
  for_each(configProps.begin(), configProps.end(), std::bind(&JvmManager::loadMethods, this, std::placeholders::_1));

  return rtnValue;
}

void JvmManager::loadClasses(properties::value_type const& prop) {
  // TODO use boost::starts_with
  if (!prop.first.compare(0, CLASS_TAG_SIZE, CLASS_TAG)) {
    jclass cls = _env->FindClass(prop.second.c_str());
    if (NULL == cls || _env->ExceptionCheck()) {
      _env->ExceptionDescribe();
      _env->ExceptionClear();
      throw java_error("Could not find the " + prop.second + " class");
    }
    string className = prop.first.substr(CLASS_TAG_SIZE);
    _managedClasses.insert(make_pair(className, jclass_metadata(cls)));
  }
}

void JvmManager::processMethod(properties::value_type const& prop, string const& label, string::size_type labelSize,
                               method_type mType) {
  // Find the separator between class tag and method tag
  string::size_type classMethodSep = prop.first.find(".", labelSize);
  if (string::npos == classMethodSep) {
    throw java_error("improperly formatted class.method format:  " + prop.first);
  }

  // Make sure the class tag was already processed
  string classTag = prop.first.substr(labelSize, classMethodSep - labelSize);
  mc_ctr_t::iterator iter = _managedClasses.find(classTag);
  if (_managedClasses.end() == iter) {
    throw java_error("Attempting to load a method for a class that has not been loaded:  " + prop.first);
  }

  // Find the separator between method name and method signature
  string::size_type nameSigSep = prop.second.find("(");
  if (string::npos == nameSigSep) {
    throw java_error("improperly formatted methodName(signature) format:  " + prop.second);
  }

  // extract the remaing data required to obtain the method id.
  string methodName = prop.second.substr(0, nameSigSep);
  string methodSig = prop.second.substr(nameSigSep);
  jmethodID mid = NULL;
  if (STATIC_METHOD == mType) {
    mid = _env->GetStaticMethodID(iter->second.theClass(), methodName.c_str(), methodSig.c_str());
  } else {
    mid = _env->GetMethodID(iter->second.theClass(), methodName.c_str(), methodSig.c_str());
  }

  if (NULL == mid || _env->ExceptionCheck()) {
    _env->ExceptionDescribe();
    _env->ExceptionClear();
    stringstream ss;
    ss << "Could not find this method:  " << classTag << "." << methodName << methodSig;
    throw java_error(ss.str());
  }

  string methodTag = prop.first.substr(classMethodSep + 1);
  switch (mType) {
  case STATIC_METHOD:
    iter->second.add_static(methodTag, mid);
    break;
  case MEMBER_METHOD:
    (iter->second)[methodTag] = mid;
    break;
  case CONSTRUCTOR:
    iter->second.add_constructor(methodTag, mid);
    break;
  }
}

void JvmManager::loadMethods(properties::value_type const & prop) {
  if (!prop.first.compare(0, METHOD_TAG_SIZE, METHOD_TAG)) {
    processMethod(prop, METHOD_TAG, METHOD_TAG_SIZE, MEMBER_METHOD);
  } else if (!prop.first.compare(0, STATIC_METHOD_TAG_SIZE, STATIC_METHOD_TAG)) {
    processMethod(prop, STATIC_METHOD_TAG, STATIC_METHOD_TAG_SIZE, STATIC_METHOD);
  } else if (!prop.first.compare(0, CONSTRUCTOR_TAG_SIZE, CONSTRUCTOR_TAG)) {
    processMethod(prop, CONSTRUCTOR_TAG, CONSTRUCTOR_TAG_SIZE, CONSTRUCTOR);
  }
}

string JvmManager::capture_exception(jthrowable e) {
  stringstream ss;
  capture_exception(ss, e, true);
  return ss.str();
}

void JvmManager::capture_exception(ostream& out, jthrowable e, bool topException) {
  // Get the array of StackTraceElements.
  jobjectArray frames = (jobjectArray) _env->CallObjectMethod(e, _managedClasses["th"]["st"]);
  jsize frames_length = _env->GetArrayLength(frames);

  // Add Throwable.toString() before descending
  // stack trace messages.
  if (0 != frames) {
    jstring msg_obj = (jstring) _env->CallObjectMethod(e, _managedClasses["th"]["toStr"]);
    const char* msg_str = _env->GetStringUTFChars(msg_obj, 0);

    // If this is not the top-of-the-trace then
    // this is a cause.
    if (!topException) {
      out << "\nCaused by: " << msg_str;
    }
    else {
      out << msg_str;
    }

    _env->ReleaseStringUTFChars(msg_obj, msg_str);
    _env->DeleteLocalRef(msg_obj);
  }

  // Append stack trace messages if there are any.
  if (frames_length > 0) {
    jsize i = 0;
    for (i = 0; i < frames_length; i++) {
      // Get the string returned from the 'toString()'
      // method of the next frame and append it to
      // the error message.
      jobject frame = _env->GetObjectArrayElement(frames, i);
      jstring msg_obj = (jstring) _env->CallObjectMethod(frame, _managedClasses["ste"]["toStr"]);

      const char* msg_str = _env->GetStringUTFChars(msg_obj, 0);

      out << "\n    " << msg_str;

      _env->ReleaseStringUTFChars(msg_obj, msg_str);
      _env->DeleteLocalRef(msg_obj);
      _env->DeleteLocalRef(frame);
    }
  }

  // If 'a_exception' has a cause then append the
  // stack trace messages from the cause.
  if (NULL != frames) {
    jthrowable cause = (jthrowable) _env->CallObjectMethod(e, _managedClasses["th"]["cause"]);
    if (NULL != cause) {
      capture_exception(out, cause, false);
    }
  }
}

void JvmManager::exception_check(string const& classTag, string const& methodTag) {
  if (_env->ExceptionCheck()) {
    jthrowable e = _env->ExceptionOccurred();
    _env->ExceptionClear();
    _exceptionCleared = true;

    string msg = capture_exception(e);
    LOG_ERROR(logger, "Failed to call " << classTag << "." << methodTag << ":  " << msg);
    if (_throws) {
      cout << "exception:  " << msg << endl;
      throw java_error(msg);
    }
  }
}

void JvmManager::DeleteLocalRef(jobject obj) {
  _env->DeleteLocalRef(obj);
}

jstring JvmManager::toJString(char const * const cStr) {
  return _env->NewStringUTF(cStr);
}

jstring JvmManager::toJString(char const * const asChar, std::size_t const strLen) {
  jstring jValue = NULL;
  if (NULL != asChar) {
    if (static_cast<std::size_t>(SQL_NTS) == strLen) {
      jValue = _env->NewStringUTF(asChar);
    } else {
      string cppValue(asChar, strLen);
      jValue = _env->NewStringUTF(cppValue.c_str());
    }
  }
  return jValue;
}

bool JvmManager::exception_cleared() {
  bool temp = _exceptionCleared;
  _exceptionCleared = false;

  return temp;
}

void JvmManager::set_throws(bool toThrow) {
  _throws = toThrow;
}

bool JvmManager::is_throws() {
  return _throws;
}
} // end namespace
