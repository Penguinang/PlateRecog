#ifndef PLATELOCATOR_V3_H
#define PLATELOCATOR_V3_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
using cv::cvtColor;
using cv::equalizeHist;
using cv::GaussianBlur;
using cv::Mat;
using cv::merge;
using cv::Point;
using cv::Rect;
using cv::RotatedRect;
using cv::Scalar;
using cv::split;

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <utility>
using std::tuple;

/*--------  Forward declarations  --------*/
namespace Doit {
namespace CV {
namespace PlateRecogn {
class PlateInfo;
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

namespace Doit {
namespace CV {
namespace PlateRecogn {
class PlateLocator_V3 {
  public:
    static bool VerifyPlateSize(const cv::Size &size, int minWidth = 60,
                                int maxWidth = 180, int minHeight = 18,
                                int maxHeight = 80, float minRatio = 0.15f,
                                float maxRatio = 0.70f);

  public:
    static vector<PlateInfo> LocatePlatesForCameraAdjust(
        const Mat &matSource, Mat &matProcess, int blur_Size = 5,
        int sobel_Scale = 1, int sobel_Delta = 0, int sobel_X_Weight = 1,
        int sobel_Y_Weight = 0, int morph_Size_Width = 17,
        int morph_Size_Height = 3, int minWidth = 60, int maxWidth = 250,
        int minHeight = 18, int maxHeight = 100, float minRatio = 0.15f,
        float maxRatio = 0.70f);

  public:
    // color:  PlateInfos, threshold, after erode and close, contours, rected
    // sobel(optional): PlateInfos, threshold, after erode and close, contours, rected
    static vector<
        tuple<vector<PlateInfo>, Mat, Mat, vector<vector<Point>>, Mat>>
    LocatePlatesForAutoSample(const Mat &matSource, Mat &matProcess,
                              int blur_Size = 5, int sobel_Scale = 1,
                              int sobel_Delta = 0, int sobel_X_Weight = 1,
                              int sobel_Y_Weight = 0, int morph_Size_Width = 17,
                              int morph_Size_Height = 3, int minWidth = 60,
                              int maxWidth = 180, int minHeight = 18,
                              int maxHeight = 80, float minRatio = 0.15f,
                              float maxRatio = 0.70f);

  public:
    static vector<PlateInfo>
    LocatePlates(const Mat &matSource, int blur_Size = 5, int sobel_Scale = 1,
                 int sobel_Delta = 0, int sobel_X_Weight = 1,
                 int sobel_Y_Weight = 0, int morph_Size_Width = 17,
                 int morph_Size_Height = 3, int minWidth = 60,
                 int maxWidth = 180, int minHeight = 18, int maxHeight = 80,
                 float minRatio = 0.15f, float maxRatio = 0.70f);

  private:
    static vector<PlateInfo>
    LocatePlatesByColor(const Mat &matSource, int blur_Size = 5,
                        int morph_Size_Width = 17, int morph_Size_Height = 3,
                        int minWidth = 60, int maxWidth = 180,
                        int minHeight = 18, int maxHeight = 80,
                        float minRatio = 0.15f, float maxRatio = 0.70f);

  private:
    static vector<PlateInfo> LocatePlatesBySobel(
        Mat matSource, int blur_Size = 5, int sobel_Scale = 1,
        int sobel_Delta = 0, int sobel_X_Weight = 1, int sobel_Y_Weight = 0,
        int morph_Size_Width = 17, int morph_Size_Height = 3, int minWidth = 60,
        int maxWidth = 180, int minHeight = 18, int maxHeight = 80,
        float minRatio = 0.15f, float maxRatio = 0.70f);
};

} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

#endif // !PLATELOCATOR_V3_H
