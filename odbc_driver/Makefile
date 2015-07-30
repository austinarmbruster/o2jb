# Copyright 2015 AnaVation, LLC. 
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

all: o2jb.dll myodbctest.exe sqltables.exe logexample.exe

JAVAHOME=/cygdrive/c/progra~1/Java/jdk1.7.0_45

CXXFLAGS=-I$(JAVAHOME)/include -I$(JAVAHOME)/include/win32 --pedantic-errors -Wall -Werror -std=c++11 -std=gnu++11 -ggdb3 -O0

o2jb.dll: json_util.o properties.o o2jb.o o2jb_env.o o2jb_connection.o o2jb_statement.o BindData.o o2jb_common.o o2jb_config.o jclass_metadata.o JvmManager.o o2jb_local_ref.o o2jb_install.o o2jb_logstream.o o2jb_logging.o TimeReporter.o RegistryKey.o resource.o
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -L$(JAVAHOME)/jre/bin -L$(JAVAHOME)/jre/bin/server -shared -o o2jb.dll json_util.o properties.o BindData.o o2jb_statement.o o2jb_connection.o o2jb_env.o o2jb_common.o jclass_metadata.o JvmManager.o o2jb_local_ref.o o2jb_install.o o2jb_config.o o2jb.o RegistryKey.o o2jb_logstream.o o2jb_logging.o TimeReporter.o resource.o -Wl,-s,--out-implib,libo2jb.a -static-libgcc -static-libstdc++

resource.o: resource.rc
	windres -i resource.rc -o resource.o

properties.o: properties.cpp properties.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c properties.cpp

o2jb.o: o2jb.cpp o2jb.h o2jb_env.h o2jb_connection.h o2jb_statement.h properties.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c -DBUILDING_O2JB_DLL o2jb.cpp

o2jb_config.o: o2jb_config.cpp o2jb_config.h JvmManager.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c -DBUILDING_O2JB_DLL o2jb_config.cpp

o2jb_install.o: o2jb_install.cpp o2jb_install.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c -DBUILDING_O2JB_DLL o2jb_install.cpp

o2jb_env.o: o2jb_env.cpp o2jb_env.h o2jb_common.h properties.h JvmManager.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c o2jb_env.cpp

json_util.o: json_util.cpp json_util.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c json_util.cpp

JvmManager.o: JvmManager.cpp JvmManager.h properties.h RegistryKey.h jclass_metadata.h java_error.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c JvmManager.cpp

jclass_metadata.o: jclass_metadata.cpp jclass_metadata.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c jclass_metadata.cpp

o2jb_connection.o: o2jb_connection.cpp o2jb_connection.h o2jb_env.h o2jb_common.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c o2jb_connection.cpp

o2jb_statement.o: o2jb_statement.cpp o2jb_statement.h o2jb_connection.h o2jb_env.h o2jb_common.h BindData.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c o2jb_statement.cpp

BindData.o: BindData.cpp BindData.h o2jb_common.h o2jb_statement.h o2jb_connection.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c BindData.cpp

o2jb_common.o: o2jb_common.cpp o2jb_common.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c o2jb_common.cpp

o2jb_logging.o: o2jb_logging.cpp o2jb_logging.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c o2jb_logging.cpp

o2jb_logstream.o: o2jb_logstream.cpp o2jb_logstream.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c o2jb_logstream.cpp

o2jb_local_ref.o: o2jb_local_ref.cpp o2jb_local_ref.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c o2jb_local_ref.cpp

logexample.o: logexample.cpp
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c logexample.cpp

TimeReporter.o: TimeReporter.cpp TimeReporter.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c TimeReporter.cpp

RegistryKey.o: RegistryKey.cpp RegistryKey.h
	x86_64-w64-mingw32-g++ $(CXXFLAGS) -c RegistryKey.cpp

myodbctest.exe: o2jb.dll odbcsql.c o2jb_logstream.o o2jb_logging.o 
	x86_64-w64-mingw32-g++ $(CXXFLAGS) odbcsql.c o2jb_logstream.o o2jb_logging.o TimeReporter.o -o myodbctest.exe -static-libgcc -static-libstdc++ -lodbc32

sqltables.exe: o2jb.dll sqltables.cpp
	x86_64-w64-mingw32-g++ $(CXXFLAGS) sqltables.cpp -o sqltables.exe -static-libgcc -static-libstdc++ -lodbc32

logexample.exe: TimeReporter.o o2jb_logging.o o2jb_logstream.o logexample.o properties.o
	x86_64-w64-mingw32-g++ -L$(JAVAHOME)/jre/bin -L$(JAVAHOME)/jre/bin/server $(CXXFLAGS) properties.o TimeReporter.o o2jb_logstream.o o2jb_logging.o logexample.o -o logexample.exe -static-libgcc -static-libstdc++

clean:
	rm *.o myodbctest.exe sqltables.exe logexample.exe o2jb.dll libo2jb.a