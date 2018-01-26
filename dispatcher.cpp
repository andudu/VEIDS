#include "dispatcher.h"
#include "error.h"
using namespace std;

Dispatcher::Dispatcher(string path)
{
    if(path.length()>2)
        cap.open(path);
    else{
        cap.open(atoi(path.c_str()));
        cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    }
}

void Dispatcher::run(){
    cv::Mat frame;
    if(!cap.isOpened())
    {
        cout << "Capture open failed!" << endl; return;
    }
    cap.read(frame);
    cv::resize(frame,frame,cv::Size(640,480));
    openpose = new OpenPose();
    motiondetector = new MotionDetection(frame.cols,frame.rows);
    while(cap.read(frame))
    {
        openpose->wait();
        if(flag_exit){
            motiondetector->wait();
            break;
        }
        if(!openpose->outputImage.empty()){
            emit outputImgDone(openpose->outputImage);
            motiondetector->wait();
            emit outputMsgDone(motiondetector->result);
            motiondetector->poseKeypoints = openpose->poseKeypoints;
            motiondetector->start();
        }
        openpose->inputImage = frame;
        openpose->start();
    }
    cap.release();
    delete openpose;
    delete motiondetector;
}

Dispatcher::~Dispatcher(){
}

