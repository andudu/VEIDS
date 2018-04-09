#ifndef MOTIONDETECTION_H
#define MOTIONDETECTION_H

#include <iostream>
#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <openpose/headers.hpp>

struct msgOutput
{
    int num = 0;
    int state = 0x00;
};

class MotionDetection : public QThread
{
    Q_OBJECT
public:
    MotionDetection(std::string resolution);
    ~MotionDetection();
    msgOutput result;
    cv::Mat outputImage;
    op::Array<float> poseKeypoints;
    int imgwidth,imgheight;
private:
    void printKeypoints();
protected:
    void run();
signals:
    void outputDone();
};

#endif // MOTIONDETECTION_H
