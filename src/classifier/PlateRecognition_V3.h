#ifndef PLATE_RECOGNITION_H
#define PLATE_RECOGNITION_H

#include <memory>
using std::shared_ptr;
#include <algorithm>
using std::max;
using std::min;
using std::sort;
#include <string>
using std::string;

/*--------  Forward declarations  --------*/
namespace Doit {
namespace CV {
namespace PlateRecogn {
class PlateInfo;
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

#include "csharpImplementations.h"

namespace Doit {
namespace CV {
namespace PlateRecogn {
class PlateRecognition_V3 {
  public:
    static vector<PlateInfo> Recognite(Mat &matSource);

    // 返回值可能是null，改成指针
  public:
    static shared_ptr<PlateInfo>
    GetPlateInfoByMutilMethodAndMutilColor(PlateInfo &plateInfo);

  public:
    static bool JudgePlateRightful(const PlateInfo &plateInfo);

    static int GetCharCount(const PlateInfo &plateInfo);

  public:
    static PlateInfo GetPlateInfoByMutilMethod(PlateInfo &plateInfo,
                                               PlateColor_t plateColor);

  public:
    static PlateInfo GetPlateInfo(PlateInfo &plateInfo, PlateColor_t plateColor,
                                  CharSplitMethod_t splitMethod);

  private:
    static void CheckLeftAndRightToRemove(PlateInfo &plateInfo);

  private:
    static void CheckPlateColor(PlateInfo &plateInfo);

  private:
    class PlateInfoComparer_DESC {
      public:
        bool operator()(const PlateInfo &x, const PlateInfo &y);
    };
};
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit
#endif // !PLATE_RECOGNITION_H
