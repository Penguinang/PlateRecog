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
// using namespace cv;
using cv::imread;
using cv::Mat;

#include <iostream>
// using namespace std;
using std::cout;
using std::endl;

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
    // Mat image =
    //     imread("../../bin/licenses/12867_2019-03-28-08-09-51-086918.jpg");
    // string license = "粤A CH383";

    // Mat image =
    //     imread("../../bin/licenses/AM5222_2019-03-23-11-23-57-072708.jpg");
    // string license = "粤A M5222";    

    // Mat image =
    //     imread("../../bin/licenses/DE4661_2019-03-23-08-55-40-053804.jpg");
    // string license = "粤A DE466";    

    // Mat image =
    //     imread("../../bin/licenses/CG948_2019-03-23-10-43-23-076807.jpg");
    // string license = "粤A CG948";    

    // Mat image =
    //     imread("../../bin/licenses/60T17_61775.jpg");
    // string license = "粤A DB797";    
    
    // Mat image =
    //     imread("../../bin/platecharsamples/plates/普通车牌/2019-03-12-09-28-36-069855_颜色法_8.jpg");
    // string license = "粤K WA305";    

    Mat image =
        imread("../../bin/licenses/CG948_2019-03-23-10-43-23-076807.jpg");
    string license = "粤A CG948";

    // DebugVisualize("original", image);
    auto plateInfos = PlateRecognition_V3::Recognite(image);
    for (auto &plateInfo : plateInfos) {
        cout << "Real: " << license << endl;
        cout << "Recognized Plate: " << endl << plateInfo.ToString() << endl;  

        Mat rectedImage = plateInfo.OriginalMat;  
        for(auto &charinfo : plateInfo.CharInfos){
            cv::rectangle(rectedImage, charinfo.OriginalRect, {0, 0, 255});
        }
        DebugVisualize("rectChars", rectedImage);
    }
}

void test_CharSplit(){
    
}

int main(int argc, char const *argv[]) {
    InitSvm();
    test_Recoginition();
    return 0;
}
