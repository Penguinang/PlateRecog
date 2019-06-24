#ifndef CharSegment_V3_H
#define CharSegment_V3_H

/**
 * 包含这个文件之前先包含 CharInfo.h ， 因为这里使用了 CharSplitMethod_t::Origin
 */

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>
using cv::Scalar;
using cv::Rect;
using cv::Mat;

#include <cmath>
#include <string>
using std::string;
#include <vector>
using std::vector;

/*--------  Forward declarations  --------*/

namespace Doit {
namespace CV {
namespace PlateRecogn {
class CharInfo;
enum class PlateColor_t;
enum class CharSplitMethod_t;
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

namespace Doit {
namespace CV {
namespace PlateRecogn {

class CharSegment_V3 {
  public:
    static cv::Mat ClearMaoding(cv::Mat &threshold);

    static cv::Mat ClearBorder(cv::Mat &threshold);

    static cv::Mat ClearMaodingAndBorder(cv::Mat &gray,
                                         PlateColor_t &plateColor);

    static vector<CharInfo> SpliteCharsInPlateMat(cv::Mat &plateMat,
                                                  vector<Rect> &rects);

    static vector<CharInfo> SplitePlateForAutoSample(cv::Mat &plateMat);

    static vector<CharInfo> SplitePlateByIndexTransform(
        std::vector<std::vector<cv::Point>> &contours, cv::Mat &originalMat,
        PlateColor_t plateColor, int leftLimit = 0, int rightLimit = 0,
        int topLimit = 0, int bottomLimit = 0, int minWidth = 2,
        int maxWidth = 30, int minHeight = 10, int maxHeight = 80,
        float minRatio = 0.08, float maxRatio = 2.0);

    static vector<CharInfo> SplitePlateByLogTransform(
        std::vector<std::vector<cv::Point>> &contours, cv::Mat &originalMat,
        PlateColor_t plateColor, int leftLimit = 0, int rightLimit = 0,
        int topLimit = 0, int bottomLimit = 0, int minWidth = 2,
        int maxWidth = 30, int minHeight = 10, int maxHeight = 80,
        float minRatio = 0.08f, float maxRatio = 2.0f);

    static vector<CharInfo> SplitePlateByGammaTransform(
        std::vector<std::vector<cv::Point>> &contours, Mat &originalMat,
        PlateColor_t plateColor, float gammaFactor = 0.40f, int leftLimit = 0,
        int rightLimit = 0, int topLimit = 0, int bottomLimit = 0,
        int minWidth = 2, int maxWidth = 30, int minHeight = 10,
        int maxHeight = 80, float minRatio = 0.08f, float maxRatio = 2.0f);

    static vector<CharInfo> SplitePlateByOriginal(
        std::vector<std::vector<cv::Point>> &contours, cv::Mat &originalMat,
        cv::Mat &plateMat, PlateColor_t plateColor,
        CharSplitMethod_t charSplitMethod = CharSplitMethod_t::Origin,
        int leftLimit = 0, int rightLimit = 0, int topLimit = 0,
        int bottomLimit = 0, int minWidth = 2, int maxWidth = 30,
        int minHeight = 10, int maxHeight = 80, float minRatio = 0.08f,
        float maxRatio = 2.0);

    static bool VerifyRect(Rect &rect, int minWidth = 2, int maxWidth = 30,
                           int minHeight = 10, int maxHeight = 80,
                           float minRatio = 0.08f, float maxRatio = 2.0);

    static bool NotOnBorder(Rect &rectToJudge, cv::Size borderSize,
                            int leftLimit = 0, int rightLimit = 0,
                            int topLimit = 0, int bottomLimit = 0);

    static Rect MergeRect(Rect &A, Rect &B);

    static vector<Rect> AdjustRects(vector<Rect> &rects);

    static vector<Rect> MergeRects(vector<Rect> &rects);

    static vector<Rect> RejectInnerRectFromRects(vector<Rect> &rects);

    static float GetRectsAverageHeight(vector<Rect> &rects);

    static int GetRectsMaxHeight(vector<Rect> &rects);

    static int GetMedianRectsTop(vector<Rect> &rects);

    static int GetMedianRectsBottom(vector<Rect> &rects);

  private:
    bool static RectTopComparer(const Rect &x, const Rect &y);
    bool static RectBottomComparer(const Rect &x, const Rect &y);
    bool static RectHeightComparer(const Rect &x, const Rect &y);
    bool static RectLeftComparer(const Rect &x, const Rect &y);
    bool static CharInfoLeftComparer(const CharInfo &x, const CharInfo &y);
};

} // namespace PlateRecogn
} // namespace CV
} // namespace Doit
#endif
