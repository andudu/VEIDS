#include "VEIDS.h"
#include <QImage>
#include <QPixmap>

QPixmap VEIDS::cvMat2QPixmap(const cv::Mat& src)
{
    cv::Mat temp; // make the same cv::Mat
    cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
    QImage qimg((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    qimg.bits(); // enforce deep copy, see documentation
    // of QImage::QImage ( const uchar * data, int width, int height, Format format )
    QPixmap dst; dst.fromImage(qimg);
    return dst;
}
