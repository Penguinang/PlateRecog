// #include "PlateRecognition_V3.cpp"

// // Stub
// class PlateLocator_V3{
// public:
//     static vector<Doit::CV::PlateRecogn::PlateInfo> LocatePlates(Mat &);
// };
// class CharSegment_V3{
// public:
//     static vector<Doit::CV::PlateRecogn::CharInfo>
//     SplitePlateByGammaTransform(Mat, Doit::CV::PlateRecogn::PlateColor_t);
//     static vector<Doit::CV::PlateRecogn::CharInfo>
//     SplitePlateByIndexTransform(Mat, Doit::CV::PlateRecogn::PlateColor_t);
//     static vector<Doit::CV::PlateRecogn::CharInfo>
//     SplitePlateByLogTransform(Mat, Doit::CV::PlateRecogn::PlateColor_t);
//     static vector<Doit::CV::PlateRecogn::CharInfo> SplitePlateByOriginal(Mat,
//     Mat, Doit::CV::PlateRecogn::PlateColor_t);
// };

// using namespace Doit::CV::PlateRecogn;
// void test_GetPlateInfo(){
//     const char *file1 = "licenses/12867_2019-03-28-08-09-51-086918.jpg";
//     Mat image1 = cv::imread(file1);
//     Rect rectROI = Rect(1000, 450, 1220-1000, 560-450);
//     Mat matROI = image1(rectROI).clone();

//     PlateInfo plateInfo;
//     plateInfo.OriginalRect = rectROI;
//     plateInfo.OriginalMat = matROI;
//     plateInfo.PlateCategory = PlateCategory_t::NormalPlate;
//     plateInfo.PlateLocateMethod = PlateLocateMethod_t::Color;

//     PlateColor_t plateColor = PlateColor_t::BluePlate;
//     PlateRecognition_V3::GetPlateInfo (plateInfo, plateColor,
//     CharSplitMethod_t::Origin); PlateRecognition_V3::GetPlateInfo (plateInfo,
//     plateColor, CharSplitMethod_t::Gamma); PlateRecognition_V3::GetPlateInfo
//     (plateInfo, plateColor, CharSplitMethod_t::Exponential);
//     PlateRecognition_V3::GetPlateInfo (plateInfo, plateColor,
//     CharSplitMethod_t::Log);

// }
#include "CharSegment_V3.h"
#include "PlateLocator_V3.h"
#include "PlateRecognition_V3.h"
using namespace Doit::CV::PlateRecogn;

#include <opencv2/core.hpp>
using namespace cv;

#include <iostream>
using namespace std;

void InitSvm() {
    try {
        PlateCategory_SVM::Load("CategorySVM.yaml");
        PlateChar_SVM::Load("CharSVM.yaml");
    } catch(exception &e){
        cerr << e.what() << endl;
        exit(0);
    }
}

void test_Recoginition() {
    Mat image =
        imread("../../bin/licenses/12867_2019-03-28-08-09-51-086918.jpg");
    string license = "粤A CH383";
    auto plateInfos = PlateRecognition_V3::Recognite(image);
    for (auto &plateInfo : plateInfos) {
        cout << "Label: " << license << endl;
        cout << "Recognized Plate: " << endl << plateInfo.ToString() << endl;
    }
}

int main(int argc, char const *argv[]) {
    InitSvm();
    test_Recoginition();
    return 0;
}
