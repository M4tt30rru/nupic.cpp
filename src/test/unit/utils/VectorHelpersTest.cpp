/* ---------------------------------------------------------------------
 * Numenta Platform for Intelligent Computing (NuPIC)
 * Copyright (C) 2016, Numenta, Inc.  Unless you have an agreement
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



#include <vector>
#include <algorithm>
#include "nupic/utils/VectorHelpers.hpp"
#include "gtest/gtest.h"
#include "nupic/types/Types.hpp"

namespace testing { 
    
using namespace nupic::utils;
using namespace nupic;
using namespace std;

TEST(VectorHelpersTest, print_vector)
{
  std::vector<float> v{1.2f, 0.2f, 1.0f, 2.2f, 0.1f};
  VectorHelpers::print_vector<float>(v);
  VectorHelpers::print_vector<float>(v," , ", "Hi there:\n");
  ASSERT_FLOAT_EQ(0.0f, 0.0f);
  vector<string> str = {"a", "b", "c"};
  VectorHelpers::print_vector<string>(str);
  ASSERT_FLOAT_EQ(0.0f, 0.0f);
};


TEST(VectorHelpersTest, castVectorType)
{
  std::vector<float> v{1.2f, 0.2f, 1.0f, 2.2f, 0.1f};
  vector<UInt> expected {1, 0, 1, 2, 0};
  vector<UInt> res = VectorHelpers::castVectorType<float, UInt>(v);
  for(UInt i=0; i< res.size(); i++) { //FIXME how compare vectors?
    ASSERT_EQ(res[i], expected[i]);
  }
};


TEST(VectorHelpersTest, stringToFloatVector)
{
  vector<string> s{"1.2", "0.2", "1", "2.2", "0.1"};
  vector<Real32> expected2 {1.2f, 0.2f, 1.0f, 2.2f, 0.1f};
  vector<Real> res2 = VectorHelpers::stringToFloatVector(s);
  for(UInt i=0; i< res2.size(); i++) { //FIXME how compare vectors?
    ASSERT_EQ(res2[i], expected2[i]);
  }
};


TEST(VectorHelpersTest, binaryToSparse)
{
  vector<Real32> v{0.0f,0.0f,1.0f,1.0f,0.0f};
  vector<UInt> expected {2, 3};
  vector<UInt> res = VectorHelpers::binaryToSparse<Real>(v);
  for(UInt i=0; i< res.size(); i++) {
    ASSERT_EQ(res[i], expected[i]);
  }
};


TEST(VectorHelpersTest, sparseToBinary)
{
  vector<Real32> expected{0.0f,0.0f,1.0f,1.0f,0.0f};
  vector<UInt> v {2u, 3u};
  vector<Real> res = VectorHelpers::sparseToBinary<Real>(v, 5);
  for(size_t i=0; i< res.size(); i++) {
    ASSERT_EQ(res[i], expected[i]);
  }
};


TEST(VectorHelpers, cellsToColumns)
{ // using binary vector 3x3 (3 cols with 3 cells per column)
  vector<UInt> v{0,0,0, 0,1u,1u, 0,0,1u};
  vector<UInt> expected {0, 1u, 1u};
  vector<UInt> res = VectorHelpers::cellsToColumns(v, 3);
  for(size_t i=0; i< res.size(); i++) {
    ASSERT_EQ(res[i], expected[i]);
  }
};


TEST(VectorHelpersTest, sparse_cellsToColumns)
{ 
  
  // using binary vector 3x3 (3 cols with 3 cells per column) as a sparse array
  vector<UInt> v1{ 4,5,8 };
  vector<UInt> expected {1u, 2u};
  vector<UInt> res = VectorHelpers::sparse_cellsToColumns<UInt>(v1, 3);
  for(size_t i=0; i< res.size(); i++) {
    ASSERT_EQ(res[i], expected[i]);
  }

  // bad cellsPerColumn
  EXPECT_THROW(res = VectorHelpers::sparse_cellsToColumns<UInt>(v1, 0),
               std::exception);

  vector<UInt> v2{}; // empty sparse array
  res = VectorHelpers::sparse_cellsToColumns<UInt>(v2, 0);
  EXPECT_EQ(res.size(), 0u);


  vector<UInt> v3{ 4,3,28,9,5 };  // not sorted.
  EXPECT_THROW(res = VectorHelpers::sparse_cellsToColumns<UInt>(v3, 3),
               std::exception);

};

TEST(VectorHelpersTest, unionOfVectors)
{
  vector<UInt> v1{ 1,2,3,4, 25 };
  vector<UInt> v2{ 1,4,5,6,7,8 };
  vector<UInt> res;
  vector<UInt> expected{ 1,2,3,4,5,6,7,8,25 };
  VectorHelpers::unionOfVectors<UInt>(res, v1, v2);
  for(size_t i=0; i< res.size(); i++) {
    ASSERT_EQ(res[i], expected[i]);
  }

  v1.clear();
  VectorHelpers::unionOfVectors<UInt>(res, v1, v2);
  for(size_t i=0; i< res.size(); i++) {
    ASSERT_EQ(res[i], v2[i]);
  }

};
}