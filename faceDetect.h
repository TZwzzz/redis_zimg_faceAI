#ifndef FACEDETECT_H_
#define FACEDETECT_H_

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include "logs.h"

using namespace cv;
using namespace cv::dnn;
using namespace std;

struct ret_rect
{
    int topLx;
    int topLy;
    int width;
    int height;
    float threshold;
};

/** @brief Configure the input picture information
 *  @param Threshold:Confidence in face detection
 *  @returns 0:successfully -1:failed
 *  @note  None 
 */
int faceDetect_init(double Threshold);

/** @brief face detection func
 *  @param picName The name of input picture
 *  @returns vector of face rectangle
 *  @note  None 
 */
vector<ret_rect> faceDetect(String picName);


#endif