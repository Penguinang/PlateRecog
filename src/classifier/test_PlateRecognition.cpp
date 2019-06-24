#include "CharInfo.h"
#include "CharSegment_V3.h"
#include "PlateLocator_V3.h"
#include "PlateRecognition_V3.h"
#include "PlateCategory_SVM.h"
#include "PlateChar_SVM.h"
using namespace Doit::CV::PlateRecogn;

#include "debug.h"

#include <opencv2/core.hpp>
using cv::imread;
using cv::Mat;

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;
#include <utility>
using std::pair;

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
vector<pair<Mat, string>> load_plates_data_from_directories(const string &name, int count = -1){
    auto allFiles = Directory::GetFiles(name);
    vector<pair<Mat, string>> ret = {};

    if(count == -1)
        count = allFiles.size();
    
    for(size_t i = 0; i < count; ++ i){
        string filePath = allFiles[i];
        size_t start = string::npos;
        if((start = filePath.find("粤")) != filePath.npos){
            size_t end = filePath.find('_', start);
            if(end == string::npos)
                continue;
            
            ret.push_back({imread(filePath), filePath.substr(start, end-start)});
        }
    }

    return ret;
}

pair<Mat, string> get_test_data(size_t seed) {
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

    static auto test_set = load_plates_data_from_directories("../../bin/plateSamples", 1000);

    size_t index = seed % test_set.size();

    return test_set[index];
}

void test_Recoginition() {
    int total_test = 1000;
    int correct_test = 0;

    for (size_t i = 0; i < total_test; i++) {
        pair<Mat, string> sample = get_test_data(i);
        Mat image = sample.first;
        string license = sample.second;
        DebugVisualizeNotWait("original", image);

        auto plateInfos = PlateRecognition_V3::Recognite(image);
        for (auto &plateInfo : plateInfos) {
            // cout << "Real: " << license << endl;
            // cout << "Recognized Plate: " << endl
            //      << plateInfo.ToString() << endl;
            if(license == plateInfo.ToString())
                ++ correct_test;

            Mat rectedImage = plateInfo.OriginalMat;
            for (auto &charinfo : plateInfo.CharInfos) {
                cv::rectangle(rectedImage, charinfo.OriginalRect, {0, 0, 255});
            }
            DebugVisualize("rectChars", rectedImage);
        }

    }
    cout << "accuracy: " << total_test << " / " << correct_test << " = " << float(correct_test) / total_test << endl;
}

void test_CharSplit() {}

int main(int argc, char const *argv[]) {
    InitSvm();
    test_Recoginition();
    return 0;
}
