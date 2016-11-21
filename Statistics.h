#ifndef REGION_FEATURES_STATISTICS_H__
#define REGION_FEATURES_STATISTICS_H__

#include <boost/lexical_cast.hpp>
#include <vigra/accumulator.hxx>

namespace region_features {

template <unsigned int N, typename ValueType, typename LabelType>
class Statistics {

public:

	struct Parameters {

		Parameters() :
			computeCoordinateStatistics(true),
			numHistogramBins(20) {}

		bool computeCoordinateStatistics;
		unsigned int numHistogramBins;
	};

	Statistics(const Parameters& parameters = Parameters()) :
		_parameters(parameters) {}

	template <typename FeatureMap>
	void fill(
			const vigra::MultiArrayView<N, ValueType>& image,
			const vigra::MultiArrayView<N, LabelType>& labels,
			FeatureMap&                                features) {

		using namespace vigra;
		using namespace vigra::acc;

		typedef UserRangeHistogram<0>        Histogram;
		typedef StandardQuantiles<Histogram> Quantiles;

//		if (_parameters.computeCoordinateStatistics) {
			AccumulatorChainArray<
					CoupledArrays<N, ValueType, LabelType>,
					Select<
							DataArg<1>,
							LabelArg<2>,
							Count,            	// size
							Sum,              	// sum of intensity
							Mean,             	// mean intensity
							Variance,         	// variance of intensity
							Skewness,         	// ...
							Kurtosis,         	// ...
							Histogram,        	// histogram on intensities
							Quantiles,        	// 0, 10, 25, 50, 75, 90, 100
							Coord<Variance>,  	// variance of cooridinates
							Coord<Skewness>,  	// ...
							Coord<Kurtosis>   	// ...
					>
			> accumulator;
//		} else {
//			AccumulatorChainArray<
//					CoupledArrays<N, ValueType, LabelType>,
//					Select<
//							DataArg<1>,
//							LabelArg<2>,
//							Count,            	// size
//							Sum,              	// sum of intensity
//							Mean,             	// mean intensity
//							Variance,         	// variance of intensity
//							Skewness,         	// ...
//							Kurtosis,         	// ...
//							Histogram,        	// histogram on intensities
//							Quantiles        	// 0, 10, 25, 50, 75, 90, 100
//					>
//			> accumulator;
//		}

		// set the histogram options
		vigra::HistogramOptions histogramOptions;
		histogramOptions = histogramOptions.setBinCount(_parameters.numHistogramBins);
		histogramOptions = histogramOptions.setMinMax(0.0, 1.0);
		accumulator.setHistogramOptions(histogramOptions);

		// don't compute features on the background
		accumulator.ignoreLabel(0);

		extractFeatures(image, labels, accumulator);

		for (LabelType i = 1;; i++) {

			features.append(i, get<Count>(accumulator, i));
			features.append(i, get<Sum>(accumulator, i));
			features.append(i, get<Mean>(accumulator, i));
			features.append(i, get<Variance>(accumulator, i));
			features.append(i, get<Skewness>(accumulator, i));
			features.append(i, get<Kurtosis>(accumulator, i));

			features.append(i, getAccumulator<Histogram>(accumulator, i).left_outliers);
			for (unsigned int j = 0; j < _parameters.numHistogramBins; j++)
				features.append(i, get<Histogram>(accumulator, i)[j]);
			features.append(i, getAccumulator<Histogram>(accumulator, i).right_outliers);

			for (unsigned int j = 0; j < 7; j++)
				features.append(i, get<Quantiles>(accumulator, i)[j]);

			if (_parameters.computeCoordinateStatistics) {
				for (unsigned int d = 0; d < N; d++)
					features.append(i, get<Coord<Variance> >(accumulator, i)[d]);
				for (unsigned int d = 0; d < N; d++)
					features.append(i, get<Coord<Skewness> >(accumulator, i)[d]);
				for (unsigned int d = 0; d < N; d++)
					features.append(i, get<Coord<Kurtosis> >(accumulator, i)[d]);
			}
			if (i == accumulator.maxRegionLabel())
				break;
		}
	}

	void getFeatureNames(std::vector<std::string>& featureNames, std::string prefix) {

		featureNames.push_back(prefix + "size");
		featureNames.push_back(prefix + "sum intensity");
		featureNames.push_back(prefix + "mean intensity");
		featureNames.push_back(prefix + "variance intensity");
		featureNames.push_back(prefix + "skewness intensity");
		featureNames.push_back(prefix + "kurtosis intensity");
		featureNames.push_back(prefix + "intensity histogram right outliers");
		for (int i = 0; i < _parameters.numHistogramBins; i++)
			featureNames.push_back(prefix + std::string("intensity histogram ") + boost::lexical_cast<std::string>(i));
		featureNames.push_back(prefix + "intensity histogram left outliers");
		featureNames.push_back(prefix + "intensity quantile 0");
		featureNames.push_back(prefix + "intensity quantile 10");
		featureNames.push_back(prefix + "intensity quantile 25");
		featureNames.push_back(prefix + "intensity quantile 50");
		featureNames.push_back(prefix + "intensity quantile 75");
		featureNames.push_back(prefix + "intensity quantile 90");
		featureNames.push_back(prefix + "intensity quantile 100");
		if (_parameters.computeCoordinateStatistics) {
			for (int d = 0; d < N; d++)
				featureNames.push_back(prefix + std::string("variance coordinates ") + boost::lexical_cast<std::string>(d));
			for (int d = 0; d < N; d++)
				featureNames.push_back(prefix + std::string("skewness coordinates ") + boost::lexical_cast<std::string>(d));
			for (int d = 0; d < N; d++)
				featureNames.push_back(prefix + std::string("kurtosis coordinates ") + boost::lexical_cast<std::string>(d));
		}
	}

private:

	Parameters _parameters;
};

} // namespace region_features

#endif // REGION_FEATURES_STATISTICS_H__

