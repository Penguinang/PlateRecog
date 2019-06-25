#include "CharInfo.h"
#include "CharSegment_V3.h"
#include "PlateCategory_SVM.h"
#include "PlateChar_SVM.h"
#include "PlateLocator_V3.h"
#include "PlateRecognition_V3.h"
using namespace Doit::CV::PlateRecogn;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
using cv::imread;
using cv::imwrite;
using cv::Mat;
using cv::Point2i;
using cv::Size;
using cv::resize;

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <utility>
using std::get;
using std::pair;
using std::tuple;
#include <vector>
using std::vector;

Size gridSize = {200, 100};
vector<Mat> outImages;
#include "debug.h"
Mat concatenteImags(vector<Mat> &images, int colCount = 5){
    int count = images.size();
    int rows = count / colCount + (count % colCount != 0 ? 1 : 0);
    Mat out(vector<int>{gridSize.height * rows, gridSize.width * colCount}, CV_8UC3);
    out = 0;
    auto it = images.begin();
    for(int r = 0; r < rows; ++ r){
        for(int c = 0; c < colCount; ++ c){
            if(it == images.end())
                break;
            out(Rect({gridSize.width*c, gridSize.height*r}, gridSize)) = *it + 0;
            ++ it;
        }
    }
    return out;
}

void InitSvm() {
    try {
        PlateCategory_SVM::Load("CategorySVM.yaml");
        PlateChar_SVM::Load("CharSVM.yaml");
    } catch (exception &e) {
        cerr << e.what() << endl;
        exit(0);
    }
}

// 加载数据，count=-1 代表全部
// @return: image, label, path
vector<tuple<Mat, string, string>>
load_plates_data_from_directories(const string &name, int count = -1) {
    auto allFiles = Directory::GetFiles(name);
    vector<tuple<Mat, string, string>> ret = {};

    if (count == -1)
        count = allFiles.size();
    size_t real_count = count;

    for (size_t i = 0; i < real_count; ++i) {
        string filePath = allFiles[i];
        size_t start = string::npos;
        if ((start = filePath.find("粤")) != filePath.npos) {
            size_t end = filePath.find('_', start);
            if (end == string::npos)
                continue;

            ret.push_back({imread(filePath),
                           filePath.substr(start, end - start), filePath});
        }
    }

    return ret;
}

static auto test_set =
    load_plates_data_from_directories("../../bin/plateSamples", 1000);

tuple<Mat, string, string> get_test_data(size_t seed) {
    // vector<pair<Mat, string>> test_set = {
    //     {imread("../../bin/licenses/60T17_61775.jpg"), "粤A DB797"},
    //     {imread("../../bin/licenses/12867_2019-03-28-08-09-51-086918.jpg"),
    //      "粤A CH383"},
    //     {imread("../../bin/licenses/AM5222_2019-03-23-11-23-57-072708.jpg"),
    //      "粤A M5222"},
    //     {imread("../../bin/licenses/DE4661_2019-03-23-08-55-40-053804.jpg"),
    //      "粤A DE466"},
    //     {imread("../../bin/licenses/CG948_2019-03-23-10-43-23-076807.jpg"),
    //      "粤A CG948"},
    //     {imread("../../bin/platecharsamples/plates/普通车牌/"
    //             "2019-03-12-09-28-36-069855_颜色法_8.jpg"),
    //      "粤K WA305"},
    //     {imread("../../bin/licenses/CG948_2019-03-23-10-43-23-076807.jpg"),
    //      "粤A CG948"},
    // };

    size_t index = seed % test_set.size();

    return test_set[index];
}

void test_Recoginition() {
    size_t total_test = 100;
    size_t correct_test = 0;

    Directory::CreateDirectory("../../bin/wrong");

    for (size_t i = 0; i < total_test; i++) {
        auto sample = get_test_data(i);
        Mat image = get<0>(sample);
        string license = get<1>(sample);
        string filePath = get<2>(sample);
        string fileName = filePath.substr(filePath.find_last_of(DIRECTORY_DELIMITER) + 1);

        auto plateInfos = PlateRecognition_V3::Recognite(image);
        for (auto &plateInfo : plateInfos) {
            if (license == plateInfo.ToString()) {
                ++correct_test;
            } else {
                // cout << "Recognized Plate: " << endl
                //     << plateInfo.ToString() << endl;
                // cout << "Real: " << license << endl;
                // DebugVisualize("original", image);
                // imwrite("../../bin/wrong/" + fileName + "-" + license + "_" +
                // plateInfo.ToString() + ".jpg", image);
                DebugVisualize("origin", image);
                imwrite("../../bin/wrong/" + fileName + "-" + license + "_" +
                            plateInfo.ToString() + ".png",
                        concatenteImags(outImages));
            }

            // Mat rectedImage = plateInfo.OriginalMat;
            // for (auto &charinfo : plateInfo.CharInfos) {
            //     cv::rectangle(rectedImage, charinfo.OriginalRect, {0, 0,
            //     255});
            // }
            // DebugVisualize("rectChars", rectedImage);
        }
        if (plateInfos.size() == 0) {
            // cout << "Recognized Plate: null" << endl;
            // cout << "Real: " << license << endl;
            // DebugVisualize("original", image);
            // imwrite("../../bin/wrong/" + fileName + "-" + license + "_null" +
            //             ".jpg",
            //         image);
            DebugVisualize("origin", image);
            imwrite("../../bin/wrong/" + fileName + "-" + license + "_null" + ".png",
                    concatenteImags(outImages));
        }
        outImages.clear();
    }
    cout << "accuracy: " << correct_test << " / " << total_test << " = "
         << float(correct_test) / total_test << endl;
}

void singleImage_getPlateInfo() {
    tuple<Mat, string, string> sample = {
        imread(
            "../../bin/plateSamples/粤A1KE07_2019-03-20-09-26-31-044685.jpg"),
        "粤A 1kE407",
        "../../bin/plateSamples/粤A1KE07_2019-03-20-09-26-31-044685.jpg"};
    Mat image = get<0>(sample);
    string license = get<1>(sample);
    string filePath = get<2>(sample);
    string fileName = filePath.substr(filePath.find_last_of("/") + 1);

    auto plateInfos = PlateRecognition_V3::Recognite(image);
    // auto plateInfos = PlateRecognition_V3::GetPlateInfo()
    for (auto &plateInfo : plateInfos) {
        cout << "Real: " << license << endl;
        cout << "Recognized Plate: " << endl << plateInfo.ToString() << endl;
        if (license != plateInfo.ToString()) {
            // cout << "Recognized Plate: " << endl
            //     << plateInfo.ToString() << endl;
            // cout << "Real: " << license << endl;
            // DebugVisualize("original", image);
        }

        // Mat rectedImage = plateInfo.OriginalMat;
        // for (auto &charinfo : plateInfo.CharInfos) {
        //     cv::rectangle(rectedImage, charinfo.OriginalRect, {0, 0, 255});
        // }
        // DebugVisualize("rectChars", rectedImage);
    }
    if (plateInfos.size() == 0) {
        // cout << "Recognized Plate: null" << endl;
        // cout << "Real: " << license << endl;
        // DebugVisualize("original", image);
    }
}

void view_Image(int index) {
    auto sample = get_test_data(index);
    Mat image = get<0>(sample);
    string license = get<1>(sample);
    string filePath = get<2>(sample);
    DebugVisualize("origin", image);
    auto plateInfos = PlateRecognition_V3::Recognite(image);
    for (auto &plateInfo : plateInfos) {
        cout << plateInfo.Info() << endl;
    }

    // auto data = test_set[index];
    // DebugVisualize("origin", get<0>(data));
    // cout << get<1>(data) << endl;
    // cout << get<2>(data) << endl;
}

void test_CharSplit() {}

int main(int argc, char const *argv[]) {
    InitSvm();
    test_Recoginition();
    // singleImage_getPlateInfo();
    // view_Image(1);
    return 0;
}
