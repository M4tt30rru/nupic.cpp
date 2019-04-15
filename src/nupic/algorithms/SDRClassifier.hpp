/* ---------------------------------------------------------------------
 * Numenta Platform for Intelligent Computing (NuPIC)
 * Copyright (C) 2016, Numenta, Inc.
 *               2019, David McDougall
 *
 * Unless you have an agreement with Numenta, Inc., for a separate license for
 * this software code, the following terms and conditions apply:
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
 * --------------------------------------------------------------------- */

/** @file
 * Definitions for the SDRClassifier.
 */

#ifndef NTA_SDR_CLASSIFIER_HPP
#define NTA_SDR_CLASSIFIER_HPP

#include <deque>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <nupic/types/Types.hpp>
#include <nupic/types/Serializable.hpp>

namespace nupic {
namespace algorithms {
namespace sdr_classifier {


const UInt sdrClassifierVersion = 2;

/**
 * PDF - Probability Density Function, distribution of likelihood of values
 */
using PDF = std::vector<Real64>;

/**
 * The key is the step, for predicting multiple time steps into the future.
 * The key ACTUAL_VALUES contains an estimate of the actual values.
 * The value is a PDF(probability density function, list of probabilities of outcomes) 
 * of the result being in each bucket.
 */
const Int ACTUAL_VALUES = -1;
using ClassifierResult = std::map<Int, PDF>;


class SDRClassifier : public Serializable
{
  // Make test class friend so it can unit test private members directly
  friend class SDRClassifierTest;

  /**
   * 2d map used to store the data.
   * write with Matrix m; m[i][j] = 1.0; //map will always allocate for new i,j index
   * access/read with get_(&m, i, j): as it handles missing values i,j and returns 0.0 for them
   */
  using Matrix = std::map<UInt, std::map<UInt, Real64>>; //Matrix[r][c] = 0.0d


public:
  /**
   * Constructor for use when deserializing.
   */
  SDRClassifier() {}
  void initialize(const std::vector<UInt> &steps, Real64 alpha, Real64 actValueAlpha,
                  UInt verbosity);

  /**
   * Constructor.
   *
   * @param steps The different number of steps to learn and predict.
   * @param alpha The alpha to use when decaying the duty cycles.
   * @param actValueAlpha The alpha to use when decaying the actual
   *                      values for each bucket.
   * @param verbosity The logging verbosity.
   */
  SDRClassifier(const std::vector<UInt> &steps, Real64 alpha, Real64 actValueAlpha,
                UInt verbosity);

  /**
   * Destructor.
   */
  virtual ~SDRClassifier();

  /**
   * Compute the likelihoods for each bucket.
   *
   * @param recordNum An incrementing integer for each record. Gaps in
   *                  numbers correspond to missing records.
   * @param patternNZ The active input bit indices.
   * @param bucketIdx The current value bucket index.
   * @param actValue The current scalar value.
   * @param category Whether the actual values represent categories.
   * @param learn Whether or not to perform learning.
   * @param infer Whether or not to perform inference.
   * @param result A mapping from prediction step to a vector of
   *               likelihoods where the value at an index corresponds
   *               to the bucket with the same index. In addition, the
   *               values for key 0 correspond to the actual values to
   *               used when predicting each bucket.
   */
  virtual void compute(UInt recordNum, const std::vector<UInt> &patternNZ,
                       const std::vector<UInt> &bucketIdxList,
                       const std::vector<Real64> &actValueList, bool category,
                       bool learn, bool infer, ClassifierResult &result);

  /**
   * Returns the class with the greatest probablility.
   */
  UInt getClassification( const PDF & ) const;

  /**
   * Gets the version number
   */
  UInt version() const;

  /**
   * Getter and setter for verbosity level.
   */
  UInt getVerbosity() const;
  void setVerbosity(UInt verbosity);

  /**
   * Gets the learning rate
   */
  Real64 getAlpha() const;

  /**
   * Save the state to the ostream.
   */
  void save(std::ostream &outStream) const override;

  /**
   * Load state from istream.
   */
  void load(std::istream &inStream) override;
	
	CerealAdapter;  // see Serializable.hpp
  // FOR Cereal Serialization
  template<class Archive>
  void save_ar(Archive& ar) const {
    ar(CEREAL_NVP(alpha_),
		   CEREAL_NVP(actValueAlpha_),
		   CEREAL_NVP(maxSteps_),
		   CEREAL_NVP(maxBucketIdx_ ),
		   CEREAL_NVP(maxInputIdx_),
		   CEREAL_NVP(verbosity_),
		   CEREAL_NVP(recordNumHistory_),
		   CEREAL_NVP(steps_),
		   CEREAL_NVP(patternNZHistory_),
		   CEREAL_NVP(weightMatrix_),
			 CEREAL_NVP(actualValues_),
			 CEREAL_NVP(actualValuesSet_));
  }


  // FOR Cereal Deserialization
  template<class Archive>
  void load_ar(Archive& ar) {
    ar(CEREAL_NVP(alpha_),
		   CEREAL_NVP(actValueAlpha_),
		   CEREAL_NVP(maxSteps_),
		   CEREAL_NVP(maxBucketIdx_ ),
		   CEREAL_NVP(maxInputIdx_),
		   CEREAL_NVP(verbosity_),
		   CEREAL_NVP(recordNumHistory_),
		   CEREAL_NVP(steps_),
		   CEREAL_NVP(patternNZHistory_),
		   CEREAL_NVP(weightMatrix_),
			 CEREAL_NVP(actualValues_),
			 CEREAL_NVP(actualValuesSet_));
  }


  /**
   * Compare the other instance to this one.
   *
   * @param other Another instance of SDRClassifier to compare to.
   * @returns true iff other is identical to this instance.
   */
  virtual bool operator==(const SDRClassifier &other) const;

private:
  // Helper function for inference mode
  void infer_(const std::vector<UInt> &patternNZ, const std::vector<Real64> &actValue,
              ClassifierResult &result);

  // Helper function to compute the error signal in learning mode
  std::vector<Real64> calculateError_(const std::vector<UInt> &bucketIdxList,
                                      const std::vector<UInt> patternNZ, UInt step);

  // softmax function
  void softmax_(std::vector<Real64>::iterator begin, std::vector<Real64>::iterator end);

  /**
   * get(x,y) accessor interface for Matrix; handles sparse (missing) values
   * @return return value stored at map[row][col], or defaultVal if such field does not exist
   **/
  Real64 get_(const Matrix& m, const UInt row, const UInt col, const Real64 defaultVal=0.0) const;

  // The list of prediction steps to learn and infer.
  std::vector<UInt> steps_;

  // The alpha used to decay the duty cycles in the BitHistorys.
  Real64 alpha_;

  // The alpha used to decay the actual values used for each bucket.
  Real64 actValueAlpha_;

  // The maximum number of the prediction steps.
  UInt maxSteps_;

  // Stores the input pattern history, starting with the previous input
  // and containing _maxSteps total input patterns.
  std::deque<std::vector<UInt>> patternNZHistory_;
  std::deque<UInt> recordNumHistory_;

  // Weight matrices for the classifier (one per prediction step)
  std::map<UInt, Matrix> weightMatrix_;

  // The highest input bit that the classifier has seen so far.
  UInt maxInputIdx_;

  // The highest bucket index that the classifier has been seen so far.
  UInt maxBucketIdx_;

  // The current actual values used for each bucket index. The index of
  // the actual value matches the index of the bucket.
  std::vector<Real64> actualValues_;

  // A boolean that distinguishes between actual values that have been
  // seen and those that have not.
  std::vector<bool> actualValuesSet_;

  // Version and verbosity.
  UInt version_;
  UInt verbosity_;

}; // end of SDRClassifier class

} // end of namespace sdr_classifier
} // end of namespace algorithms
} // namespace nupic

#endif
