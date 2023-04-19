#include "faceDetect.h"

Net faceNet;
float confidenceThreshold;

int faceDetect_init(double Threshold)
{
    String model_bin = "../ch12_face_age/opencv_face_detector_uint8.pb";
    String config_text = "../ch12_face_age/opencv_face_detector.pbtxt";
    faceNet = readNet(model_bin,config_text);
    if(faceNet.empty())
    {
        cout << "Make sure you enter the correct model file" << endl;
        return -1;
    }
    confidenceThreshold = Threshold;
    return 0;
}


vector<ret_rect> faceDetect(String picName)
{
    vector<ret_rect> vec_ret_rect;
    Mat img = imread(picName);
    ret_rect retRect;

    vec_ret_rect.clear();
    memset(&retRect, 0, sizeof(retRect));

    if(img.empty())
    {
        cout << "Make sure you enter the correct picName file" << endl;
        return vec_ret_rect;
    }
    Mat blobImage = blobFromImage(img,1.0,Size(300,300),Scalar(),false,false);
    faceNet.setInput(blobImage,"data");
    Mat detect = faceNet.forward("detection_out");
    Mat detectionMat(detect.size[2],detect.size[3],CV_32F,detect.ptr<float>());

    for(int i = 0;i < detectionMat.rows;i++)
    {
        memset(&retRect, 0, sizeof(retRect));
        retRect.threshold = detectionMat.at<float>(i,2);
        if(retRect.threshold > confidenceThreshold)
        {
            retRect.topLx = detectionMat.at<float>(i,3) * img.cols;
            retRect.topLy = detectionMat.at<float>(i,4) * img.rows;
            retRect.width = detectionMat.at<float>(i,5) * img.cols - retRect.topLx;
            retRect.height = detectionMat.at<float>(i,6) * img.rows - retRect.topLy;
            rectangle(img,Rect(retRect.topLx,retRect.topLy,retRect.width,retRect.height),
                        Scalar(0,0,255),2,8,0);
            vec_ret_rect.push_back(retRect);
        }
    }
    // imshow("result",img);
    return vec_ret_rect;
}

