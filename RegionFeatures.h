#ifndef REGION_FEATURES_REGION_FEATURES_H__
#define REGION_FEATURES_REGION_FEATURES_H__

#include <vector>
#include <vigra/multi_array.hxx>
#include "Statistics.h"
#include "ShapeFeatures.h"

/**
 * Class to compute and store selected region features for a labeled image.
 */
template <unsigned int N, typename ValueType, typename LabelType>
class RegionFeatures {

public:

	// convenience typedefs
	typedef region_features::Statistics<N, ValueType, LabelType> Statistics;
	typedef region_features::ShapeFeatures<N, LabelType>         ShapeFeatures;

	struct Parameters {

		Parameters() :
			computeStatistics(true),
		    computeShapeFeatures(false) {}

		bool computeStatistics;
		bool computeShapeFeatures;

		typename Statistics::Parameters statisticsParameters;
		typename ShapeFeatures::Parameters shapeFeaturesParameters;
	};

	/**
	 * Create a new region feature extraction object.
	 */
	RegionFeatures(const Parameters& parameters = Parameters()) :
		_parameters(parameters) {}

	/**
	 * Compute region features and store them in the given feature map. The 
	 * feature map has to model
	 *
	 *   featureMap.append(label, value)
	 *
	 * where label is a LabelType (the label of a region in the label image), 
	 * and value is a ValueType.
	 */
	template <typename FeatureMap>
	void fill(
			vigra::MultiArrayView<N, ValueType> image,
			vigra::MultiArrayView<N, LabelType> labels,
			FeatureMap& featureMap);
	template <typename FeatureMap>
	void fill(
			vigra::MultiArrayView<N, LabelType> labels,
			FeatureMap& featureMap);

	/**
	 * Get the names of the features, i.e., the components of the feature 
	 * vectors.
	 */
	std::vector<std::string> getFeatureNames(std::string prefix = "") const;

private:

	Parameters _parameters;
};

////////////////////
// IMPLEMENTATION //
////////////////////

template <unsigned int N, typename ValueType, typename LabelType>
template <typename FeatureMap>
void
RegionFeatures<N, ValueType, LabelType>::fill(
		vigra::MultiArrayView<N, ValueType> image,
		vigra::MultiArrayView<N, LabelType> labels,
		FeatureMap& featureMap) {

	// if statistics are selected
	if (_parameters.computeStatistics) {

		Statistics statistics(_parameters.statisticsParameters);
		statistics.fill(
				image,
				labels,
				featureMap);
	}

	// if hape features are selected
	if (_parameters.computeShapeFeatures) {

		ShapeFeatures shapeFeatures(_parameters.shapeFeaturesParameters);
		shapeFeatures.fill(
				labels,
				featureMap);
	}
}

template <unsigned int N, typename ValueType, typename LabelType>
template <typename FeatureMap>
void
RegionFeatures<N, ValueType, LabelType>::fill(
		vigra::MultiArrayView<N, LabelType> labels,
		FeatureMap& featureMap) {

	// if hape features are selected
	if (_parameters.computeShapeFeatures) {

		ShapeFeatures shapeFeatures(_parameters.shapeFeaturesParameters);
		shapeFeatures.fill(
				labels,
				featureMap);
	}
}

template <unsigned int N, typename ValueType, typename LabelType>
std::vector<std::string>
RegionFeatures<N, ValueType, LabelType>::getFeatureNames(std::string prefix) const {

	std::vector<std::string> names;

	// if statistics are selected
	if (_parameters.computeStatistics) {

		Statistics statistics(_parameters.statisticsParameters);
		statistics.getFeatureNames(names, prefix);
	}

	// if shapeFeatures is selected
	if (_parameters.computeShapeFeatures) {

		ShapeFeatures shapeFeatures(_parameters.shapeFeaturesParameters);
		shapeFeatures.getFeatureNames(names, prefix);
	}

	return names;
}

#endif // REGION_FEATURES_REGION_FEATURES_H__

