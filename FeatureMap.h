#ifndef REGION_FEATURES_FEATURE_MAP_H__
#define REGION_FEATURES_FEATURE_MAP_H__

#include <map>
#include <vector>

/**
 * A map from region labels to their feature vectors.
 */
template <typename LabelType>
class FeatureMap {

	typedef std::vector<double> features_type;

public:

	/**
	 * Clear the feature map.
	 */
	inline void clear() { _features.clear(); }

	/**
	 * Get the feature vector of the given label.
	 */
	inline features_type& operator[](LabelType i) { return _features[i]; }

	/**
	 * Get the feature vector of the given label.
	 */
	inline const features_type& operator[](LabelType i) const { return _features.at(i); }

	/**
	 * Add a single feature to the feature vector of the given label. Converts 
	 * nan into 0.
	 */
	inline void safeAdd(LabelType i, double feature) {

		if (feature != feature)
			feature = 0;

		_features[i].push_back(feature);
	}

private:

	std::map<LabelType, features_type> _features;
};

#endif // REGION_FEATURES_FEATURE_MAP_H__

