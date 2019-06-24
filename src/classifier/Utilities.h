#ifndef UTILITIES_H
#define UTILITIES_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>
using cv::Rect;
using cv::Mat;

#include <vector>
#include <cmath>

#include "PlateCategory_SVM.h"
#include "PlateChar_SVM.h"
#include "CharInfo.h"

using namespace std;
namespace Doit{
namespace CV{
namespace PlateRecogn{

class Utilities{
public:
    static cv::Mat IndexTransform(cv::Mat &originalMat)
    {
        cv::Mat plateMat = originalMat.clone();
        if(plateMat.channels()==1)
        {
            cv::Mat midMat = originalMat.clone();
            midMat.convertTo(midMat, CV_32FC1);
            cv::normalize(midMat, midMat, 0.f, 1.f, cv::NORM_MINMAX);
            cv::exp(midMat, midMat);
            cv::normalize(midMat,midMat,0,255,cv::NORM_MINMAX);
            midMat.convertTo(plateMat, CV_8UC1);
        }
        else if(plateMat.channels()==3)
        {
            cv::Mat midMat = originalMat.clone();
            midMat.convertTo(midMat, CV_32FC3);
            cv::normalize(midMat, midMat, 0.f, 1.f, cv::NORM_MINMAX);
            cv::exp(midMat, midMat);
            cv::normalize(midMat,midMat,0,255,cv::NORM_MINMAX);
            midMat.convertTo(plateMat, CV_8UC3);
        }
        return plateMat;
    }

    static cv::Mat LogTransform(cv::Mat &originalMat)
    {
        cv::Mat plateMat = originalMat.clone();
        if(plateMat.channels()==1)
        {
            for(int i=0;i<plateMat.rows;i++)
            {
                for(int j=0;j<plateMat.cols;j++)
                {
                    plateMat.at<uchar>(i,j) = (unsigned char)(logf(plateMat.at<uchar>(i, j)));
                }
            }
            cv::normalize(plateMat,plateMat,0,255,cv::NORM_MINMAX);

        }
        else if(plateMat.channels()==3)
        {
            for(int i=0;i<plateMat.rows;i++)
            {
                for(int j=0;j<plateMat.cols;j++)
                {
                    plateMat.at<cv::Vec3b>(i,j)[0]=(unsigned char)(logf(originalMat.at<cv::Vec3b>(i, j)[0]));
                    plateMat.at<cv::Vec3b>(i,j)[1]=(unsigned char)(logf(originalMat.at<cv::Vec3b>(i, j)[1]));
                    plateMat.at<cv::Vec3b>(i,j)[2]=(unsigned char)(logf(originalMat.at<cv::Vec3b>(i, j)[2]));
                }
            }
            cv::normalize(plateMat,plateMat,0,255,cv::NORM_MINMAX);
        }
        return plateMat;
    }

    static cv::Mat GammaTransform(cv::Mat &originalMat,float gammaFactor)
    {      
        cv::Mat plateMat = originalMat.clone();
		
	
        if(plateMat.channels()==1)
        {
			plateMat.convertTo(plateMat, CV_32FC1);
			cv::normalize(plateMat, plateMat, 0, 1, cv::NORM_MINMAX);
			cv::pow(plateMat,gammaFactor,plateMat);
            cv::normalize(plateMat,plateMat,0,255,cv::NORM_MINMAX);
			plateMat.convertTo(plateMat, CV_8UC3);
        }
        else if(plateMat.channels()==3)
        {
			plateMat.convertTo(plateMat, CV_32FC3);
			cv::normalize(plateMat, plateMat, 0, 1, cv::NORM_MINMAX);
			cv::pow(plateMat, gammaFactor, plateMat);
            cv::normalize(plateMat,plateMat,0,255,cv::NORM_MINMAX);
			plateMat.convertTo(plateMat, CV_8UC3);
        }
        return plateMat;
    }

    static Rect GetSafeRect(const Rect &rect, const Mat &mat){
        int x = rect.x, y = rect.y, width = rect.width, height = rect.height;
        if(x < 0){
            x = 0;
        }
        else if(x > mat.cols-1){
            x = mat.cols-1;
        }

        if(y < 0){
            y = 0;
        }
        else if(y > mat.rows-1){
            y = mat.rows-1;
        }

        if(width < 0){
            width = 0;
        } else if(x + width > mat.cols-1){
            width = mat.cols-1 - x;
        }

        if(height < 0){
            height = 0;
        } else if(y + height > mat.rows-1){
            height = mat.rows-1 - y;
        }
                
        return {x, y, width, height};
    }

    static cv::Mat HistogramTransform(cv::Mat &originalMat){
        Mat transMat = originalMat.clone();
        vector<Mat> bgrMats(3);
        cv::split(transMat, bgrMats);
        cv::equalizeHist(bgrMats[0], bgrMats[0]);
        cv::equalizeHist(bgrMats[1], bgrMats[1]);
        cv::equalizeHist(bgrMats[2], bgrMats[2]);
        cv::merge(bgrMats, transMat);
        return transMat;
    }
};
}
}
}

#endif
