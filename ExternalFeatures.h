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

	static void fill(
			const vigra::MultiArrayView<N, ValueType>& image,
			const vigra::MultiArrayView<N, LabelType>& labels,
			FeatureMap<LabelType>&                     features) {

		readFeaturesFile();

		LabelType _, maxLabel;
		labels.minmax(_, maxLabel);

		for (LabelType i = 1;; i++) {

			addFeatures(i);

			if (i == maxLabel)
				break;
		}
	}

private:

	void readFeaturesFile();

	// add the features for label i
	void addFeatures(LabelType i) {

		// add each feature
		//features.append(i, f);
	}
};

#endif // REGION_FEATURES_EXTERNAL_FEATURES_H__

