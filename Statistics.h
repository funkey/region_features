#ifndef REGION_FEATURES_STATISTICS_H__
#define REGION_FEATURES_STATISTICS_H__

#include <vigra/accumulator.hxx>
#include "FeatureMap.h"

namespace region_features {

template <unsigned int N, typename ValueType, typename LabelType>
class Statistics {

public:

	static void fill(
			const vigra::MultiArrayView<N, ValueType>& image,
			const vigra::MultiArrayView<N, LabelType>& labels,
			FeatureMap<LabelType>&                     features,
			std::vector<std::string>&                  featureNames) {

		using namespace vigra;
		using namespace vigra::acc;

		typedef UserRangeHistogram<0>        Histogram;
		typedef StandardQuantiles<Histogram> Quantiles;

		AccumulatorChainArray<
				CoupledArrays<N, ValueType, LabelType>,
				Select<
						DataArg<1>,
						LabelArg<2>,
						Count,            // size
						Sum,              // sum of intensity
						Mean,             // mean intensity
						Variance,         // variance of intensity
						Skewness,         // ...
						Kurtosis,         // ...
						Histogram,        // histogram on intensities
						Quantiles,        // 0, 10, 25, 50, 75, 90, 100
						Coord<Variance>,  // variance of cooridinates
						Coord<Skewness>,  // ...
						Coord<Kurtosis>   // ...
				>
		> accumulator;

		const int NumHistogramBins = 20;

		featureNames.push_back("size");
		featureNames.push_back("sum intensity");
		featureNames.push_back("mean intensity");
		featureNames.push_back("variance intensity");
		featureNames.push_back("skewness intensity");
		featureNames.push_back("kurtosis intensity");
		for (int i = 0; i < NumHistogramBins; i++)
			featureNames.push_back(std::string("intensity histogram ") + boost::lexical_cast<std::string>(i));
		featureNames.push_back("intensity quantile 0");
		featureNames.push_back("intensity quantile 10");
		featureNames.push_back("intensity quantile 25");
		featureNames.push_back("intensity quantile 50");
		featureNames.push_back("intensity quantile 75");
		featureNames.push_back("intensity quantile 90");
		featureNames.push_back("intensity quantile 100");
		featureNames.push_back("variance coordinates");
		featureNames.push_back("skewness coordinates");
		featureNames.push_back("kurtosis coordinates");

		// set the histogram options
		vigra::HistogramOptions histogramOptions;
		histogramOptions = histogramOptions.setBinCount(NumHistogramBins); 
		histogramOptions = histogramOptions.setMinMax(0.0, 1.0); 
		accumulator.setHistogramOptions(histogramOptions);

		// don't compute features on the background
		accumulator.ignoreLabel(0);

		extractFeatures(image, labels, accumulator);

		for (LabelType i = 1;; i++) {

			features.safeAdd(i, get<Count>(accumulator, i));
			features.safeAdd(i, get<Sum>(accumulator, i));
			features.safeAdd(i, get<Mean>(accumulator, i));
			features.safeAdd(i, get<Variance>(accumulator, i));
			features.safeAdd(i, get<Skewness>(accumulator, i));
			features.safeAdd(i, get<Kurtosis>(accumulator, i));

			for (unsigned int j = 0; j < 20; j++)
				features.safeAdd(i, get<Histogram>(accumulator, i)[j]);
			for (unsigned int j = 0; j < 7; j++)
				features.safeAdd(i, get<Quantiles>(accumulator, i)[j]);

			for (unsigned int d = 0; d < N; d++)
				features.safeAdd(i, get<Coord<Variance> >(accumulator, i)[d]);
			for (unsigned int d = 0; d < N; d++)
				features.safeAdd(i, get<Coord<Skewness> >(accumulator, i)[d]);
			for (unsigned int d = 0; d < N; d++)
				features.safeAdd(i, get<Coord<Kurtosis> >(accumulator, i)[d]);

			if (i == accumulator.maxRegionLabel())
				break;
		}
	}
};

} // namespace region_features

#endif // REGION_FEATURES_STATISTICS_H__

