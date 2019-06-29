#include "Utilities.h"

using cv::Scalar;
using cv::Size;

#include <vector>
using std::vector;

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

// following code is from
// http://www.juergenbrauer.org/old_wiki/doku.php?id=public:hog_descriptor_computation_and_visualization
cv::Mat Utilities::get_hogdescriptor_visu(const Mat &color_origImg,
                               vector<float> &descriptorValues, const cv::Size &size) {
    const int DIMX = size.width;
    const int DIMY = size.height;
    float zoomFac = 3;
    Mat visu;
    resize(color_origImg, visu,
           Size((int)(color_origImg.cols * zoomFac),
                (int)(color_origImg.rows * zoomFac)));

    int cellSize = 8;
    int gradientBinSize = 9;
    float radRangeForOneBin =
            (float)(CV_PI /
                    (float)gradientBinSize); // dividing 180 into 9 bins, how large
    // (in rad) is one bin?

    // prepare data structure: 9 orientation / gradient strenghts for each cell
    int cells_in_x_dir = DIMX / cellSize;
    int cells_in_y_dir = DIMY / cellSize;
    float ***gradientStrengths = new float **[cells_in_y_dir];
    int **cellUpdateCounter = new int *[cells_in_y_dir];
    for (int y = 0; y < cells_in_y_dir; y++) {
        gradientStrengths[y] = new float *[cells_in_x_dir];
        cellUpdateCounter[y] = new int[cells_in_x_dir];
        for (int x = 0; x < cells_in_x_dir; x++) {
            gradientStrengths[y][x] = new float[gradientBinSize];
            cellUpdateCounter[y][x] = 0;

            for (int bin = 0; bin < gradientBinSize; bin++)
                gradientStrengths[y][x][bin] = 0.0;
        }
    }

    // nr of blocks = nr of cells - 1
    // since there is a new block on each cell (overlapping blocks!) but the
    // last one
    int blocks_in_x_dir = cells_in_x_dir - 1;
    int blocks_in_y_dir = cells_in_y_dir - 1;

    // compute gradient strengths per cell
    int descriptorDataIdx = 0;
    int cellx = 0;
    int celly = 0;

    for (int blockx = 0; blockx < blocks_in_x_dir; blockx++) {
        for (int blocky = 0; blocky < blocks_in_y_dir; blocky++) {
            // 4 cells per block ...
            for (int cellNr = 0; cellNr < 4; cellNr++) {
                // compute corresponding cell nr
                cellx = blockx;
                celly = blocky;
                if (cellNr == 1)
                    celly++;
                if (cellNr == 2)
                    cellx++;
                if (cellNr == 3) {
                    cellx++;
                    celly++;
                }

                for (int bin = 0; bin < gradientBinSize; bin++) {
                    float gradientStrength =
                            descriptorValues[descriptorDataIdx];
                    descriptorDataIdx++;

                    gradientStrengths[celly][cellx][bin] += gradientStrength;

                } // for (all bins)

                // note: overlapping blocks lead to multiple updates of this
                // sum! we therefore keep track how often a cell was updated, to
                // compute average gradient strengths
                cellUpdateCounter[celly][cellx]++;

            } // for (all cells)

        } // for (all block x pos)
    }     // for (all block y pos)

    // compute average gradient strengths
    for (celly = 0; celly < cells_in_y_dir; celly++) {
        for (cellx = 0; cellx < cells_in_x_dir; cellx++) {

            float NrUpdatesForThisCell = (float)cellUpdateCounter[celly][cellx];

            // compute average gradient strenghts for each gradient bin
            // direction
            for (int bin = 0; bin < gradientBinSize; bin++) {
                gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
            }
        }
    }

    // draw cells
    for (celly = 0; celly < cells_in_y_dir; celly++) {
        for (cellx = 0; cellx < cells_in_x_dir; cellx++) {
            int drawX = cellx * cellSize;
            int drawY = celly * cellSize;

            int mx = drawX + cellSize / 2;
            int my = drawY + cellSize / 2;

            rectangle(visu,
                      Point((int)(drawX * zoomFac), (int)(drawY * zoomFac)),
                      Point((int)((drawX + cellSize) * zoomFac),
                            (int)((drawY + cellSize) * zoomFac)),
                      Scalar(100, 100, 100), 1);

            // draw in each cell all 9 gradient strengths
            for (int bin = 0; bin < gradientBinSize; bin++) {
                float currentGradStrength =
                        gradientStrengths[celly][cellx][bin];

                // no line to draw?
                if (currentGradStrength == 0)
                    continue;

                float currRad = bin * radRangeForOneBin + radRangeForOneBin / 2;

                float dirVecX = cos(currRad);
                float dirVecY = sin(currRad);
                float maxVecLen = (float)(cellSize / 2.f);
                float scale =
                        2.5; // just a visualization scale, to see the lines better

                // compute line coordinates
                float x1 =
                        mx - dirVecX * currentGradStrength * maxVecLen * scale;
                float y1 =
                        my - dirVecY * currentGradStrength * maxVecLen * scale;
                float x2 =
                        mx + dirVecX * currentGradStrength * maxVecLen * scale;
                float y2 =
                        my + dirVecY * currentGradStrength * maxVecLen * scale;

                // draw gradient visualization
                line(visu, Point((int)(x1 * zoomFac), (int)(y1 * zoomFac)),
                     Point((int)(x2 * zoomFac), (int)(y2 * zoomFac)),
                     Scalar(0, 255, 0), 1);

            } // for (all bins)

        } // for (cellx)
    }     // for (celly)

    // don't forget to free memory allocated by helper data structures!
    for (int y = 0; y < cells_in_y_dir; y++) {
        for (int x = 0; x < cells_in_x_dir; x++) {
            delete[] gradientStrengths[y][x];
        }
        delete[] gradientStrengths[y];
        delete[] cellUpdateCounter[y];
    }
    delete[] gradientStrengths;
    delete[] cellUpdateCounter;

    return visu;

} // get_hogdescriptor_visu


namespace Doit {
namespace CV {
namespace PlateRecogn {
void drawBoundingRects(Mat &InputOutputMat,
                       const vector<vector<Point>> &contours, int index,
                       const Scalar &color) {
    if (index != -1) {
        Rect rect = cv::boundingRect(contours[index]);
        cv::rectangle(InputOutputMat, rect, color);
    } else {
        for (auto &contour : contours) {
            Rect rect = cv::boundingRect(contour);
            cv::rectangle(InputOutputMat, rect, color);
        }
    }
}

void reserveBoundingRects(Mat &InputOutputMat,
                       const vector<vector<Point>> &contours, int index,
                       const Scalar &color) {
    Mat output = InputOutputMat.clone() = 0;
    if (index != -1) {
        Rect rect = cv::boundingRect(contours[index]);
        output(rect) = InputOutputMat(rect) + 0;
        InputOutputMat = output;
    } else {
        for (auto &contour : contours) {
            Rect rect = cv::boundingRect(contour);
            output(rect) = InputOutputMat(rect) + 0;
        }
        InputOutputMat = output;
    }
}


} // namespace PlateRecogn
} // namespace CV
} // namespace Doit
