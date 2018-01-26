#include "motiondetection.h"
#include <iostream>

using namespace cv;
using namespace std;

MotionDetection::MotionDetection(int cols, int rows)
{
    imgwidth = rows; imgheight = cols;
}

void MotionDetection::run()
{
    Mat resultimg(imgwidth,imgheight,CV_8UC3,Scalar(255,255,255));
    Point point;
//    printKeypoints();
    result.num = poseKeypoints.getSize(0);

    for (auto person = 0 ; person < poseKeypoints.getSize(0) ; person++)
    {
        //RShoulder
        point.x = (int)(poseKeypoints[{person, 3, 0}]);
        point.y = (int)(poseKeypoints[{person, 3, 1}]);
        circle(resultimg,point,2,Scalar(255,0,0));
    }
    imshow("result",resultimg);
}

void MotionDetection::printKeypoints()
{
    op::log("Person pose keypoints:");
    for (auto person = 0 ; person < poseKeypoints.getSize(0) ; person++)
    {
        op::log("Person " + std::to_string(person) + " (x, y, score):");
        for (auto bodyPart = 0 ; bodyPart < poseKeypoints.getSize(1) ; bodyPart++)
        {
            std::string valueToPrint;
            for (auto xyscore = 0 ; xyscore < poseKeypoints.getSize(2) ; xyscore++)
            {
                valueToPrint += std::to_string(   poseKeypoints[{person, bodyPart, xyscore}]   ) + " ";
            }
            op::log(valueToPrint);
        }
    }
    // Result for COCO (18 body parts)
    // POSE_COCO_BODY_PARTS {
    //     {0,  "Nose"},
    //     {1,  "Neck"},
    //     {2,  "RShoulder"},
    //     {3,  "RElbow"},
    //     {4,  "RWrist"},
    //     {5,  "LShoulder"},
    //     {6,  "LElbow"},
    //     {7,  "LWrist"},
    //     {8,  "RHip"},
    //     {9,  "RKnee"},
    //     {10, "RAnkle"},
    //     {11, "LHip"},
    //     {12, "LKnee"},
    //     {13, "LAnkle"},
    //     {14, "REye"},
    //     {15, "LEye"},
    //     {16, "REar"},
    //     {17, "LEar"},
    //     {18, "Background"},
    // }
}

MotionDetection::~MotionDetection()
{
    destroyAllWindows();
    std::cout << "MotionDetector delete successful." << std::endl;
}
