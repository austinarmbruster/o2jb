#ifndef _INCLUDE_GUARD_JVM_MANAGER_H_
#define _INCLUDE_GUARD_JVM_MANAGER_H_ 1
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

#include "jclass_metadata.h"
#include "properties.h"

#include <jni.h>

#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace o2jb {

std::unique_ptr<jvalue[]> make_args(char const * fmt, ...);

class JvmManager {
public:
  static std::vector<std::string> NO_OPTIONS;
  static std::string registryPath();
  static std::string javaHome();
  JvmManager(std::string const& classPath,
             std::vector<std::string> const& additionalJvmOptions = std::vector<std::string>(),
             std::string const& jvmPath = "C:\\Program Files\\Java\\jre7\\bin\\server\\jvm.dll");
  virtual ~JvmManager();
  JNIEnv* env() const;

  bool loadConfig(std::string const& propFileName);
  jobject CallStaticObjectMethodA(std::string const& classTag, std::string const& methodTag, jvalue const * args = NULL);

  jobject NewObjectA(std::string const& classTag, std::string const& methodTag, jvalue const * args = NULL);

  void CallVoidMethodA(jobject obj, std::string const& classTag, std::string const& methodTag, jvalue const * args = NULL);
  jint CallIntMethodA(jobject obj, std::string const& classTag, std::string const& methodTag, jvalue const * args = NULL);
  jboolean CallBooleanMethodA(jobject obj, std::string const& classTag, std::string const& methodTag, jvalue const * args = NULL);
  jobject CallObjectMethodA(jobject obj, std::string const& classTag, std::string const& methodTag, jvalue const * args = NULL);
  virtual jobject CallObjectMethod(jobject obj, std::string const& classTag, std::string const& methodTag);

  void DeleteLocalRef(jobject obj);
  jstring toJString(char const * const cStr);
  jstring toJString(char const * const asChar, std::size_t const strLen);

  bool exception_cleared();

  void set_throws(bool);
  bool is_throws();
private:
  enum method_type { CONSTRUCTOR, MEMBER_METHOD, STATIC_METHOD };

  void loadClasses(properties::value_type const& prop);
  void loadMethods(properties::value_type const& prop);
  void processMethod(properties::value_type const& prop, std::string const& label, std::string::size_type labelSize,
                     method_type mType);

  std::string capture_exception(jthrowable e);
  void capture_exception(std::ostream& out, jthrowable e, bool topException = true);
  void exception_check(std::string const& classTag, std::string const& methodTag);

  JavaVM * _jvm;
  JNIEnv * _env;

  typedef std::map<std::string, jclass_metadata> mc_ctr_t;
  mc_ctr_t _managedClasses;
  bool _exceptionCleared;
  bool _throws;
};

}

#endif