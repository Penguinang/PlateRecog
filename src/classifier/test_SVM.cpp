#include "CharInfo.h"
#include "PlateCategory_SVM.h"
#include "PlateChar_SVM.h"
using cv::Mat;
using cv::Rect;
using cv::Scalar;

#include <iostream>
#include <numeric>
using std::cout;
using std::endl;
using std::iota;
using std::pair;
using std::random_shuffle;
using std::setprecision;
using std::setw;
using std::string;
using std::vector;

#include <cassert>

using namespace Doit::CV::PlateRecogn;
void test_charinfo() {
    CharInfo A =
        CharInfo(PlateChar_t::NonChar, Mat(), Rect(1, 2, 3, 4),
                 PlateLocateMethod_t::Color, CharSplitMethod_t::Exponential);
    cout << A.ToString() << endl;
    cout << A.Info() << endl;

    CharInfo B =
        CharInfo(PlateChar_t::NonChar, Mat(), Rect(1, 2, 4, 4),
                 PlateLocateMethod_t::Color, CharSplitMethod_t::Exponential);
    cout << B.ToString() << endl;
    cout << B.Info() << endl;
}

void test_plateinfo() {
    CharInfo CharA =
        CharInfo(PlateChar_t::A, Mat(), Rect(1, 2, 3, 4),
                 PlateLocateMethod_t::Color, CharSplitMethod_t::Exponential);
    CharInfo Char1 =
        CharInfo(PlateChar_t::_1, Mat(), Rect(1, 2, 3, 4),
                 PlateLocateMethod_t::Color, CharSplitMethod_t::Exponential);
    PlateInfo A =
        PlateInfo(PlateCategory_t::AlternativeEnergyPlate, Rect(1, 2, 3, 4),
                  Mat(), {CharA, Char1}, PlateLocateMethod_t::Color);

    cout << A.Info() << endl;
}

void test_Char_SVM() {
    PlateChar_SVM classifier;

    string charsPath = "../../bin/platecharsamples/chars";
    vector<string> allImageCategories = Directory::GetFiles(charsPath);
    vector<vector<float>> Hogs;
    vector<int> Tags;
    vector<string> imageNames;

    for (auto &imageCategory : allImageCategories) {
        vector<string> allImages = Directory::GetFiles(imageCategory);
        string tagStr = imageCategory.substr(charsPath.size() + 1);
        auto tagIter =
            find(begin(PlateChar_tToString), end(PlateChar_tToString), tagStr);
        assert(tagIter != end(PlateChar_tToString));
        int tagInt = tagIter - begin(PlateChar_tToString);
        for (auto &imageFile : allImages) {
            Mat image = cv::imread(imageFile);
            vector<float> Hog = classifier.ComputeHogDescriptors(image);
            Hogs.push_back(Hog);
            Tags.push_back(tagInt);
            imageNames.push_back(imageFile);
        }
    }

    int sampleCount = Hogs.size();
    int trainingCount = sampleCount * 0.9,
        validationCount = sampleCount - trainingCount;
    vector<size_t> randomIndices(sampleCount);
    iota(randomIndices.begin(), randomIndices.end(), 0);
    random_shuffle(randomIndices.begin(), randomIndices.end());

    int hogSize = Hogs[0].size();
    Mat training_data(trainingCount, hogSize, CV_32FC1, Scalar::all(0));
    Mat training_tag(trainingCount, 1, CV_32S);
    for (int i = 0; i < trainingCount; ++i) {
        Mat(1, hogSize, CV_32FC1, Hogs[randomIndices[i]].data())
            .copyTo(training_data.row(i));
        training_tag.at<int>(i) = Tags[randomIndices[i]];
        assert(abs(training_data.at<float>(i, 0) - Hogs[randomIndices[i]][0]) <
               1e-3);
        assert(abs(training_data.at<float>(i, 10) -
                   Hogs[randomIndices[i]][10]) < 1e-3);
        assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
        assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
    }

    if (!Directory::Exists("CharSVM.yaml")) {
        cout << "Traininig ..." << endl;
        classifier.Train(training_data, training_tag);
        classifier.Save("CharSVM.yaml");
    } else {
        cout << "Loading trained model ..." << endl;
        classifier.Load("CharSVM.yaml");
    }

    vector<int> validationPrediction = {};
    int trueCount = 0;
    for (int i = trainingCount; i < trainingCount + validationCount; ++i) {
        Mat image = cv::imread(imageNames[randomIndices[i]]);
        int prediction = static_cast<int>(classifier.Test(image));
        validationPrediction.push_back(prediction);
        if (prediction == Tags[randomIndices[i]]) {
            ++trueCount;
        }
        cout << "\rValidating ... " << setiosflags(ios::fixed) << setw(2)
             << setprecision(1)
             << float(100 * (i - trainingCount)) / validationCount << "%";
    }
    cout << endl;
    cout << "validation accuracy: " << trueCount << " / " << validationCount
         << " = " << setprecision(6) << float(trueCount) / validationCount << endl;
}

void test_Category_SVM() {
    PlateCategory_SVM classifier;

    string imagesPath = "../../bin/platecharsamples/plates";
    vector<string> allImageCategories = Directory::GetFiles(imagesPath);
    vector<vector<float>> Hogs;
    vector<int> Tags;
    vector<string> imageNames;

    for (auto &imageCategory : allImageCategories) {
        vector<string> allImages = Directory::GetFiles(imageCategory);
        string tagStr = imageCategory.substr(imagesPath.size() + 1);
        auto tagIter = find(begin(PlateCategory_tToString),
                            end(PlateCategory_tToString), tagStr);
        assert(tagIter != end(PlateCategory_tToString));
        int tagInt = tagIter - begin(PlateCategory_tToString);
        for (auto &imageFile : allImages) {
            Mat image = cv::imread(imageFile);
            vector<float> Hog = classifier.ComputeHogDescriptors(image);
            Hogs.push_back(Hog);
            Tags.push_back(tagInt);
            imageNames.push_back(imageFile);
        }
    }

    int sampleCount = Hogs.size();
    int trainingCount = sampleCount * 0.9,
        validationCount = sampleCount - trainingCount;
    // int trainingCount = 1000, validationCount = 400;
    vector<size_t> randomIndices(sampleCount);
    iota(randomIndices.begin(), randomIndices.end(), 0);
    random_shuffle(randomIndices.begin(), randomIndices.end());

    int hogSize = Hogs[0].size();
    // int sizes[] = {trainingCount, hogSize};
    Mat training_data(trainingCount, hogSize, CV_32FC1, Scalar::all(0));
    Mat training_tag(trainingCount, 1, CV_32S);
    for (int i = 0; i < trainingCount; ++i) {
        Mat(1, hogSize, CV_32FC1, Hogs[randomIndices[i]].data())
            .copyTo(training_data.row(i));
        training_tag.at<int>(i) = Tags[randomIndices[i]];
        assert(abs(training_data.at<float>(i, 0) - Hogs[randomIndices[i]][0]) <
               1e-3);
        assert(abs(training_data.at<float>(i, 10) -
                   Hogs[randomIndices[i]][10]) < 1e-3);
        assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
        assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
    }

    if (!Directory::Exists("CategorySVM.yaml")) {
        cout << "Traininig ..." << endl;
        classifier.Train(training_data, training_tag);
        classifier.Save("CategorySVM.yaml");
    } else {
        cout << "Loading trained model ..." << endl;
        classifier.Load("CategorySVM.yaml");
    }

    vector<int> validationPrediction = {};
    int trueCount = 0;
    for (int i = trainingCount; i < trainingCount + validationCount; ++i) {
        Mat image = cv::imread(imageNames[randomIndices[i]]);
        int prediction = static_cast<int>(classifier.Test(image));
        validationPrediction.push_back(prediction);
        if (prediction == Tags[randomIndices[i]]) {
            ++trueCount;
        }
        cout << "\rValidating ... " << setiosflags(ios::fixed) << setw(2)
             << setprecision(1)
             << float(100 * (i - trainingCount)) / validationCount << "%";
    }
    cout << endl;
    cout << "validation accuracy: " << trueCount << " / " << validationCount
         << " = " << setprecision(6) << float(trueCount) / validationCount << endl;
}

#include <cstdio>
using std::remove;
void grid_search() {
    auto prepare_sample = []() {
        PlateChar_SVM classifier;

        string imagesPath = "../../bin/platecharsamples/chars";
        vector<string> allImageCategories = Directory::GetFiles(imagesPath);
        vector<vector<float>> Hogs;
        vector<int> Tags;
        vector<string> imageNames;

        for (auto &imageCategory : allImageCategories) {
            vector<string> allImages = Directory::GetFiles(imageCategory);
            string tagStr = imageCategory.substr(imagesPath.size() + 1);
            auto tagIter = find(begin(PlateChar_tToString),
                                end(PlateChar_tToString), tagStr);
            assert(tagIter != end(PlateChar_tToString));
            int tagInt = tagIter - begin(PlateChar_tToString);
            for (auto &imageFile : allImages) {
                Mat image = cv::imread(imageFile);
                vector<float> Hog = classifier.ComputeHogDescriptors(image);
                Hogs.push_back(Hog);
                Tags.push_back(tagInt);
                imageNames.push_back(imageFile);
            }
        }

        int sampleCount = Hogs.size();
        int trainingCount = sampleCount * 0.9,
            validationCount = sampleCount - trainingCount;
        vector<size_t> randomIndices(sampleCount);
        iota(randomIndices.begin(), randomIndices.end(), 0);
        random_shuffle(randomIndices.begin(), randomIndices.end());

        int hogSize = Hogs[0].size();
        Mat training_data(trainingCount, hogSize, CV_32FC1, Scalar::all(0));
        Mat training_tag(trainingCount, 1, CV_32S);
        for (int i = 0; i < trainingCount; ++i) {
            Mat(1, hogSize, CV_32FC1, Hogs[randomIndices[i]].data())
                .copyTo(training_data.row(i));
            training_tag.at<int>(i) = Tags[randomIndices[i]];
            assert(abs(training_data.at<float>(i, 0) -
                       Hogs[randomIndices[i]][0]) < 1e-3);
            assert(abs(training_data.at<float>(i, 10) -
                       Hogs[randomIndices[i]][10]) < 1e-3);
            assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
            assert(training_tag.at<int>(i, 0) == Tags[randomIndices[i]]);
        }

        Mat validation_data(validationCount, hogSize, CV_32FC1, Scalar::all(0));
        Mat validation_tag(validationCount, 1, CV_32S);
        for (int i = 0; i < validationCount; ++i) {
            Mat(1, hogSize, CV_32FC1,
                Hogs[randomIndices[i + trainingCount]].data())
                .copyTo(validation_data.row(i));
            validation_tag.at<int>(i) = Tags[randomIndices[i + trainingCount]];
        }
        return pair<pair<Mat, Mat>, pair<Mat, Mat>>{
            {training_data, training_tag}, {validation_data, validation_tag}};
    };

    auto train_svm = [](Mat &samples, Mat &responses, float C, float gamma,
                        SVM::KernelTypes kernel) {
        auto svm = SVM::create();
        svm->setType(SVM::Types::C_SVC);
        // svm->setKernel(SVM::KernelTypes::LINEAR);
        // svm->setKernel(SVM::KernelTypes::SIGMOID);
        svm->setKernel(kernel);
        if (kernel == SVM::KernelTypes::POLY) {
            svm->setDegree(0.5);
        }
        svm->setC(C);
        svm->setGamma(gamma);

        svm->setTermCriteria(
            TermCriteria(TermCriteria::Type::MAX_ITER, 1000, 1e-5));
        // svm->setTermCriteria(
        //     TermCriteria(TermCriteria::Type::MAX_ITER, 10000, 1e-10));
        svm->train(samples, SampleTypes::ROW_SAMPLE, responses);
        return svm;
    };

    auto validate = [](Ptr<SVM> &svm, Mat &sample_data, Mat &sample_tag) {
        int trueCount = 0;
        for (int i = 0; i < sample_data.rows; ++i) {
            Mat sample = sample_data.row(i);
            auto predictions = static_cast<int>(svm->predict(sample));
            trueCount += predictions == sample_tag.at<int>(i, 0);
        }
        int totalCount = sample_data.rows;
        return float(trueCount) / totalCount;
    };

    auto sample = prepare_sample();
    Mat training_data = sample.first.first, training_tag = sample.first.second,
        validation_data = sample.second.first,
        validation_tag = sample.second.second;

    vector<float> C_candidate = {5.0f, 10.0f, 15.f};
    vector<float> gamma_candidate = {0.9f, 1.4f, 2.f};
    vector<SVM::KernelTypes> kernel_candidate = {SVM::KernelTypes::RBF};
    float best_accuracy = -1;
    float best_C = C_candidate[0], best_gamma = gamma_candidate[0];
    SVM::KernelTypes best_kernel = SVM::KernelTypes::LINEAR;

    for (auto kernel : kernel_candidate) {
        for (auto C : C_candidate) {
            for (auto gamma : gamma_candidate) {

                if (Directory::Exists("CharSVM.yaml")) {
                    remove("CharSVM.yaml");
                }
                auto svm =
                    train_svm(training_data, training_tag, C, gamma, kernel);
                float accuracy = validate(svm, validation_data, validation_tag);
                if (accuracy > best_accuracy) {
                    best_C = C;
                    best_gamma = gamma;
                    best_accuracy = accuracy;
                    best_kernel = kernel;
                }

                cout << "C: " << C << ", "
                     << "gamma: " << gamma << ", kernel: " << kernel
                     << ", accuracy: " << accuracy
                     << ", validation count: " << validation_data.rows << endl;
            }
        }
    }

    cout << "best_C: " << best_C << ", "
         << "best_gamma: " << best_gamma << ", best_kernel: " << best_kernel
         << ", best_accuracy: " << best_accuracy << endl;
}

int main(int argc, char const *argv[]) {
    // test_charinfo();
    // test_plateinfo();
    test_Char_SVM();
    //test_Category_SVM();

    // grid_search();
    std::cin.get();
    return 0;
}
