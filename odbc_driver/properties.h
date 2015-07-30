#ifndef _INCLUDE_GUARD_PROPERTIES_H_
#define _INCLUDE_GUARD_PROPERTIES_H_ 1
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

#include <istream>
#include <map>
#include <ostream>
#include <string>

namespace o2jb {

	class properties {
	private:
				typedef std::map<std::string, std::string> ctr_t;
	public:
		properties();
		properties(properties const& that);
		properties(properties&& that);
		~properties();
		properties& operator=(properties const that);

		typedef std::pair<bool, std::string const * const> return_type;

		return_type getProperty(std::string const& key) const;
		void setProperty(std::string const& key, std::string const& value);

		std::string& operator[](std::string const& key);
		std::string const& operator[](std::string const& key) const;

		friend std::ostream& operator<<(std::ostream& out, properties const & props);
		friend std::istream& operator>>(std::istream& in, properties& props);

		friend void swap(properties& lhs, properties& rhs);

		typedef ctr_t::iterator iterator;
		typedef ctr_t::const_iterator const_iterator;
		typedef ctr_t::value_type value_type;

		void insert(const_iterator begin, const_iterator end);
		
		const_iterator begin() const;
		const_iterator end() const;
	private:
  	struct prop_impl;
		prop_impl* _pimpl;
	};	
}

#endif