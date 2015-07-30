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
#include "properties.h"

#include <exception>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl; 	
using std::exception;
using odbc_jdbc::properties;
using std::ofstream;
using std::ifstream;

void write_properties() {
	properties props;
	props.setProperty("bob", "engineer");
	props.setProperty("phil", "pm");

	ofstream f("test.properties");

	f << props << endl;
	f.close();
}

void read_properties() {
	properties props;
	props.setProperty("bob", "engineer");

	ifstream f("test.properties");

	f >> props;

	cout << props << endl;
	f.close();
}

int main(int argc, char** argv) {
	try {
		write_properties();
		read_properties();
	} catch (exception& e) {
		cout << "Caught an exception:  " << e.what() << endl;
	} catch (...) {
		cout << "Something was thrown that was not a standard exception" << endl;
	}
}