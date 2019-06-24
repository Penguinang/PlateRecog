#include "Utilities.h"

using namespace Doit::CV::PlateRecogn;

cv::Mat Utilities::IndexTransform(cv::Mat &originalMat) {
    cv::Mat plateMat = originalMat.clone();
    if (plateMat.channels() == 1) {
        cv::Mat midMat = originalMat.clone();
        midMat.convertTo(midMat, CV_32FC1);
        cv::normalize(midMat, midMat, 0.f, 1.f, cv::NORM_MINMAX);
        cv::pow(midMat, 2, midMat);
        cv::normalize(midMat, midMat, 0, 255, cv::NORM_MINMAX);
        midMat.convertTo(plateMat, CV_8UC1);
    } else if (plateMat.channels() == 3) {
        cv::Mat midMat = originalMat.clone();
        midMat.convertTo(midMat, CV_32FC3);
        cv::normalize(midMat, midMat, 0.f, 1.f, cv::NORM_MINMAX);
        cv::pow(midMat, 2, midMat);
        cv::normalize(midMat, midMat, 0, 255, cv::NORM_MINMAX);
        midMat.convertTo(plateMat, CV_8UC3);
    }
    return plateMat;
}

cv::Mat Utilities::LogTransform(cv::Mat &originalMat) {
    cv::Mat plateMat = originalMat.clone();
    if (plateMat.channels() == 1) {
        plateMat.convertTo(plateMat, CV_32FC1);
        for (int i = 0; i < plateMat.rows; i++) {
            for (int j = 0; j < plateMat.cols; j++) {
                plateMat.at<float>(i, j) =
                    log10f(plateMat.at<float>(i, j) + 1.0f);
            }
        }
        cv::normalize(plateMat, plateMat, 0, 255, cv::NORM_MINMAX);
        plateMat.convertTo(plateMat, CV_8UC1);

    } else if (plateMat.channels() == 3) {
        plateMat.convertTo(plateMat, CV_32FC3);
        for (int i = 0; i < plateMat.rows; i++) {
            for (int j = 0; j < plateMat.cols; j++) {
                plateMat.at<cv::Vec3f>(i, j)[0] =
                    log10f(plateMat.at<cv::Vec3f>(i, j)[0] + 1.0f);
                plateMat.at<cv::Vec3f>(i, j)[1] =
                    log10f(plateMat.at<cv::Vec3f>(i, j)[1] + 1.0f);
                plateMat.at<cv::Vec3f>(i, j)[2] =
                    log10f(plateMat.at<cv::Vec3f>(i, j)[2] + 1.0f);
            }
        }
        cv::normalize(plateMat, plateMat, 0, 255, cv::NORM_MINMAX);
        plateMat.convertTo(plateMat, CV_8UC3);
    }
    return plateMat;
}

cv::Mat Utilities::GammaTransform(cv::Mat &originalMat, float gammaFactor) {
    cv::Mat plateMat = originalMat.clone();

    if (plateMat.channels() == 1) {
        plateMat.convertTo(plateMat, CV_32FC1);
        cv::normalize(plateMat, plateMat, 0, 1, cv::NORM_MINMAX);
        cv::pow(plateMat, gammaFactor, plateMat);
        cv::normalize(plateMat, plateMat, 0, 255, cv::NORM_MINMAX);
        plateMat.convertTo(plateMat, CV_8UC1);
    } else if (plateMat.channels() == 3) {
        plateMat.convertTo(plateMat, CV_32FC3);
        cv::normalize(plateMat, plateMat, 0, 1, cv::NORM_MINMAX);
        cv::pow(plateMat, gammaFactor, plateMat);
        cv::normalize(plateMat, plateMat, 0, 255, cv::NORM_MINMAX);
        plateMat.convertTo(plateMat, CV_8UC3);
    }
    return plateMat;
}

cv::Mat Utilities::LaplaceTransform(cv::Mat &originalMat) {
    cv::Mat kernel = cv::Mat(3, 3, CV_32FC1).clone();
    kernel.at<float>(0, 0) = 0;
    kernel.at<float>(0, 1) = -1;
    kernel.at<float>(0, 2) = 0;
    kernel.at<float>(1, 0) = 0;
    kernel.at<float>(1, 1) = 5;
    kernel.at<float>(1, 2) = 0;
    kernel.at<float>(2, 0) = 0;
    kernel.at<float>(2, 1) = -1;
    kernel.at<float>(2, 2) = 0;
    cv::Mat result;
    cv::filter2D(originalMat, result, 3, kernel);
    return result;
}

Rect Utilities::GetSafeRect(const Rect &rect, const Mat &mat) {
    int x = rect.x, y = rect.y, width = rect.width, height = rect.height;
    if (x < 0) {
        x = 0;
    } else if (x > mat.cols - 1) {
        x = mat.cols - 1;
    }

    if (y < 0) {
        y = 0;
    } else if (y > mat.rows - 1) {
        y = mat.rows - 1;
    }

    if (width < 0) {
        width = 0;
    } else if (x + width > mat.cols - 1) {
        width = mat.cols - 1 - x;
    }

    if (height < 0) {
        height = 0;
    } else if (y + height > mat.rows - 1) {
        height = mat.rows - 1 - y;
    }

    return {x, y, width, height};
}

cv::Mat Utilities::HistogramTransform(cv::Mat &originalMat) {
    Mat transMat = originalMat.clone();
    vector<Mat> bgrMats(3);
    cv::split(transMat, bgrMats);
    cv::equalizeHist(bgrMats[0], bgrMats[0]);
    cv::equalizeHist(bgrMats[1], bgrMats[1]);
    cv::equalizeHist(bgrMats[2], bgrMats[2]);
    cv::merge(bgrMats, transMat);
    return transMat;
}