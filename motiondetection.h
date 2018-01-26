#ifndef MOTIONDETECTION_H
#define MOTIONDETECTION_H

#include <iostream>
#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <openpose/headers.hpp>

struct msgOutput
{
    int num;
    int state;
};

class MotionDetection : public QThread
{
    Q_OBJECT
public:
    MotionDetection(std::string resolution);
    ~MotionDetection();
    msgOutput result;
    op::Array<float> poseKeypoints;
private:
    int imgwidth,imgheight;
    void printKeypoints();
protected:
    void run();
};

#endif // MOTIONDETECTION_H
