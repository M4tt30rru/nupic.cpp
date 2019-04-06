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
 * Implementation of the Scalar class
 *
 * A Scalar object is an instance of an NTA_BasicType -- essentially a union
 * It is used internally in the conversion of YAML strings to C++ objects.
 */

#include <nupic/ntypes/Scalar.hpp>
#include <nupic/utils/Log.hpp>

using namespace nupic;

Scalar::Scalar(NTA_BasicType theTypeParam) {
  theType_ = theTypeParam;
  value.uint64 = 0;
}

NTA_BasicType Scalar::getType() const { return theType_; }

// gcc 4.2 complains about the template specializations
// in a different namespace if we don't include this
namespace nupic {

template <> Handle Scalar::getValue<Handle>() const {
  NTA_CHECK(theType_ == NTA_BasicType_Handle);
  return value.handle;
}
template <> Byte Scalar::getValue<Byte>() const {
  NTA_CHECK(theType_ == NTA_BasicType_Byte);
  return value.byte;
}
template <> UInt16 Scalar::getValue<UInt16>() const {
  NTA_CHECK(theType_ == NTA_BasicType_UInt16);
  return value.uint16;
}
template <> Int16 Scalar::getValue<Int16>() const {
  NTA_CHECK(theType_ == NTA_BasicType_Int16);
  return value.int16;
}
template <> UInt32 Scalar::getValue<UInt32>() const {
  NTA_CHECK(theType_ == NTA_BasicType_UInt32);
  return value.uint32;
}
template <> Int32 Scalar::getValue<Int32>() const {
  NTA_CHECK(theType_ == NTA_BasicType_Int32);
  return value.int32;
}
template <> UInt64 Scalar::getValue<UInt64>() const {
  NTA_CHECK(theType_ == NTA_BasicType_UInt64);
  return value.uint64;
}
template <> Int64 Scalar::getValue<Int64>() const {
  NTA_CHECK(theType_ == NTA_BasicType_Int64);
  return value.int64;
}
template <> Real32 Scalar::getValue<Real32>() const {
  NTA_CHECK(theType_ == NTA_BasicType_Real32);
  return value.real32;
}
template <> Real64 Scalar::getValue<Real64>() const {
  NTA_CHECK(theType_ == NTA_BasicType_Real64);
  return value.real64;
}
template <> bool Scalar::getValue<bool>() const {
  NTA_CHECK(theType_ == NTA_BasicType_Bool);
  return value.boolean;
}

///////////////////////////////////////////////////////////////////////////////
//    Compare operators
///////////////////////////////////////////////////////////////////////////////
// Compare contents of two Scalar objects.
bool Scalar::operator==(const Scalar &rhs) const {
  if (theType_ != rhs.getType())
    return false;
  switch (theType_) {
  case NTA_BasicType_Byte:   return (value.byte == rhs.getValue<Byte>());
	case NTA_BasicType_Int16:  return (value.int16 == rhs.getValue<Int16>());
	case NTA_BasicType_UInt16: return (value.uint16 == rhs.getValue<UInt16>());
	case NTA_BasicType_Int32:  return (value.int32 == rhs.getValue<Int32>());
	case NTA_BasicType_UInt32: return (value.uint32 == rhs.getValue<UInt32>());
	case NTA_BasicType_Int64:  return (value.int64 == rhs.getValue<Int64>());
	case NTA_BasicType_UInt64: return (value.uint64 == rhs.getValue<UInt64>());
	case NTA_BasicType_Real32: return (value.real32 == rhs.getValue<Real32>());
	case NTA_BasicType_Real64: return (value.real64 == rhs.getValue<Real64>());
	case NTA_BasicType_Bool:   return (value.boolean == rhs.getValue<bool>());
	default:
	  break;
	}
  return false;
}

std::ostream &operator<< (std::ostream &f, const Scalar &s) {
	switch (theType_) {
	case NTA_BasicType_Byte:   f << value.byte;    break;
	case NTA_BasicType_Int16:  f << value.int16;   break;
	case NTA_BasicType_UInt16: f << value.uint16;  break;
	case NTA_BasicType_Int32:  f << value.int32;   break;
	case NTA_BasicType_UInt32: f << value.uint32;  break;
	case NTA_BasicType_Int64:  f << value.int64;   break;
	case NTA_BasicType_UInt64: f << value.uint64;  break;
	case NTA_BasicType_Real32: f << value.real32;  break;
	case NTA_BasicType_Real64: f << value.real64;  break;
	case NTA_BasicType_Bool:   f << value.boolean; break;
	default:
	  NTA_THROW << "Unexpected Element Type: " << theType_;
	  break;
	}
	return f;
}

} // namespace nupic
