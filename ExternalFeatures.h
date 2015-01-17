#ifndef REGION_FEATURES_EXTERNAL_FEATURES_H__
#define REGION_FEATURES_EXTERNAL_FEATURES_H__

#include "FeatureMap.h"

namespace region_features {

/**
 * Feature accumulator that reads external features from a file.
 */
template <unsigned int N, typename ValueType, typename LabelType>
class ExternalFeatures {


public:

	ExternalFeatures(const std::string& filename) {

		readFeaturesFile();
	}

	struct DefaultLabelLineMap;

	/**
	 * Append features for the given images and regions (provided via a label 
	 * image) to a FeatureMap.
	 *
	 * @param image
	 *              The raw image.
	 *
	 * @param labels
	 *              A label image that defines the regions.
	 *
	 * @param features
	 *              The feature map to append to.
	 *
	 * @param labelLineMap
	 *              A functor that implements operator[] to map from LabelType 
	 *              to a line number. If not given, the label number will be 
	 *              used as the line number.
	 */
	template <typename LabelLineMap = DefaultLabelLineMap>
	static void fill(
			const vigra::MultiArrayView<N, ValueType>& image,
			const vigra::MultiArrayView<N, LabelType>& labels,
			FeatureMap<LabelType>&                     features,
			const LabelLineMap&                        labelLineMap = DefaultLabelLineMap()) {

		LabelType _, maxLabel;
		labels.minmax(_, maxLabel);

		for (LabelType i = 1;; i++) {

			addFeatures(i, labelLineMap);

			if (i == maxLabel)
				break;
		}
	}

private:

	/**
	 * Default LabelLineMap to use. Maps each label id to the corresponding line 
	 * number.
	 */
	struct DefaultLabelLineMap {
		std::size_t operator[](LabelType i) { return i; }
	};

	void readFeaturesFile();

	// add the features for label i
	template <typename LabelLineMap>
	void addFeatures(LabelType i, const LabelLineMap& labelLineMap) {

		std::size_t line =

		// add each feature
		//features.append(i, f);
	}
};

#endif // REGION_FEATURES_EXTERNAL_FEATURES_H__

