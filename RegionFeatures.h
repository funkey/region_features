#ifndef REGION_FEATURES_REGION_FEATURES_H__
#define REGION_FEATURES_REGION_FEATURES_H__

#include <vector>
#include <vigra/multi_array.hxx>
#include "Statistics.h"
#include "Regionprops.h"

/**
 * Class to compute and store selected region features for a labeled image.
 */
template <unsigned int N, typename ValueType, typename LabelType>
class RegionFeatures {

public:

	// convenience typedefs
	typedef region_features::Statistics<N, ValueType, LabelType> Statistics;
	typedef region_features::Regionprops<N, ValueType, LabelType> Regionprops;

	struct Parameters {

		Parameters() :
			computeStatistics(true),
		    computeRegionprops(false) {}

		bool computeStatistics;
		bool computeRegionprops;

		typename Statistics::Parameters statisticsParameters;
		typename Regionprops::Parameters regionpropsParameters;
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

	// if regionprops is selected
	if (_parameters.computeRegionprops) {

		Regionprops regionprops(_parameters.regionpropsParameters);
		regionprops.fill(
				_image,
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

	// if regionprops is selected
	if (_parameters.computeRegionprops) {

		Regionprops regionprops(_parameters.regionpropsParameters);
		regionprops.getFeatureNames(names);
	}

	return names;
}

#endif // REGION_FEATURES_REGION_FEATURES_H__

