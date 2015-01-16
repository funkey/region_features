#ifndef REGION_FEATURES_REGIONPROPS_H__
#define REGION_FEATURES_REGIONPROPS_H__

#include <boost/lexical_cast.hpp>
#include <vigra/accumulator.hxx>

namespace region_features {

template <unsigned int N, typename ValueType, typename LabelType>
class Regionprops {

public:

	struct Parameters {

		Parameters() :
			numHistogramBins(20) {}

		unsigned int numHistogramBins;
	};

	Regionprops(const Parameters& parameters = Parameters()) :
		_parameters(parameters) {}

	template <typename FeatureMap>
	void fill(
			const vigra::MultiArrayView<N, ValueType>& image,
			const vigra::MultiArrayView<N, LabelType>& labels,
			FeatureMap&                                features) {

		using namespace vigra;
		using namespace vigra::acc;

		//typedef UserRangeHistogram<0>        Histogram;
		//typedef StandardQuantiles<Histogram> Quantiles;

		AccumulatorChainArray<
				CoupledArrays<N, ValueType, LabelType>,
				Select<
						DataArg<1>,
						LabelArg<2>,
						RegionCircularity,	// circularity of a 2D region
						RegionEccentricity	// eccentricity of a 2D region
				>
		> accumulator;

		// don't compute features on the background
		accumulator.ignoreLabel(0);

		extractFeatures(image, labels, accumulator);

		for (LabelType i = 1;; i++) {

			if (N == 2) {
				features.append(i, get<RegionCircularity>(accumulator, i));
				features.append(i, get<RegionEccentricity>(accumulator, i));
			}

			if (i == accumulator.maxRegionLabel())
				break;
		}
	}

	void getFeatureNames(std::vector<std::string> featureNames) {

        if (N == 2) {
		    featureNames.push_back("2D region circularity");
		    featureNames.push_back("2D region eccentricity");
		} 
		        
	}

private:

	Parameters _parameters;
};

} // namespace region_features

#endif // REGION_FEATURES_REGIONPROPS_H__

