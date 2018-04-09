#ifndef VEIDS_H
#define VEIDS_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include "motiondetection.h"
#include "openpose.h"

namespace Ui {
class VEIDS;
}

class VEIDS : public QMainWindow
{
    Q_OBJECT
public:
    explicit VEIDS(QWidget *parent = 0);
    ~VEIDS();
private:
    Ui::VEIDS *ui;
    void keyPressEvent(QKeyEvent *event);
    OpenPose *openpose;
    MotionDetection *detector;
    int openPoseTutorialWrapper1();
    QPixmap cvMat2QPixmap(const cv::Mat& src);
private slots:
    void refreashImg();
    void refreashMsg();
    void openposeDelete();
    void on_source_comboBox_currentIndexChanged(int index);
    void on_select_pushButton_clicked();
    void on_start_pushButton_clicked();
    void on_stop_pushButton_clicked();
};

#endif // VEIDS_H
