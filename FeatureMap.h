#ifndef REGION_FEATURES_FEATURE_MAP_H__
#define REGION_FEATURES_FEATURE_MAP_H__

#include <map>
#include <vector>

/**
 * A map from region labels to their feature vectors.
 */
template <typename LabelType>
class FeatureMap {

public:

	typedef std::vector<double>                       features_type;
	typedef std::map<LabelType, features_type>        feature_map_type;
	typedef typename feature_map_type::iterator       iterator;
	typedef typename feature_map_type::const_iterator const_iterator;

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

	/**
	 * Direct access to the map from labels to feature vectors.
	 */
	iterator begin() { return _features.begin(); }
	const_iterator begin() const { return _features.begin(); }
	iterator end() { return _features.end(); }
	const_iterator end() const { return _features.end(); }

private:

	std::map<LabelType, features_type> _features;
};

#endif // REGION_FEATURES_FEATURE_MAP_H__

