#ifndef REGION_FEATURES_REGION_FEATURES_H__
#define REGION_FEATURES_REGION_FEATURES_H__

#include <vector>
#include <vigra/multi_array.hxx>
#include "Statistics.h"

/**
 * Class to compute and store selected region features for a labeled image.
 */
template <unsigned int N, typename ValueType, typename LabelType>
class RegionFeatures {

public:

	/**
	 * A selection of the features to compute.
	 */
	struct Selection {

	};

	/**
	 * Compute region features on the given image. Regions are identified by 
	 * having the same label in a separate view.
	 *
	 * @param image
	 *              The raw image.
	 *
	 * @param labels
	 *              A label image that identifies regions.
	 *
	 * @param selection
	 *              A selection of features to compute. If not given, all 
	 *              available features will be computed.
	 */
	void computeFeatures(
			const vigra::MultiArrayView<N, ValueType>& image,
			const vigra::MultiArrayView<N, LabelType>& labels,
			const Selection& selection = Selection());

	/**
	 * Get the computed features for the region with the given label.
	 *
	 * @param region
	 *              The label of the region.
	 *
	 * @return A vector of all features for this region.
	 */
	const std::vector<double>& getFeatures(LabelType region) { return _features[region]; }

private:

	// convenience typedefs
	typedef region_features::Statistics<N, ValueType, LabelType> Statistics;

	FeatureMap<LabelType> _features;
};

////////////////////
// IMPLEMENTATION //
////////////////////

template <unsigned int N, typename ValueType, typename LabelType>
void
RegionFeatures<N, ValueType, LabelType>::computeFeatures(
		const vigra::MultiArrayView<N, ValueType>& image,
		const vigra::MultiArrayView<N, LabelType>& labels,
		const Selection& /*selection*/) {

	_features.clear();

	// if statistics are selected
	RegionFeatures::Statistics::fill(
			image,
			labels,
			_features);
}

#endif // REGION_FEATURES_REGION_FEATURES_H__

