/* ---------------------------------------------------------------------
 * Numenta Platform for Intelligent Computing (NuPIC)
 * Copyright (C) 2013, Numenta, Inc.  Unless you have an agreement
 * with Numenta, Inc., for a separate license for this software code, the
 * following terms and conditions apply:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero Public License for more details.
 *
 * You should have received a copy of the GNU Affero Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 *
 * http://numenta.org/licenses/
 * ---------------------------------------------------------------------
 */

/** @file
 * Definitions for the Value class
 *
 * A Value object holds a Scalar or an Array
 * A ValueMap is essentially a map<string, Value>
 * It is used internally in the conversion of YAML strings to C++ objects.
 * The API and implementation are geared towards clarify rather than
 * performance, since it is expected to be used only during network construction.
 */

#ifndef NTA_VALUE_HPP
#define NTA_VALUE_HPP


#include <map>
#include <nupic/ntypes/Array.hpp>
#include <nupic/ntypes/Scalar.hpp>
#include <nupic/types/BasicType.hpp>
#include <nupic/types/Types.hpp>
#include <nupic/types/Serializable.hpp>
#include <string>

namespace nupic {

/**
 * The Value class is used to store construction parameters
 * for regions and links. A YAML string specified by the user
 * is parsed and converted into a set of Values.
 *
 * A Value is essentially a union of Scalar/Array/string.
 * In turn, a Scalar is a union of NTA_BasicType types,
 * and an Array is an array of such types.
 *
 * A string is similar to an Array of NTA_BasicType_Byte, but
 * is handled differently, so it is separated in the API.
 *
 * The Value API uses std::shared_ptr instead of directly
 * using the underlying objects, to avoid copying, and because
 * Array may not be copied.
 */
class Value : public Serializable {
public:
  Value(const Scalar &s);
  Value(const Array &a);
  Value(const std::string &s);

  enum Category { scalar=0, array, string };

  bool isArray() const;
  bool isString() const;
  bool isScalar() const;
  Category getCategory() const;

  NTA_BasicType getType() const;

  const Scalar& getScalar() const;

  const Array& getArray() const;

  std::string getString() const;

  template <typename T> T getScalarT() const;

  const std::string getDescription() const;

  CerealAdapter;  // see Serializable.hpp
  template<class Archive>
  void save_ar(Archive & ar) const {
    switch(category_) {
    case Category::scalar: ar(category_, scalar_); break;
    case Category::array: ar(category_, array_);   break;
    case Category::string: ar(category_, string_); break;
    default: break;
    }
  }
  template<class Archive>
  void load_ar(Archive & ar) {
    ar(category_);
    switch(category_) {
    case Category::scalar: ar(scalar_); break;
    case Category::array: ar(array_);   break;
    case Category::string: ar(string_); break;
    default: break;
    }
  }




private:
  // Default constructor would not be useful
  Value();

  Category category_;
  Scalar scalar_;
  Array array_;
  std::string string_;
};

std::ostream &operator<< (std::ostream &f, const Value &s);


class ValueMap : public Serializable {
public:
  ValueMap() { };
  ValueMap(const ValueMap &rhs) {
    map_.clear();
    for (const auto &rh : rhs) {
      map_.insert(std::make_pair(rh.first, rh.second));
    }
  }
  ~ValueMap() { map_.clear(); }

  void add(const std::string &key, const Value &value);

  // map.find(key) != map.end()
  bool contains(const std::string &key) const;

  // map.find(key) + exception if not found
  const Value &getValue(const std::string &key) const;

  // Method below are for convenience, bypassing the Value
  const Array& getArray(const std::string &key) const;
  const Scalar& getScalar(const std::string &key) const;
  std::string getString(const std::string &key) const;
  std::string getString(const std::string &key,  const std::string defaultValue) const;

  // More convenience methods, bypassing the Value and the contained Scalar

  // use default value if not specified in map
  template <typename T>
  T getScalarT(const std::string &key, T defaultValue) const;

  // raise exception if value is not specified in map
  template <typename T> T getScalarT(const std::string &key) const;

  void dump() const;

  typedef std::map<std::string, Value>::const_iterator const_iterator;
  const_iterator begin() const;
  const_iterator end() const;

  CerealAdapter;   // see Serializable.hpp
  template<class Archive>
  void save_ar(Archive & ar) const {
    ar(map_);
  }
  template<class Archive>
  void load_ar(Archive & ar) {
    ar(map_);
  }


private:
  typedef std::map<std::string, Value>::iterator iterator;
  std::map<std::string, Value> map_;
};

} // namespace nupic

#endif // NTA_VALUE_HPP
