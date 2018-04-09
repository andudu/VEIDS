#include "motiondetection.h"
#include <iostream>
#include <cmath>
#include"omp.h"

using namespace cv;
using namespace std;

#define threshold_num_of_people 6

struct People
{
    Point center;
    int height;
    int weight;
};

MotionDetection::MotionDetection(std::string resolution)
{
    sscanf(resolution.c_str(),"%dx%d",&imgwidth,&imgheight);
}

void MotionDetection::run()
{
    //count people
    vector<People> people;
    for(int person_count = 0; person_count < poseKeypoints.getSize(0); person_count++)
    {
        People person;
        Point Neck,RHip,LHip;
        Neck.x = poseKeypoints[{person_count, 1, 0}];
        if(Neck.x == 0) continue;
        Neck.y = poseKeypoints[{person_count, 1, 1}];
        RHip.x = poseKeypoints[{person_count, 8, 0}];
        if(RHip.x == 0) continue;
        RHip.y = poseKeypoints[{person_count, 8, 1}];
        LHip.x = poseKeypoints[{person_count, 11, 0}];
        if(LHip.x == 0) continue;
        LHip.y = poseKeypoints[{person_count, 11, 1}];

        person.center.x = (Neck.x + RHip.x + LHip.x)/3;
        person.center.y = (Neck.y + RHip.y + LHip.y)/3;
        person.height = (RHip.y+LHip.y)/2-Neck.y;
        person.weight = abs(Neck.x - (RHip.x+LHip.x)/2);
        people.push_back(person);
    }


    //异常一：总人数过多 0000 0001
    result.num = people.size();
    if(result.num >= threshold_num_of_people)
        result.state |= 1;
    else{
        result.state &= ~1;
    }

    //画图
//    for(uint person_count = 0; person_count < people.size(); person_count++)
//    {
//        People person = people[person_count];
//        rectangle(outputImage,Point(person.center.x-person.tall/2,person.center.y-person.tall),
//                  Point(person.center.x+person.tall/2,person.center.y+person.tall),3);
//        putText(outputImage,to_string(person_count),person.center,FONT_HERSHEY_COMPLEX,0.5,Scalar(0),1);
//    }

//    if(result.num < 2) return;

    //异常二：过分集中 0000 0010
    int cishu=0;
//#pragma omp parallel for
    for(uint i = 0; i < people.size(); i++)
    {
        People now = people[i];
        for(uint j=i+1; j < people.size(); j++)
        {
            People next = people[j];
            if((float)now.height/next.height > 1.5 || (float)next.height/now.height > 1.5) continue;
            if(abs(now.center.x - next.center.x)/2 > now.height) continue;
            if(abs(now.center.y - next.center.y) > now.height) continue;
            cishu++;
        }
    }
    if(cishu > 3)
        result.state |= 2;
    else
        result.state &= ~2;

    //异常三：手举过肩 0000 0100
    bool state3 = false;
#pragma omp parallel for
    for (auto person = 0 ; person < people.size() ; person++)
    {
        float RShoulderY = (poseKeypoints[{person, 2, 1}]);
        float RWristY = (poseKeypoints[{person, 4, 1}]);
        if(RWristY == 0)  RWristY = (poseKeypoints[{person, 3, 1}]);
        if(RShoulderY != 0 || RWristY != 0)
        {
            if(RShoulderY > RWristY)
            {
                state3 = true;
                People personData = people[person];
                rectangle(outputImage,Point(personData.center.x-personData.weight,personData.center.y-personData.height),
                          Point(personData.center.x+personData.weight,personData.center.y+personData.height),Scalar(255,0,0),2);
            }
        }

        float LShoulderY = (poseKeypoints[{person, 5, 1}]);
        if(LShoulderY == 0) continue;
        float LWristY = (poseKeypoints[{person, 7, 1}]);
        if(LWristY == 0)
        {
            LWristY = (poseKeypoints[{person, 6, 1}]);
            if(LWristY == 0) continue;
        }

        if(LShoulderY > LWristY)
        {
            state3 = true;
            People personData = people[person];
            rectangle(outputImage,Point(personData.center.x-personData.weight,personData.center.y-personData.height),
                      Point(personData.center.x+personData.weight,personData.center.y+personData.height),Scalar(255,0,0),2);
        }
    }
    if(state3)
        result.state |= 4;
    else
        result.state &= ~4;

////        circle(outputImage,RShoulder,5,Scalar(255,0,0));
////        circle(outputImage,RElbow,5,Scalar(255,0,0));
////        circle(outputImage,RWrist,5,Scalar(255,0,0));
////        line(outputImage,RShoulder,RElbow,Scalar(255,0,0),3);
////        line(outputImage,RWrist,RElbow,Scalar(255,0,0),3);
////        circle(outputImage,RShoulder,5,Scalar(255,0,0));
////        circle(outputImage,RElbow,5,Scalar(255,0,0));
////        circle(outputImage,RWrist,5,Scalar(255,0,0));
////        line(outputImage,RShoulder,RElbow,Scalar(255,0,0),3);
////        line(outputImage,RWrist,RElbow,Scalar(255,0,0),3);
////        circle(outputImage,LShoulder,5,Scalar(0,255,0));
////        circle(outputImage,LElbow,5,Scalar(0,255,0));
////        circle(outputImage,LWrist,5,Scalar(0,255,0));
////        line(outputImage,LShoulder,LElbow,Scalar(0,255,0),3);
////        line(outputImage,LWrist,LElbow,Scalar(0,255,0),3);

    //异常四：有人倒地 0000 1000
    bool state4 = false;
#pragma omp parallel for
    for(uint i = 0; i < people.size(); i++)
    {
        People personData = people[i];
        if(personData.weight > personData.height*3/5.0)
        {
            state4 = true;
            rectangle(outputImage,Point(personData.center.x-personData.weight,personData.center.y-personData.height),
                      Point(personData.center.x+personData.weight,personData.center.y+personData.height),Scalar(0,0,255),2);
        }
    }
    if(state4)
        result.state |= 8;
    else
        result.state &= ~8;

    emit outputDone();
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
    //     {2,  "RShoulder"},   肩
    //     {3,  "RElbow"},      肘
    //     {4,  "RWrist"},      手腕
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
