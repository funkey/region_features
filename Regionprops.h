#ifndef REGION_FEATURES_REGIONPROPS_H__
#define REGION_FEATURES_REGIONPROPS_H__

#include <boost/lexical_cast.hpp>
#include <vigra/accumulator.hxx>
#include <vigra/polygon.hxx>

namespace region_features {

template <unsigned int N, typename ValueType, typename LabelType>
class Regionprops {

public:

	struct Parameters {

		Parameters() :
			numAnglePoints(50),
			contourVecAsArcSegmentRatio(0.1),
			numAngleHistBins(16) {}

		int numAnglePoints;
		double contourVecAsArcSegmentRatio;
		const int numAngleHistBins;
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
		Polygon<>		   			regionContours;
		typedef Polygon<>::Point	Point;

		AccumulatorChainArray<
				CoupledArrays<N, ValueType, LabelType>,
				Select<
						DataArg<1>,
						LabelArg<2>,
						RegionContour,
						RegionCircularity,	// circularity of a 2D region
						RegionEccentricity	// eccentricity of a 2D region
				>
		> accumulator;

		// don't compute features on the background
		accumulator.ignoreLabel(0);

		extractFeatures(image, labels, accumulator);

		for (LabelType i = 1;; i++) {
			if (N == 2) {
				
				////////////////////////////////////
				//The code assumes that the contour is sampled uniformly, in counter-clockwise direction, and is defined by a pair of vectors of equal length (x and y). resolution alters the number of neighboring points the code takes into account in computing the tangent direction at any given point.
				////////////////////////////////////
				
				regionContours = get<RegionContour>(accumulator, i);
				//CHECK WHETHER last point is the same as the first on, i.e. closed contour
				//int numAnglePoints = 50;
				double arcDist = 1.0/_parameters.numAnglePoints*_parameters.contourVecAsArcSegmentRatio;
				double avgAngle = 0;
				//const int numAngleHistBins = 16;
				int angleHist[_parameters.numAngleHistBins] = {0};

				//assumes counter clockwise order
				for (int ithAnglePoint=0; ithAnglePoint<_parameters.numAnglePoints; ithAnglePoint++) {

					double quantileCenter = (double)ithAnglePoint/_parameters.numAnglePoints;
					double c=regionContours.arcLengthQuantile(quantileCenter);
					Point centerPt = regionContours.interpolate((int)floor(c), c - floor(c));

					double prev=regionContours.arcLengthQuantile((quantileCenter-arcDist)>0 ? quantileCenter-arcDist : 1-arcDist+quantileCenter);
					Point prevPt = regionContours.interpolate((int)floor(prev), prev - floor(prev));

					double next=regionContours.arcLengthQuantile((quantileCenter+arcDist)>1 ? quantileCenter+arcDist-1 : quantileCenter+arcDist);
					Point nextPt = regionContours.interpolate((int)floor(next), next - floor(next));

					Point prevVec = prevPt - centerPt;
					Point nextVec = nextPt - centerPt;

					double cos_angle = std::min(1.0, std::max(-1.0, (prevVec[0]*nextVec[0]+prevVec[1]*nextVec[1]) / (norm(prevVec)*norm(nextVec))));

					double angle = acos( cos_angle );

					int binID = (int)floor(angle/M_PI*_parameters.numAngleHistBins);

					if (binID==_parameters.numAngleHistBins)
						binID--;
					angleHist[binID] += 1;
					avgAngle += angle;

				}

				////////////////////////////////////

				features.append(i, avgAngle);
				for (unsigned int d = 0; d < _parameters.numAngleHistBins; d++)
					features.append(i, angleHist[d]);
				features.append(i, get<RegionCircularity>(accumulator, i));
				features.append(i, get<RegionEccentricity>(accumulator, i));
			}

			if (i == accumulator.maxRegionLabel())
				break;
		}
	}

	void getFeatureNames(std::vector<std::string> featureNames) {

        if (N == 2) {
        	featureNames.push_back("2D region pointyness");
		    featureNames.push_back("2D region circularity");
		    featureNames.push_back("2D region eccentricity");
		} 
		        
	}

//	computeContourPointyness() {
//		////////////////////////////////////
//		//The code assumes that the contour is sampled uniformly, in counter-clockwise direction, and is defined by a pair of vectors of equal length (x and y). resolution alters the number of neighboring points the code takes into account in computing the tangent direction at any given point.
//		////////////////////////////////////
//
//		regionContours = get<RegionContour>(accumulator, i);
//		//CHECK WHETHER last point is the same as the first on, i.e. closed contour
//		int numAnglePoints = 50;
//		double arcDist = 1.0/numAnglePoints*0.1;
//		double avgAngle = 0;
//		const int numAngleHistBins = 16;
//		int angleHist[numAngleHistBins] = {0};
//
//		//assumes counter clockwise order
//		for (int ithAnglePoint=0; ithAnglePoint<numAnglePoints; ithAnglePoint++) {
//
//			double quantileCenter = (double)ithAnglePoint/numAnglePoints;
//			double c=regionContours.arcLengthQuantile(quantileCenter);
//			Point centerPt = regionContours.interpolate((int)floor(c), c - floor(c));
//
//			double prev=regionContours.arcLengthQuantile((quantileCenter-arcDist)>0 ? quantileCenter-arcDist : 1-arcDist+quantileCenter);
//			Point prevPt = regionContours.interpolate((int)floor(prev), prev - floor(prev));
//
//			double next=regionContours.arcLengthQuantile((quantileCenter+arcDist)>1 ? quantileCenter+arcDist-1 : quantileCenter+arcDist);
//			Point nextPt = regionContours.interpolate((int)floor(next), next - floor(next));
//
//			Point prevVec = prevPt - centerPt;
//			Point nextVec = nextPt - centerPt;
//
//			double cos_angle = std::min(1.0, std::max(-1.0, (prevVec[0]*nextVec[0]+prevVec[1]*nextVec[1]) / (norm(prevVec)*norm(nextVec))));
//
//			double angle = acos( cos_angle );
//
//			int binID = (int)floor(angle/M_PI*numAngleHistBins);
//
//			if (binID==numAngleHistBins)
//				binID--;
//			angleHist[binID] += 1;
//			avgAngle += angle;
//
//		}
//	}

private:

	Parameters _parameters;
};

} // namespace region_features

#endif // REGION_FEATURES_REGIONPROPS_H__

