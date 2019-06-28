#ifndef PLATECHAR_SVM_H
#define PLATECHAR_SVM_H

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

#include <iomanip>
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
class CharInfo;
enum class PlateChar_t;
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

#include "csharpImplementations.h"

namespace Doit {
namespace CV {
namespace PlateRecogn {
class PlateChar_SVM {
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
    static vector<float> ComputeHogDescriptors(Mat &image);

    // polyDegree 参数只在核函数是多项式时候其作用
    static bool Train(Mat &samples, Mat &responses,
                      SVM::KernelTypes kernel = SVM::KernelTypes::RBF,
                      float C = 1, float gamma = 1, float polyDegree = 1,
                      unsigned long IterCount = 10000,
                      long double epsilon = 1e-10);
    static void Save(const string &fileName);
    static void Load(const string &fileName);
    static bool IsCorrectTrainngDirectory(const string &path);
    static PlateChar_t Test(Mat &matTest);
    static PlateChar_t Test(const string &fileName);
    static void SaveCharSample(CharInfo &charInfo, const string &libPath);
    static void SaveCharSample(Mat &charMat, PlateChar_t plateChar,
                               const string &libPath,
                               const string &shortFileNameNoExt);
    static void SaveCharSample(Mat &charMat, PlateChar_t plateChar,
                               const string &libPath);
    static bool PrepareCharTrainningDirectory(const string &path);
};

} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

#endif // !PLATECHAR_SVM_H
