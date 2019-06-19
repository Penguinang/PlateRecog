
#include <opencv2/core.hpp>
using namespace cv;

#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
    float arr[] = {1, 2, 3, 4, 5, 6, 6};
    int dims[] = {2, 2, 2};
    Mat a = Mat(3, 3, CV_32FC1);
    Mat b = Mat(3, 3, CV_32FC1, arr);
    
    // a.row(0) = b.row(0).clone()+0;
    // cout << a << endl;    

    // b.row(0) = b.row(1).clone();

    // auto row = b.row(1);
    // b.row(0).assignTo(row);

    b.row(1).copyTo(b.row(0));
    cout << b << endl;

    // a = b;
    // cout << a << endl;
    return 0;
}


    // float arr[] = {1, 2, 3, 4, 5};
    // Mat a = Mat(19, 11, CV_32FC1, arr);
    // cout << a.dims << endl;
    // cout << a.rows << endl;    
    // cout << a.cols << endl;
    // cout << a.size() << endl;

    // int dims[] = {2, 2, 2, 2, 2};
    // Mat b = Mat(5, dims, CV_32FC1);
    // cout << b.dims << endl;
    // cout << b.rows << endl;    
    // cout << b.cols << endl;
    // cout << b.size() << endl;

    // Mat c = b.row(0);
    // cout << c.dims << endl;
    // cout << c.rows << endl;    
    // cout << c.cols << endl;
    // cout << c.size() << endl;

    // // Mat e = Mat(5, dims, CV_32FC1, arr);
    // c.row(0) = a.row(3);
    // // cout << c.size << endl;
    // // cout << c.row(0).size << endl;
    // // cout << c.size[3] << endl;
    // int indices[] = {0, 0, 0, 0, 0};
    // cout << c.at<float>(indices) << endl;
    // cout << a.row(3) << endl;
    // cout << c.row(0).reshape(1) << endl;