#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using cv::imread;
using cv::Mat;
using cv::Point2i;
using cv::Rect;

#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;

#include "CharInfo.h"
#include "CharSegment_V3.h"
using namespace Doit::CV::PlateRecogn;

#include "debug.h"
void InitSvm() {
    try {
        PlateCategory_SVM::Load("CategorySVM.yaml");
        PlateChar_SVM::Load("CharSVM.yaml");
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(0);
    }
}

using DataItemType = tuple<Mat, string, Rect, PlateColor_t>;
DataItemType get_test_data(size_t seed) {
    vector<DataItemType> test_set = {
        {imread("../../bin/licenses/12867_2019-03-28-08-09-51-086918.jpg"),
         "粤A CH383", Rect(1030, 463, 1201 - 1030, 540 - 463),
         PlateColor_t::YellowPlate},
        {imread("../../bin/licenses/DE4661_2019-03-23-08-55-40-053804.jpg"),
         "粤A DE466", Rect(756, 384, 880 - 756, 440 - 384),
         PlateColor_t::YellowPlate},
        {imread("../../bin/licenses/AM5222_2019-03-23-11-23-57-072708.jpg"),
         "粤A M5222", Rect(Point2i{968, 332}, Point2i{1074, 380}),
         PlateColor_t::YellowPlate},
        {imread("../../bin/licenses/CG948_2019-03-23-10-43-23-076807.jpg"),
         "粤A CG948", Rect(Point2i{772, 144}, Point2i{841, 193}),
         PlateColor_t::YellowPlate},
        {imread("../../bin/licenses/60T17_61775.jpg"), "粤A DB797",
         Rect(Point2i{698, 406}, Point2i{818, 455}), PlateColor_t::YellowPlate},
        {imread("../../bin/platecharsamples/plates/普通车牌/"
                "2019-03-12-09-28-36-069855_颜色法_8.jpg"),
         "粤K WA305", Rect(Point2i{0, 0}, Point2i{77, 23}),
         PlateColor_t::BluePlate},
        {imread("../../bin/licenses/CG948_2019-03-23-10-43-23-076807.jpg"),
         "粤A CG948", Rect(Point2i{772, 144}, Point2i{841, 193}),
         PlateColor_t::YellowPlate},
    };

    size_t index = seed % test_set.size();

    return test_set[index];
}

void test_SplitePlateByGammaTransform() {
    auto data = get_test_data(0);
    Mat image = get<0>(data);
    string license = get<1>(data);
    Rect rectRoi = get<2>(data);

    image = image(rectRoi);
    DebugVisualize("Origin", image);
    PlateColor_t color = PlateColor_t::YellowPlate;
    auto charInfos = CharSegment_V3::SplitePlateByGammaTransform(image, color);

    cout << "Recognized rect count " << charInfos.size() << endl;
    for (auto charInfo : charInfos) {
        charInfo.PlateChar = PlateChar_t::NonChar;
        cout << charInfo.ToString() << charInfo.OriginalRect << endl;
        DebugVisualize("Rect", image(charInfo.OriginalRect));
    }

    cout << "Real license " << license << endl;
}

#include "PlateLocator_V3.h"
#include "PlateRecognition_V3.h"
void test_GetPlateInfo() {
    auto data = get_test_data(1);
    Mat image = get<0>(data);
    string license = get<1>(data);
    Rect rectRoi = get<2>(data);
    PlateColor_t color = get<3>(data);

    image = image(rectRoi);
    DebugVisualizeNotWait("Origin", image);
    Mat expImage = Utilities::GammaTransform(image, 0.4);
    // Mat expImage = Utilities::HistogramTransform(image);
    DebugVisualize("Exponential", expImage);

    PlateInfo plateInfo = {PlateCategory_t::NormalPlate,
                           rectRoi,
                           image,
                           {},
                           PlateLocateMethod_t::Color};
    PlateInfo recognizedPlateInfo = PlateRecognition_V3::GetPlateInfo(
        plateInfo, color, CharSplitMethod_t::Log);

    auto recognizedCharInfos = recognizedPlateInfo.CharInfos;
    cout << "Recognized rect count " << recognizedCharInfos.size() << endl;

    Mat rectedImage = image.clone();
    rectedImage = 0;
    for (auto charInfo : recognizedCharInfos) {
        // charInfo.PlateChar = PlateChar_t::NonChar;
        cout << charInfo.ToString() << charInfo.OriginalRect << endl;
        rectedImage(charInfo.OriginalRect) = image(charInfo.OriginalRect) + 0;
    }
    DebugVisualize("Rect", rectedImage);

    cout << "Real license " << license << endl;
}

int main(int argc, char const *argv[]) {
    InitSvm();
    // test_SplitePlateByGammaTransform();
    test_GetPlateInfo();
    return 0;
}
