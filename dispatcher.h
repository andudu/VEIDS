#ifndef Dispatcher_H
#define Dispatcher_H

#include <iostream>
#include <QObject>
#include <QThread>
#include "openpose.h"
#include "motiondetection.h"
#include <opencv2/opencv.hpp>

class Dispatcher : public QThread
{
    Q_OBJECT
public:
    Dispatcher(std::string path);
    ~Dispatcher();
    bool flag_exit = false;
private:
    std::string path;
    cv::VideoCapture cap;
    OpenPose *openpose;
    MotionDetection *motiondetector;
protected:
    void run();
signals:
    void outputImgDone(cv::Mat resultimg);
    void outputMsgDone(msgOutput resultmsg);
};

#endif // Dispatcher_H
