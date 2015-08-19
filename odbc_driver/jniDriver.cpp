#include "JvmManager.h"

#include <exception>
#include <vector>
#include <iostream>

using std::cout; using std::endl;
using o2jb::JvmManager;

int main() {
  try {
    JvmManager jvm("", std::vector<std::string>(), JvmManager::registryPath());
    jvm.loadConfig("jvmDriver.properties");

    JNIEnv* env = jvm.env();

    char const* hw = "Hello World";
    jstring jHw = env->NewStringUTF(hw);

    char const* regex = "World";
    jstring jRegex = env->NewStringUTF(regex);

    char const* replacement = "Bob";
    jstring jReplacement = env->NewStringUTF(replacement);

    const char* cppHw = env->GetStringUTFChars(jHw, 0);
    cout << cppHw << endl;

    jstring jResult = (jstring) jvm.CallObjectMethodA(jHw, "string", "replaceAll", o2jb::make_args("LL", jRegex, jReplacement).get());
    cout << "abc" << endl;
    const char* cppResult = env->GetStringUTFChars(jResult, 0);
    cout << cppResult << endl;
  } catch (std::exception& e) {
    cout << "Activity failed:  " << e.what() << endl;
  }
  return 0;
}