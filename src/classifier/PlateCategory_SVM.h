#ifndef PLATECATEGORY_SVM_H
#define PLATECATEGORY_SVM_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>

using cv::HOGDescriptor;
using cv::Mat;
using cv::Ptr;
using cv::TermCriteria;
using cv::ml::SampleTypes;
using cv::ml::SVM;

#include <ctime>
#include <iomanip>
#include <random>
#include <string>
using std::ostringstream;
using std::string;
#include <vector>
using std::vector;
#include <algorithm>
using std::begin;
using std::end;
#include <exception>
using std::exception;
using std::logic_error;


/*--------  Forward declarations  --------*/
namespace Doit {
namespace CV {
namespace PlateRecogn {
class PlateInfo;
enum class PlateCategory_t;
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

#include "csharpImplementations.h"

namespace Doit {
namespace CV {
namespace PlateRecogn {
class PlateCategory_SVM {

  public:
    static bool IsReady;
    static cv::Size HOGWinSize;
    static cv::Size HOGBlockSize;
    static cv::Size HOGBlockStride;
    static cv::Size HOGCellSize;
    static int HOGNBits;

  private:
    static Ptr<SVM> svm;
    static Random random;

  public:
    PlateCategory_SVM() {}
    static void SavePlateSample(PlateInfo &plateInfo, string fileName);
    static void SavePlateSample(Mat &matPlate, PlateCategory_t &plateCategory,
                                string libPath);
    static void SavePlateSample(Mat &matPlate, PlateCategory_t plateCategory,
                                string libPath, string shortFileNameNoExt);

    // use vector to replace array
    static vector<float> ComputeHogDescriptors(Mat &image);
    static bool Train(Mat &samples, Mat &responses,
                      SVM::KernelTypes kernel = SVM::KernelTypes::LINEAR,
                      float C = 1, float gamma = 1, float polyDegree = 1,
                      unsigned long IterCount = 10000,
                      long double epsilon = 1e-10);
    static void Save(const string &fileName);
    static void Load(const string &fileName);
    static bool IsCorrectTrainngDirectory(const string &path);
    static PlateCategory_t Test(Mat &matTest);
    static PlateCategory_t Test(const string &fileName);

    static bool PreparePlateTrainningDirectory(const string &path);
};
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

#endif // !PLATECATEGORY_SVM_H
