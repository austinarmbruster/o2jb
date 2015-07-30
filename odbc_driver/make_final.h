#ifndef _INCLUDE_GUARD_MAKE_FILE_H_
#define _INCLUDE_GUARD_MAKE_FILE_H_ 1
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
/**
 * make_final is a base class used to ensure that another class is not extensible.  The finalized class fits the
 *   following pattern
 * <pre>
 * class is_final : virtual public make_final<is_final> {};
 * </pre>
 *********************************************************************************************************************/
template <typename T>
class make_final
{
private:
  ~make_final() {};
  friend T;
};

#endif