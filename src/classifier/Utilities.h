#ifndef UTILITIES_H
#define UTILITIES_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>
using cv::Mat;
using cv::Point;
using cv::Rect;
using cv::Scalar;

#include <cmath>
#include <vector>
using namespace std;

namespace Doit {
namespace CV {
namespace PlateRecogn {

class Utilities {
  public:
    static cv::Mat IndexTransform(cv::Mat &originalMat);

    static cv::Mat LogTransform(cv::Mat &originalMat);

    static cv::Mat GammaTransform(cv::Mat &originalMat, float gammaFactor);

    static cv::Mat LaplaceTransform(cv::Mat &originalMat);

    static Rect GetSafeRect(const Rect &rect, const Mat &mat);

    static cv::Mat HistogramTransform(cv::Mat &originalMat);
};

void drawBoundingRects(Mat &InputOutputMat,
                       const vector<vector<Point>> &contours, int index,
                       const Scalar &color);
void reserveBoundingRects(Mat &InputOutputMat,
                       const vector<vector<Point>> &contours, int index,
                       const Scalar &color);
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

#endif
