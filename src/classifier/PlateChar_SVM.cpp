#include "CharInfo.h"
#include "PlateChar_SVM.h"

using namespace Doit::CV::PlateRecogn;

bool PlateChar_SVM::IsReady = false;
cv::Size PlateChar_SVM::HOGWinSize = cv::Size(16, 32);
cv::Size PlateChar_SVM::HOGBlockSize = cv::Size(16, 16);
cv::Size PlateChar_SVM::HOGBlockStride = cv::Size(8, 8);
cv::Size PlateChar_SVM::HOGCellSize = cv::Size(8, 8);
int PlateChar_SVM::HOGNBits = 9;
Ptr<SVM> PlateChar_SVM::svm = nullptr;
Random PlateChar_SVM::random = Random();

vector<float> PlateChar_SVM::ComputeHogDescriptors(Mat &image) {
    Mat matToHog;
    cv::resize(image, matToHog, HOGWinSize);
    HOGDescriptor hog = HOGDescriptor(HOGWinSize, HOGBlockSize, HOGBlockStride,
                                      HOGCellSize, HOGNBits);
    vector<float> ret;
    hog.compute(matToHog, ret, cv::Size(1, 1), cv::Size(0, 0));
    return ret;
}
bool PlateChar_SVM::PlateChar_SVM::Train(Mat &samples, Mat &responses,
                                         SVM::KernelTypes kernel, float C,
                                         float gamma, unsigned long IterCount,
                                         long double epsilon) {
    svm = SVM::create();
    svm->setType(SVM::Types::C_SVC);
    svm->setKernel(kernel);
    svm->setC(C);
    svm->setGamma(gamma);

    svm->setTermCriteria(
        TermCriteria(TermCriteria::Type::MAX_ITER, IterCount, epsilon));

    IsReady = true;
    return svm->train(samples, SampleTypes::ROW_SAMPLE, responses);
}
void PlateChar_SVM::Save(const string &fileName) {
    if (IsReady == false || svm == nullptr)
        return;
    svm->save(fileName);
}
void PlateChar_SVM::Load(const string &fileName) {
    try {
        svm = SVM::load(fileName);
        IsReady = true;
    } catch (exception &) {
        throw logic_error("字符识别库加载异常，请检查存放路径");
    }
}
bool PlateChar_SVM::IsCorrectTrainngDirectory(const string &path) {
    bool isCorrect = true;
    vector<const char *> plateCharNames(begin(PlateChar_tToString),
                                        end(PlateChar_tToString));

    for (size_t index = 0; index < plateCharNames.size(); index++) {
        string plateChar = plateCharNames[index];
        ostringstream buffer;
        buffer << path << "" DIRECTORY_DELIMITER "" << plateChar;
        string charDirectory = buffer.str();
        isCorrect = Directory::Exists(charDirectory);
        if (isCorrect == false)
            break;
    }

    return isCorrect;
}
PlateChar_t PlateChar_SVM::Test(Mat &matTest) {
    if (IsReady == false || svm == null) {
        throw logic_error("training data is null, please retrain plate type "
                          "recognition or load data");
    }
    PlateChar_t result = PlateChar_t::NonChar;

    vector<float> descriptor = ComputeHogDescriptors(matTest);
    Mat testDescriptor = Mat::zeros(1, descriptor.size(), CV_32FC1);
    for (size_t index = 0; index < descriptor.size(); index++) {
        testDescriptor.at<float>(0, index) = descriptor[index];
    }
    float predict = svm->predict(testDescriptor);
    result = (PlateChar_t)((int)predict);
    return result;
}
PlateChar_t PlateChar_SVM::Test(const string &fileName) {
    Mat matTest = cv::imread(fileName, cv::ImreadModes::IMREAD_GRAYSCALE);
    return Test(matTest);
}
void PlateChar_SVM::SaveCharSample(CharInfo &charInfo, const string &libPath) {
    DateTime now = DateTime::Now();
    ostringstream buffer;
    buffer << now.Year << "-" << std::setfill('0') << std::setw(2) << now.Month
           << std::setfill('0') << std::setw(2) << "-" << std::setfill('0')
           << std::setw(2) << now.Day << "-" << std::setfill('0')
           << std::setw(2) << now.Hour << std::setfill('0') << std::setw(2)
           << "-" << std::setfill('0') << std::setw(2) << now.Minute << "-"
           << std::setfill('0') << std::setw(2) << now.Second << "-"
           << std::setfill('0') << std::setw(6) << random.Next(100000);
    string time = buffer.str();
    buffer.clear();

    buffer << charInfo.PlateLocateMethod << "_" << charInfo.CharSplitMethod
           << "_" << time;
    string shortFileNameNoExt = buffer.str();

    SaveCharSample(charInfo.OriginalMat, charInfo.PlateChar, libPath,
                   shortFileNameNoExt);
}
void PlateChar_SVM::SaveCharSample(Mat &charMat, PlateChar_t plateChar,
                                   const string &libPath,
                                   const string &shortFileNameNoExt) {
    ostringstream buffer;
    buffer << libPath << "" DIRECTORY_DELIMITER "chars" DIRECTORY_DELIMITER ""
           << plateChar << "" DIRECTORY_DELIMITER "" << shortFileNameNoExt
           << ".jpg";
    string fileName = buffer.str();
    cv::resize(charMat.clone(), charMat, HOGWinSize);
    cv::imwrite(fileName, charMat);
}
void PlateChar_SVM::SaveCharSample(Mat &charMat, PlateChar_t plateChar,
                                   const string &libPath) {
    CharInfo charInfo = CharInfo();
    charInfo.OriginalMat = charMat;
    charInfo.PlateChar = plateChar;
    SaveCharSample(charInfo, libPath);
}
bool PlateChar_SVM::PrepareCharTrainningDirectory(const string &path) {
    bool success = true;
    try {
        success = Directory::Exists(path);
        string charsDirectory = path + "" DIRECTORY_DELIMITER "chars";

        if (Directory::Exists(charsDirectory) == false)
            Directory::CreateDirectory(charsDirectory);
        vector<const char *> plateCharNames(begin(PlateChar_tToString),
                                            end(PlateChar_tToString));
        for (size_t index_PlateChar = 0;
             index_PlateChar < plateCharNames.size(); index_PlateChar++) {
            string plateChar = plateCharNames[index_PlateChar];
            ostringstream buffer;
            buffer << charsDirectory << "" DIRECTORY_DELIMITER "" << plateChar;
            string plateCharDirectory = buffer.str();
            if (Directory::Exists(plateCharDirectory) == false)
                Directory::CreateDirectory(plateCharDirectory);
        }
    } catch (exception &) {
        success = false;
    }
    return success;
}
