#ifndef PLATECATEGORY_SVM_H
#define PLATECATEGORY_SVM_H


#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>

using cv::Mat;
using cv::ml::SVM;
using cv::HOGDescriptor;
using cv::TermCriteria;
using cv::ml::SampleTypes;
using cv::Ptr;

#include <random>
#include <ctime>
#include <iomanip>
#include <string>
using std::string;
using std::ostringstream;
#include <vector>
using std::vector;
#include <algorithm>
using std::begin;
using std::end;
#include <exception>
using std::exception;
using std::logic_error;


#include "csharpImplementations.h"
#include "CharInfo.h"

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
    static void SavePlateSample(PlateInfo &plateInfo, string fileName) {
        // HACK
        // plateInfo.OriginalMat.SaveImage (fileName);
        cv::imwrite(fileName, plateInfo.OriginalMat);
    }
    static void SavePlateSample(Mat &matPlate, PlateCategory_t &plateCategory,
                                string libPath) {
        DateTime now = DateTime::Now();
        // string name =
        //     string.Format("{0}-{1:00}-{2:00}-{3:00}-{4:00}-{5:00}-{6:000000}",
        //                   now.Year, now.Month, now.Day, now.Hour, now.Minute,
        //                   now.Second, random.Next(100000));
        ostringstream buffer;
        buffer << now.Year << "-" << std::setfill('0') << std::setw(2)
               << now.Month << std::setfill('0') << std::setw(2) << "-"
               << std::setfill('0') << std::setw(2) << now.Day << "-"
               << std::setfill('0') << std::setw(2) << now.Hour
               << std::setfill('0') << std::setw(2) << "-" << std::setfill('0')
               << std::setw(2) << now.Minute << "-" << std::setfill('0')
               << std::setw(2) << now.Second << "-" << std::setfill('0')
               << std::setw(6) << random.Next(100000);
        string name = buffer.str();
        buffer.clear();

        // string fileName = string.Format(@"{0}\plates\{1}\{2}.jpg", libPath,
        //                                 plateCategory, name);
        buffer << libPath << "" DIRECTORY_DELIMITER"plates" DIRECTORY_DELIMITER"" << plateCategory << "" DIRECTORY_DELIMITER"" << name << ".jpg";
        string fileName = buffer.str();

        // matPlate.SaveImage(fileName);
        cv::imwrite(fileName, matPlate);
    }
    static void SavePlateSample(Mat &matPlate, PlateCategory_t plateCategory,
                                string libPath, string shortFileNameNoExt) {
        DateTime now = DateTime::Now();
        // string fileName = string.Format(@"{0}\plates\{1}\{2}.jpg", libPath,
        //                                 plateCategory, shortFileNameNoExt);
        ostringstream buffer;
        buffer << libPath << "" DIRECTORY_DELIMITER"plates" DIRECTORY_DELIMITER"" << plateCategory << "" DIRECTORY_DELIMITER"" << shortFileNameNoExt << ".jpg";
        string fileName = buffer.str();
        // matPlate.SaveImage(fileName);
        cv::imwrite(fileName, matPlate);
    }

    // use vector to replace array
    static vector<float> ComputeHogDescriptors(Mat &image) {
        // Mat matToHog = image.Resize(HOGWinSize);
        Mat matToHog;
        cv::resize(image, matToHog, HOGWinSize);
        HOGDescriptor hog = HOGDescriptor(
            HOGWinSize, HOGBlockSize, HOGBlockStride, HOGCellSize, HOGNBits);
        // return hog.Compute(matToHog, new OpenCvSharp.Size(1, 1),
        //                    new OpenCvSharp.Size(0, 0));
        vector<float> ret;
        hog.compute(matToHog, ret, cv::Size(1, 1), cv::Size(0, 0));
        return ret;
    }
    static bool Train(Mat &samples, Mat &responses) {
        svm = SVM::create();
        svm->setType(SVM::Types::C_SVC);
        svm->setKernel(SVM::KernelTypes::LINEAR);
        svm->setTermCriteria(TermCriteria(TermCriteria::Type::MAX_ITER, 10000, 1e-10));
        IsReady = true;
        // return svm.Train(samples, SampleTypes.RowSample, responses);
        return svm->train(samples, SampleTypes::ROW_SAMPLE, responses);
    }
    static void Save(const string &fileName) {
        if (IsReady == false || svm == nullptr)
            return;
        // svm.Save(fileName);
        svm->save(fileName);
    }
    static void Load(const string &fileName) {
        try {
            // svm = SVM.Load(fileName);
            svm = SVM::load(fileName);
            IsReady = true;
        } catch (exception) {
            // throw new Exception("字符识别库加载异常，请检查存放路径");
            throw logic_error("字符识别库加载异常，请检查存放路径");
        }
    }
    static bool IsCorrectTrainngDirectory(const string &path) {
        bool isCorrect = true;
        // string[] plateCategoryNames = Enum.GetNames(typeof(PlateCategory));
        vector<const char*> plateCategoryNames(begin(PlateCategory_tToString), end(PlateCategory_tToString));

        for (int index = 0; index < plateCategoryNames.size(); index++) {
            string plateCategoryName = plateCategoryNames[index];
            // string platePath =
            //     string.Format(@"{0}\{1}", path, plateCategoryName);
            ostringstream buffer;
            buffer << path << "" DIRECTORY_DELIMITER"" << plateCategoryName;
            string platePath = buffer.str();

            if (Directory::Exists(platePath) == false) {
                isCorrect = false;
                break;
            }
        }
        return isCorrect;
    }
    static PlateCategory_t Test(Mat &matTest) {
        try {
            if (IsReady == false || svm == null) {
                // throw new Exception(
                //     "训练数据为空，请重新训练⻋牌类型识别或加载数据");
                throw logic_error("训练数据为空，请重新训练⻋牌类型识别或加载数据");
            }

            // TODO
            // 名字改为英文，对应的生成目录路径的过程也应该改
            PlateCategory_t result = PlateCategory_t::NonPlate;

            if (IsReady == false || svm == null)
                return result;
            // float[] descriptor = ComputeHogDescriptors(matTest);
            vector<float> descriptor = ComputeHogDescriptors(matTest);
            Mat testDescriptor =
                Mat::zeros(1, descriptor.size(), CV_32FC1);
            for (int index = 0; index < descriptor.size(); index++) {
                // testDescriptor.Set<float>(0, index, descriptor[index]);
                // TODO 不确定是不是对应的
                testDescriptor.at<float>(0, index) = descriptor[index];                                
            }
            float predict = svm->predict(testDescriptor);
            result = (PlateCategory_t)((int)predict);
            return result;
        } catch (exception &ex) {
            std::cerr << ex.what() << std::endl;
            throw ex;
        }
    }
    static PlateCategory_t Test(const string &fileName) {
        // Mat matTest = Cv2.ImRead(fileName, ImreadModes.Grayscale);
        Mat matTest = cv::imread(fileName, cv::ImreadModes::IMREAD_GRAYSCALE);
        return Test(matTest);
    }

    static bool PreparePlateTrainningDirectory(const string &path) {
        bool success = true;
        try {
            // string platesDirectory = path + @"\plates";
            string platesDirectory = path + "" DIRECTORY_DELIMITER"plates";
            if (Directory::Exists(platesDirectory) == false)
                Directory::CreateDirectory(platesDirectory);
            // string[] plateCategoryNames = Enum.GetNames(typeof(PlateCategory));
            vector<const char*> plateCategoryNames(begin(PlateCategory_tToString), end(PlateCategory_tToString));
            for (int index_PlateCategory = 0;
                 index_PlateCategory < plateCategoryNames.size();
                 index_PlateCategory++) {
                // string plateCategoryDirectory =
                //     string.Format(@"{0}\{1}", platesDirectory,
                //                   plateCategoryNames[index_PlateCategory]);
                ostringstream buffer;
                // TODO 文件分隔符要对不同的系统
                buffer << platesDirectory << "" DIRECTORY_DELIMITER"" << plateCategoryNames[index_PlateCategory];
                string plateCategoryDirectory = buffer.str();
                if (Directory::Exists(plateCategoryDirectory) == false)
                    Directory::CreateDirectory(plateCategoryDirectory);
            }
        } catch (exception) {
            success = false;
        }
        return success;
    }
};
bool PlateCategory_SVM::IsReady = false;
cv::Size PlateCategory_SVM::HOGWinSize = cv::Size(96, 32);
cv::Size PlateCategory_SVM::HOGBlockSize = cv::Size(16, 16);
cv::Size PlateCategory_SVM::HOGBlockStride = cv::Size(8, 8);
cv::Size PlateCategory_SVM::HOGCellSize = cv::Size(8, 8);
int PlateCategory_SVM::HOGNBits = 9;
Ptr<SVM> PlateCategory_SVM::svm = null;
Random PlateCategory_SVM::random = Random();

} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

#endif // !PLATECATEGORY_SVM_H
