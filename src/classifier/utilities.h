#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>
#include "vector"
#include "PlateCategory_SVM.h"
#include "PlateChar_SVM.h"
#include "cmath"
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
            for(int i=0;i<plateMat.rows;i++)
            {
                for(int j=0;j<plateMat.cols;j++)
                {
                    plateMat.at<uchar>(i,j) = (unsigned char)(255.0*expf(originalMat.at<uchar>(i, j)));
                }
            }
            cv::normalize(plateMat,plateMat,0,255,NOMINMAX);

        }
        else if(plateMat.channels()==3)
        {
            for(int i=0;i<plateMat.rows;i++)
            {
                for(int j=0;j<plateMat.cols;j++)
                {
                    plateMat.at<cv::Vec3f>(i,j)[0]=(unsigned char)(expf(originalMat.at<cv::Vec3f>(i, j)[0]));
                    plateMat.at<cv::Vec3f>(i,j)[1]=(unsigned char)(expf(originalMat.at<cv::Vec3f>(i, j)[1]));
                    plateMat.at<cv::Vec3f>(i,j)[2]=(unsigned char)(expf(originalMat.at<cv::Vec3f>(i, j)[2]));
                }
            }
            cv::normalize(plateMat,plateMat,0,255,NOMINMAX);
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
            cv::normalize(plateMat,plateMat,0,255,NOMINMAX);

        }
        else if(plateMat.channels()==3)
        {
            for(int i=0;i<plateMat.rows;i++)
            {
                for(int j=0;j<plateMat.cols;j++)
                {
                    plateMat.at<cv::Vec3f>(i,j)[0]=(unsigned char)(logf(originalMat.at<cv::Vec3f>(i, j)[0]));
                    plateMat.at<cv::Vec3f>(i,j)[1]=(unsigned char)(logf(originalMat.at<cv::Vec3f>(i, j)[1]));
                    plateMat.at<cv::Vec3f>(i,j)[2]=(unsigned char)(logf(originalMat.at<cv::Vec3f>(i, j)[2]));
                }
            }
            cv::normalize(plateMat,plateMat,0,255,NOMINMAX);
        }
        return plateMat;
    }

    static cv::Mat GammaTransform(cv::Mat &originalMat,float gammaFactor)
    {      
        cv::Mat plateMat = originalMat.clone();
        if(plateMat.channels()==1)
        {
            for(int i=0;i<plateMat.rows;i++)
            {
                for(int j=0;j<plateMat.cols;j++)
                {
                    plateMat.at<uchar>(i,j) = (unsigned char)(pow(plateMat.at<uchar>(i, j),gammaFactor));
                }
            }
            cv::normalize(plateMat,plateMat,0,255,NOMINMAX);

        }
        else if(plateMat.channels()==3)
        {
            for(int i=0;i<plateMat.rows;i++)
            {
                for(int j=0;j<plateMat.cols;j++)
                {
                    plateMat.at<cv::Vec3f>(i,j)[0]=(unsigned char)(pow(plateMat.at<cv::Vec3f>(i, j)[0],gammaFactor));
                    plateMat.at<cv::Vec3f>(i,j)[1]=(unsigned char)(pow(plateMat.at<cv::Vec3f>(i, j)[1],gammaFactor));
                    plateMat.at<cv::Vec3f>(i,j)[2]=(unsigned char)(pow(plateMat.at<cv::Vec3f>(i, j)[2],gammaFactor));
                }
            }
            cv::normalize(plateMat,plateMat,0,255,NOMINMAX);
        }
        return plateMat;
    }
};
}
}
}
