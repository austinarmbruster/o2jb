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
#include "o2jb_logstream.h"

#include <fstream>

using std::flush;
using std::ofstream;
using std::ostream;
using std::ptrdiff_t;
using std::size_t;
using std::string;

namespace o2jb {
LogStream::LogStream(ostream& sink, size_t buff_sz) : std::streambuf(), _theStream(&sink), _ownTheStream(false), _buffer(buff_sz+1){
    char *base = &_buffer.front();
    //set putbase pointer and endput pointer
    setp(base, base + buff_sz); 
}

LogStream::LogStream(string const& fileName, size_t buff_sz) : std::streambuf(), _ownTheStream(false), _buffer(buff_sz+1) {
	ofstream* pfout = new ofstream();
    pfout->open(fileName, ofstream::out | std::ofstream::app);
	_theStream = pfout;
	_ownTheStream = true;
    char *base = &_buffer.front();
    //set putbase pointer and endput pointer
    setp(base, base + buff_sz); 
}

LogStream::~LogStream() {
	if (_ownTheStream) {
		ofstream* pfout = dynamic_cast<ofstream*>(_theStream);
		pfout->close();
		delete _theStream;
	}
}

int LogStream::sync() {
    return doOutput()? 0:-1;
}

bool LogStream::doOutput() {
    ptrdiff_t n = pptr() - pbase();
    string temp;
    temp.assign(pbase(), n);
    pbump(-n);
    stream() << temp.c_str() << flush;
    return true;
}

ostream& LogStream::stream() {
	return *_theStream;
}

}