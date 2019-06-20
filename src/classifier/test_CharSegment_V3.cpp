#include <opencv2/core.hpp>
using namespace cv;

#include <iostream>
using namespace std;

#include "CharSegment_V3.h"
#include "CharInfo.h"
using namespace Doit::CV::PlateRecogn;

void test_SplitePlateByGammaTransform(){
    Mat mat = imread("../../bin/licenses/12867_2019-03-28-08-09-51-086918.jpg");
    string license = "粤A CH383";

    Rect rectRoi = Rect(1045, 463, 1201-1045, 523-463);
    mat = mat(rectRoi);
    PlateColor_t color = PlateColor_t::YellowPlate;
    auto charInfos = CharSegment_V3::SplitePlateByGammaTransform(mat, color);
    for(auto charInfo : charInfos){
        cout << charInfo.ToString() << endl;
    }

    cout << "Real license " << license << endl;

}

int main(int argc, char const *argv[])
{
    test_SplitePlateByGammaTransform();
    return 0;
}
