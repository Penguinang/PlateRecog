#ifndef PLATECHAR_SVM_H
#define PLATECHAR_SVM_H


#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>

// using cv::Mat;
using cv::Ptr;
using cv::ml::SVM;
using cv::HOGDescriptor;
using cv::TermCriteria;
using cv::ml::SampleTypes;

// #include <random>
// #include <ctime>
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
        // string[] plateCharNames = Enum.GetNames(typeof(PlateChar));
        vector<const char*> plateCharNames(begin(PlateChar_tToString), end(PlateChar_tToString));

        for (int index = 0; index < plateCharNames.size(); index++) {
            // string plateChar = plateCharNames[index];
            string plateChar = plateCharNames[index];

            // string charDirectory = string.Format(@"{0}\{1}", path, plateChar);
            ostringstream buffer;
            buffer << path << "" DIRECTORY_DELIMITER"" << plateChar;
            string charDirectory = buffer.str();
            isCorrect = Directory::Exists(charDirectory);
            if (isCorrect == false)
                break;
        }

        return isCorrect;
    }
    static PlateChar_t Test(Mat &matTest) {
        // try
        //{
        if (IsReady == false || svm == null) {
            // throw new Exception("训练数据为空，请重新训练字符或加载数据");
            throw logic_error("训练数据为空，请重新训练⻋牌类型识别或加载数据");
        }
        // TODO
        // 名字改为英文，对应的生成目录路径的过程也应该改
        PlateChar_t result = PlateChar_t::NonChar;

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
        result = (PlateChar_t)((int)predict);
        return result;
        //}
        // catch (Exception ex)
        //{
        // throw ex;
        //}
    }
    static PlateChar_t Test(const string &fileName) {
        // Mat matTest = Cv2.ImRead(fileName, ImreadModes.Grayscale);
        Mat matTest = cv::imread(fileName, cv::ImreadModes::IMREAD_GRAYSCALE);
        return Test(matTest);
    }
    static void SaveCharSample(CharInfo &charInfo, const string &libPath) {
        DateTime now = DateTime::Now();
        // string time =
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
        string time = buffer.str();
        buffer.clear();

        // string shortFileNameNoExt =
        //     string.Format("{0}_{1}_{2}", charInfo.PlateLocateMethod,
        //                   charInfo.CharSplitMethod, time);
        buffer << charInfo.PlateLocateMethod << "_" << charInfo.CharSplitMethod << "_" << time;
        string shortFileNameNoExt = buffer.str();

        SaveCharSample(charInfo.OriginalMat, charInfo.PlateChar, libPath,
                       shortFileNameNoExt);
    }
    static void SaveCharSample(Mat &charMat, PlateChar_t plateChar, const string &libPath,
                               const string &shortFileNameNoExt) {
        // string fileName = string.Format(@"{0}\chars\{1}\{2}.jpg", libPath,
        //                                 plateChar, shortFileNameNoExt);
        ostringstream buffer;
        buffer << libPath << "" DIRECTORY_DELIMITER"chars" DIRECTORY_DELIMITER"" << plateChar << "" DIRECTORY_DELIMITER"" << shortFileNameNoExt << ".jpg";
        string fileName = buffer.str();
        // charMat = charMat.Resize(HOGWinSize);
        cv::resize(charMat.clone(), charMat, HOGWinSize);
        // charMat.SaveImage(fileName);
        cv::imwrite(fileName, charMat);
    }
    static void SaveCharSample(Mat &charMat, PlateChar_t plateChar,
                               const string &libPath) {
        CharInfo charInfo = CharInfo();
        charInfo.OriginalMat = charMat;
        charInfo.PlateChar = plateChar;
        SaveCharSample(charInfo, libPath);
    }
    static bool PrepareCharTrainningDirectory(const string &path) {
        bool success = true;
        try {
            success = Directory::Exists(path);
            // string charsDirectory = path + @"\chars";
            string charsDirectory = path + "" DIRECTORY_DELIMITER"chars";

            if (Directory::Exists(charsDirectory) == false)
                Directory::CreateDirectory(charsDirectory);
            // string[] plateCharNames = Enum.GetNames(typeof(PlateChar));
            vector<const char*> plateCharNames(begin(PlateChar_tToString), end(PlateChar_tToString));
            for (int index_PlateChar = 0;
                 index_PlateChar < plateCharNames.size(); index_PlateChar++) {
                string plateChar = plateCharNames[index_PlateChar];
                // string plateCharDirectory = charsDirectory + @"" DIRECTORY_DELIMITER"" + plateChar;
                ostringstream buffer;
                // TODO 文件分隔符要对不同的系统
                buffer << charsDirectory << "" DIRECTORY_DELIMITER"" << plateChar;
                string plateCharDirectory = buffer.str();
                if (Directory::Exists(plateCharDirectory) == false)
                    Directory::CreateDirectory(plateCharDirectory);
            }
        } catch (exception) {
            success = false;
        }
        return success;
    }
};

bool PlateChar_SVM::IsReady = false;
cv::Size PlateChar_SVM::HOGWinSize = cv::Size(16, 32);
cv::Size PlateChar_SVM::HOGBlockSize = cv::Size(16, 16);
cv::Size PlateChar_SVM::HOGBlockStride = cv::Size(8, 8);
cv::Size PlateChar_SVM::HOGCellSize = cv::Size(8, 8);
int PlateChar_SVM::HOGNBits = 9;
Ptr<SVM> PlateChar_SVM::svm = nullptr;
Random PlateChar_SVM::random = Random();
} // namespace PlateRecogn
} // namespace CV
} // namespace Doit

#endif // !PLATECHAR_SVM_H
