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

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <memory>

using std::endl;
using std::make_pair;
using std::string;
using std::getline;

using std::find_if;
using std::not1;
using std::ptr_fun;

namespace {
string const EMPTY_STRING;
}

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

namespace o2jb {

struct properties::prop_impl {
	ctr_t _props;
};

properties::properties() : _pimpl(new properties::prop_impl()) {}

properties::properties(properties const& that) {
	std::auto_ptr<properties::prop_impl> tmp(new properties::prop_impl);

	tmp->_props.insert(that._pimpl->_props.begin(), that._pimpl->_props.end());

	_pimpl = tmp.release();
}

properties::properties(properties&& that) : _pimpl(NULL) {
	using std::swap;
	swap(*this, that);
}

// properties(properties const&& that) = default;
properties::~properties() {
	if (NULL != _pimpl) {
		delete _pimpl;
	}
}

properties& properties::operator=(properties that) {
	swap(*this, that);
	return *this;
}

properties::return_type properties::getProperty(std::string const& key) const {
	ctr_t::const_iterator iter = _pimpl->_props.find(key);

	bool hasKey = _pimpl->_props.end() != iter;
	string const *  strPtr = NULL;
	if (hasKey) {
		strPtr = &(iter->second);
	}
	return make_pair(hasKey, strPtr);
}

std::string const& properties::operator[](std::string const& key) const {
	properties::return_type val = this->getProperty(key);
	return val.first ? *val.second : EMPTY_STRING;
}

void properties::setProperty(std::string const& key, std::string const& value) {
	_pimpl->_props[key] = value;
}

string& properties::operator[](std::string const& key) {
	return _pimpl->_props[key];
}

void properties::insert(const_iterator begin, const_iterator end) {
	_pimpl->_props.insert(begin, end);
}

properties::const_iterator properties::begin() const {
	return _pimpl->_props.begin();
}

properties::const_iterator properties::end() const {
	return _pimpl->_props.end();
}

std::ostream& operator<<(std::ostream& out, std::pair<std::string, std::string> const& kvp) {
	out << kvp.first << "=" << kvp.second;
	return out;
}

std::ostream& operator<<(std::ostream& out, properties const& props) {
	for (properties::ctr_t::const_iterator iter = props._pimpl->_props.begin(); iter != props._pimpl->_props.end(); ++iter) {
		out << *iter << endl;
	}
	return out;
}

std::istream& operator>>(std::istream& in, properties& props) {
	std::string line;
	while (getline(in, line)) {
		// TODO use s.erase
		std::size_t loc = line.find_first_of('#');
		if (string::npos != loc) {
			line = line.substr(0, loc);
		}

		line = trim(line);

		if (!line.empty()) {
			loc = line.find_first_of('=');

			if (string::npos == loc) {
				throw std::runtime_error("Ill formed properties stream");
			} else {
				string key = line.substr(0, loc);
				string value = line.substr(loc + 1, line.length() - loc - 1);
				props.setProperty(key, value);
			}
		}
	}
	return in;
}

void swap(properties& lhs, properties& rhs) {
	using std::swap;
	swap(lhs._pimpl, rhs._pimpl);
}

} // end of namespace