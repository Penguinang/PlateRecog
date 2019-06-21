#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//using namespace cv;
using cv::imread;
using cv::Mat;
using cv::Rect;
using cv::Point2i;


#include <iostream>
// using namespace std;
using std::cout;
using std::endl;

#include "CharSegment_V3.h"
#include "CharInfo.h"
using namespace Doit::CV::PlateRecogn;

#include "debug.h"
// inline void DebugVisualize(const string &WindowName, const Mat &mat){
// #ifdef VISUALIZE_DEBUG
//     cv::imshow(WindowName, mat);
//     waitKey(0);
// #endif // VISUALIZE_DEBUG
// }

void test_SplitePlateByGammaTransform(){
    Mat image = imread("../../bin/licenses/12867_2019-03-28-08-09-51-086918.jpg");
    string license = "粤A CH383";
    Rect rectRoi = Rect(1030, 463, 1201-1030, 540-463);

    // Mat image =
    //     imread("../../bin/licenses/DE4661_2019-03-23-08-55-40-053804.jpg");
    // string license = "粤A DE466";    
    // Rect rectRoi = Rect(756, 384, 880-756, 440-384);

    // Mat image =
    //     imread("../../bin/licenses/AM5222_2019-03-23-11-23-57-072708.jpg");
    // string license = "粤A M5222";    
    // Rect rectRoi = Rect(Point2i{968, 332}, Point2i{1074, 380});

    // Mat image =
    //     imread("../../bin/licenses/60T17_61775.jpg");
    // string license = "粤A DB797";    
    // Rect rectRoi = Rect(Point2i{698, 406}, Point2i{818, 455});

    // Mat image =
    //     imread("../../bin/platecharsamples/plates/普通车牌/2019-03-12-09-28-36-069855_颜色法_8.jpg");
    // string license = "粤K WA305";    
    // Rect rectRoi = Rect(Point2i{0, 0}, Point2i{77, 23});

    // Mat image =
    //     imread("../../bin/licenses/CG948_2019-03-23-10-43-23-076807.jpg");
    // string license = "粤A CG948";
    // Rect rectRoi = Rect(Point2i{772, 144}, Point2i{841, 193});
    

    image = image(rectRoi);
    DebugVisualize("Origin", image);
    PlateColor_t color = PlateColor_t::YellowPlate;
    auto charInfos = CharSegment_V3::SplitePlateByGammaTransform(image, color);

    cout << "Recognized rect count " << charInfos.size() << endl;
    for(auto charInfo : charInfos){
        charInfo.PlateChar = PlateChar_t::NonChar;
        cout << charInfo.ToString() << charInfo.OriginalRect << endl;
        DebugVisualize("Rect", image(charInfo.OriginalRect));
    }

    cout << "Real license " << license << endl;

}

int main(int argc, char const *argv[])
{
    test_SplitePlateByGammaTransform();
    return 0;
}
