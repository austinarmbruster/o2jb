#include "JvmManager.h"

#include <iostream>
#include <string>

using std::cout; using std::endl;
using std::string;

using o2jb::JvmManager;
using o2jb::make_args;

int main() {
  try {
    JvmManager jvm("h2-1.3.175.jar;commons-dbcp-1.4.jar;commons-pool-1.5.4.jar", std::vector<std::string>(), JvmManager::registryPath());
    jvm.loadConfig("jvmManager.properties");

    JNIEnv* env = jvm.env();

    jobject ds = jvm.NewObjectA("ds", "defCon");

    string cDriver = "org.h2.Driver";
    jstring driver = env->NewStringUTF(cDriver.c_str());

    string cUrl = "jdbc:h2:mem:test;DB_CLOSE_DELAY=-1";
    jstring url = env->NewStringUTF(cUrl.c_str());

    string cUser = "user";
    jstring user = env->NewStringUTF(cUser.c_str());

    string cPwd = "password";
    jstring pwd = env->NewStringUTF(cPwd.c_str());

    jstring createSql = env->NewStringUTF("CREATE TABLE data (key VARCHAR(255) PRIMARY KEY, value VARCHAR(1023))");
    jstring insertSql = env->NewStringUTF("insert into data values ('key','value')");
    jstring selectSql = env->NewStringUTF("select * from data");

    jvm.CallVoidMethodA(ds, "ds", "setDriver", make_args("L", driver).get());
    jvm.CallVoidMethodA(ds, "ds", "setUrl", make_args("L", url).get());
    jvm.CallVoidMethodA(ds, "ds", "setUser", make_args("L", user).get());
    jvm.CallVoidMethodA(ds, "ds", "setPwd", make_args("L", pwd).get());

    jobject conn = jvm.CallObjectMethod(ds, "ds", "getConn");
    jobject stmt = jvm.CallObjectMethod(conn, "conn", "createStatement");

    jvm.CallIntMethodA(stmt, "stmt", "execUpdate", make_args("I", createSql).get());
    jvm.CallIntMethodA(stmt, "stmt", "execUpdate", make_args("I", insertSql).get());

    jobject rs = jvm.CallObjectMethodA(stmt, "stmt", "execQuery", make_args("L", selectSql).get());

    while (JNI_TRUE == jvm.CallBooleanMethodA(rs, "rs", "next")) {
      cout << env->GetStringUTFChars((jstring)jvm.CallObjectMethodA(rs, "rs", "str", make_args("I", 1).get()), 0)
           << "," << env->GetStringUTFChars((jstring)jvm.CallObjectMethodA(rs, "rs", "str", make_args("I", 2).get()), 0)
           << endl;
    }

  } catch (std::exception& e) {
    cout << "Failed execution:  " << e.what() << endl;
  }
  return 0;
}
