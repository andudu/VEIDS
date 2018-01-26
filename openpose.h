#ifndef OPENPOSE_H
#define OPENPOSE_H

#include <QObject>
#include <QThread>
// OpenPose dependencies
#include <openpose/headers.hpp>

// If the user needs his own variables, he can inherit the op::Datum struct and add them
// UserDatum can be directly used by the OpenPose wrapper because it inherits from op::Datum, just define
// Wrapper<UserDatum> instead of Wrapper<op::Datum>
struct UserDatum : public op::Datum
{
    bool boolThatUserNeedsForSomeReason;

    UserDatum(const bool boolThatUserNeedsForSomeReason_ = false) :
        boolThatUserNeedsForSomeReason{boolThatUserNeedsForSomeReason_}
    {}
};

class OpenPose : public QThread
{
    Q_OBJECT
public:
    OpenPose();
    ~OpenPose();
    cv::Mat inputImage;
    cv::Mat outputImage;
    op::Array<float> poseKeypoints;
private:
    op::ScaleAndSizeExtractor *scaleAndSizeExtractor;
protected:
    void run();
signals:
    void outputDone();
};

#endif // OPENPOSE_H
