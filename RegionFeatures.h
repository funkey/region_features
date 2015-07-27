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
	 *
	 * @param image
	 *              The raw image.
	 *
	 * @param labels
	 *              A label image that identifies regions: All voxels with the 
	 *              same label are considered as one region.
	 */
	RegionFeatures(
			vigra::MultiArrayView<N, ValueType> image,
			vigra::MultiArrayView<N, LabelType> labels,
			const Parameters& parameters = Parameters()) :
		_image(image),
		_labels(labels),
		_parameters(parameters) {}

	/**
	 * Create a new region feature extraction object. This constructor does not 
	 * set an intensity image, hence only features that don't need those can be 
	 * extracted (like shape features).
	 *
	 * @param labels
	 *              A label image that identifies regions: All voxels with the 
	 *              same label are considered as one region.
	 */
	RegionFeatures(
			vigra::MultiArrayView<N, LabelType> labels,
			const Parameters& parameters = Parameters()) :
		_labels(labels),
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
	void fill(FeatureMap& featureMap);

	/**
	 * Get the names of the features, i.e., the components of the feature 
	 * vectors.
	 */
	std::vector<std::string> getFeatureNames();

private:

	vigra::MultiArrayView<N, ValueType> _image;
	vigra::MultiArrayView<N, LabelType> _labels;
	Parameters _parameters;
};

////////////////////
// IMPLEMENTATION //
////////////////////

template <unsigned int N, typename ValueType, typename LabelType>
template <typename FeatureMap>
void
RegionFeatures<N, ValueType, LabelType>::fill(
		FeatureMap& featureMap) {

	// if statistics are selected
	if (_parameters.computeStatistics) {

		Statistics statistics(_parameters.statisticsParameters);
		statistics.fill(
				_image,
				_labels,
				featureMap);
	}

	// if hape features are selected
	if (_parameters.computeShapeFeatures) {

		ShapeFeatures shapeFeatures(_parameters.shapeFeaturesParameters);
		shapeFeatures.fill(
				_labels,
				featureMap);
	}
}

template <unsigned int N, typename ValueType, typename LabelType>
std::vector<std::string>
RegionFeatures<N, ValueType, LabelType>::getFeatureNames() {

	std::vector<std::string> names;

	// if statistics are selected
	if (_parameters.computeStatistics) {

		Statistics statistics(_parameters.statisticsParameters);
		statistics.getFeatureNames(names);
	}

	// if shapeFeatures is selected
	if (_parameters.computeShapeFeatures) {

		ShapeFeatures shapeFeatures(_parameters.shapeFeaturesParameters);
		shapeFeatures.getFeatureNames(names);
	}

	return names;
}

#endif // REGION_FEATURES_REGION_FEATURES_H__

