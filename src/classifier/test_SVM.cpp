#include "PlateChar_SVM.h"
#include "PlateCategory_SVM.h"

#include <iostream>
using namespace std;
using namespace cv;

#include <cassert>

using namespace Doit::CV::PlateRecogn;
void test_charinfo(){
    CharInfo A = CharInfo(PlateChar_t::NonChar, Mat(), Rect(1, 2, 3, 4), PlateLocateMethod_t::Color, CharSplitMethod_t::Exponential);
    cout << A.ToString() << endl;
    cout << A.Info() << endl;

    CharInfo B = CharInfo(PlateChar_t::NonChar, Mat(), Rect(1, 2, 4, 4), PlateLocateMethod_t::Color, CharSplitMethod_t::Exponential);
    cout << B.ToString() << endl;
    cout << B.Info() << endl;    
}

void test_plateinfo(){
    CharInfo CharA = CharInfo(PlateChar_t::A, Mat(), Rect(1, 2, 3, 4), PlateLocateMethod_t::Color, CharSplitMethod_t::Exponential);
    CharInfo Char1 = CharInfo(PlateChar_t::_1, Mat(), Rect(1, 2, 3, 4), PlateLocateMethod_t::Color, CharSplitMethod_t::Exponential);
    PlateInfo A = PlateInfo(PlateCategory_t::AlternativeEnergyPlate, Rect(1, 2, 3, 4), Mat(), {CharA, Char1}, PlateLocateMethod_t::Color);

    cout << A.Info() << endl;
}

void test_Char_SVM(){
    PlateChar_SVM classifier;

    string charsPath = "platecharsamples/chars";
    vector<string> allImageCategories = Directory::GetFiles(charsPath);
    vector<vector<float>> Hogs;
    vector<int> Tags;
    vector<string> imageNames;

    for(auto &imageCategory : allImageCategories){
        vector<string> allImages = Directory::GetFiles(imageCategory);
        string tagStr = imageCategory.substr(charsPath.size()+1);
        auto tagIter = find(begin(PlateChar_tToString), end(PlateChar_tToString), tagStr);
        assert(tagIter != end(PlateChar_tToString));
        int tagInt = tagIter - begin(PlateChar_tToString);
        for(auto &imageFile : allImages){
            Mat image = cv::imread(imageFile);
            vector<float> Hog = classifier.ComputeHogDescriptors(image);
            Hogs.push_back(Hog);
            Tags.push_back(tagInt);            
            imageNames.push_back(imageFile);
        }
    }

    int sampleCount = Hogs.size();
    int trainingCount = sampleCount * 0.9, validationCount = sampleCount - trainingCount;
    // int trainingCount = 1000, validationCount = 400;
    vector<size_t> randomIndices(sampleCount);
    iota(randomIndices.begin(), randomIndices.end(), 0);
    random_shuffle(randomIndices.begin(), randomIndices.end());

    int hogSize = Hogs[0].size();
    int sizes[] = {trainingCount, hogSize};
    Mat training_data(trainingCount, hogSize, CV_32FC1, Scalar::all(0));
    Mat training_tag(trainingCount, 1, CV_32S);
    for(int i = 0; i < trainingCount; ++ i){
        // training_data.row(i) = Mat(1, hogSize, CV_32FC1, Hogs[randomIndices[i]].data());
        Mat(1, hogSize, CV_32FC1, Hogs[randomIndices[i]].data()).copyTo(training_data.row(i));
        training_tag.at<int>(i) = Tags[randomIndices[i]];
        assert(abs(training_data.at<float>(i, 0)-Hogs[randomIndices[i]][0]) < 1e-3);
        assert(abs(training_data.at<float>(i, 10)-Hogs[randomIndices[i]][10]) < 1e-3);
        assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
        assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
    }

    if(!Directory::Exists("CharSVM.yaml")){
        cout << "Traininig ..." << endl;
        classifier.Train(training_data, training_tag);
        classifier.Save("CharSVM.yaml");
    } else{
        cout << "Loading trained model ..." << endl;
        classifier.Load("CharSVM.yaml");
    }

    vector<int> validationPrediction = {};
    int trueCount = 0;
    for(int i = trainingCount; i < trainingCount+validationCount; ++ i){
        Mat image = cv::imread(imageNames[randomIndices[i]]);
        int prediction = static_cast<int>(classifier.Test(image));
        validationPrediction.push_back(prediction);
        if(prediction == Tags[randomIndices[i]]){
            ++ trueCount;
        }
        cout << "\rValidating ... " << setiosflags(ios::fixed) << setw(2) << setprecision(1) << float(100 * (i-trainingCount)) / validationCount << "%";
    }
    cout << endl;
    cout << "validation accuracy: " << trueCount << " / " << validationCount << " = " << setprecision(6) << float(trueCount) / validationCount;
}

void test_Category_SVM(){
    PlateCategory_SVM classifier;

    string imagesPath = "platecharsamples/plates";
    vector<string> allImageCategories = Directory::GetFiles(imagesPath);
    vector<vector<float>> Hogs;
    vector<int> Tags;
    vector<string> imageNames;    

    for(auto &imageCategory : allImageCategories){
        vector<string> allImages = Directory::GetFiles(imageCategory);
        string tagStr = imageCategory.substr(imagesPath.size()+1);
        auto tagIter = find(begin(PlateCategory_tToString), end(PlateCategory_tToString), tagStr);
        assert(tagIter != end(PlateCategory_tToString));
        int tagInt = tagIter - begin(PlateCategory_tToString);
        for(auto &imageFile : allImages){
            Mat image = cv::imread(imageFile);
            vector<float> Hog = classifier.ComputeHogDescriptors(image);
            Hogs.push_back(Hog);
            Tags.push_back(tagInt);            
            imageNames.push_back(imageFile);
        }
    }

    int sampleCount = Hogs.size();
    int trainingCount = sampleCount * 0.9, validationCount = sampleCount - trainingCount;
    // int trainingCount = 1000, validationCount = 400;
    vector<size_t> randomIndices(sampleCount);
    iota(randomIndices.begin(), randomIndices.end(), 0);
    random_shuffle(randomIndices.begin(), randomIndices.end());

    int hogSize = Hogs[0].size();
    int sizes[] = {trainingCount, hogSize};
    Mat training_data(trainingCount, hogSize, CV_32FC1, Scalar::all(0));
    Mat training_tag(trainingCount, 1, CV_32S);
    for(int i = 0; i < trainingCount; ++ i){
        Mat(1, hogSize, CV_32FC1, Hogs[randomIndices[i]].data()).copyTo(training_data.row(i));
        training_tag.at<int>(i) = Tags[randomIndices[i]];
        assert(abs(training_data.at<float>(i, 0)-Hogs[randomIndices[i]][0]) < 1e-3);
        assert(abs(training_data.at<float>(i, 10)-Hogs[randomIndices[i]][10]) < 1e-3);
        assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
        assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
    }

    if(!Directory::Exists("CategorySVM.yaml")){
        cout << "Traininig ..." << endl;
        classifier.Train(training_data, training_tag);
        classifier.Save("CategorySVM.yaml");
    } else{
        cout << "Loading trained model ..." << endl;
        classifier.Load("CategorySVM.yaml");
    }

    vector<int> validationPrediction = {};
    int trueCount = 0;
    for(int i = trainingCount; i < trainingCount+validationCount; ++ i){
        Mat image = cv::imread(imageNames[randomIndices[i]]);
        int prediction = static_cast<int>(classifier.Test(image));
        validationPrediction.push_back(prediction);
        if(prediction == Tags[randomIndices[i]]){
            ++ trueCount;
        }
        cout << "\rValidating ... " << setiosflags(ios::fixed) << setw(2) << setprecision(1) << float(100 * (i-trainingCount)) / validationCount << "%";
    }
    cout << endl;
    cout << "validation accuracy: " << trueCount << " / " << validationCount << " = " << setprecision(6) << float(trueCount) / validationCount;
}

int main(int argc, char const *argv[])
{
    // test_charinfo();
    // test_plateinfo();
    // test_Char_SVM();
    test_Category_SVM();
    
    return 0;
}