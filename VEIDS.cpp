#include "VEIDS.h"
#include "ui_VEIDS.h"
#include <QFileDialog>
#include <QSound>
#include <QMessageBox>
#include <QKeyEvent>

VEIDS::VEIDS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VEIDS)
{
    ui->setupUi(this);
    ui->source_comboBox->addItem("视频文件");
    ui->source_comboBox->addItem("摄像头");
    ui->source_comboBox->addItem("Rtsp直播源");
    ui->IMAGE->setScaledContents(true);
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<msgOutput>("msgOutput");
}

void VEIDS::refreashImg()
{
    if(openpose->outputImage.empty()) return;
    cv::Mat picMat;
    cv::cvtColor(openpose->outputImage,picMat,CV_BGR2RGB);
    QImage picQImage = QImage((uchar*) picMat.data, picMat.cols, picMat.rows, QImage::Format_RGB888);
    QPixmap picQPixmap = QPixmap::fromImage(picQImage);
    ui->IMAGE->setPixmap(picQPixmap);
    ui->IMAGE->show();
}

void VEIDS::refreashMsg()
{
    if(detector->outputImage.empty()) return;
    ui->number->setText(QString::number(detector->result.num));

    if(detector->result.state & 1)
        ui->abnormity_1->setText("总人数过多");
    else
        ui->abnormity_1->setText("");

    if(detector->result.state & 2)
    {
        ui->abnormity_2->setText("过分聚集");
//        QSound::play(":/new/sound/alarm.wav");
    }
    else
        ui->abnormity_2->setText("");

    if(detector->result.state & 4)
        ui->abnormity_3->setText("手举过肩");
    else
        ui->abnormity_3->setText("");

    if(detector->result.state & 8)
        ui->abnormity_4->setText("有人倒地");
    else
        ui->abnormity_4->setText("");

    if(detector->result.state & 16)
        ui->abnormity_5->setText("异常五");
    else
        ui->abnormity_5->setText("");

    cv::Mat picMat;
    cv::cvtColor(detector->outputImage,picMat,CV_BGR2RGB);
    QImage picQImage = QImage((uchar*) picMat.data, picMat.cols, picMat.rows, QImage::Format_RGB888);
    QPixmap picQPixmap = QPixmap::fromImage(picQImage);
    ui->IMAGE_2->setPixmap(picQPixmap);
    ui->IMAGE_2->show();
}

void VEIDS::openposeDelete()
{
    ui->start_pushButton->setEnabled(true);
    ui->stop_pushButton->setEnabled(false);
    openpose->terminate();
    openpose->wait();
    delete openpose;
}

void VEIDS::on_source_comboBox_currentIndexChanged(int index)
{
    switch(index){
    case 1: ui->select_pushButton->setVisible(false);
        ui->source_lineEdit->setText("0");
        break;
    case 2: ui->select_pushButton->setVisible(false);
        ui->source_lineEdit->setText("rtsp://");
        break;
    default: ui->select_pushButton->setVisible(true);
        ui->source_lineEdit->setText("/home/wiki/Project/Gang/GangVideo_Edit/15_f.mp4");
        break;
    }
}

void VEIDS::on_select_pushButton_clicked()
{
    QString path = QFileDialog::getOpenFileUrl(this,tr("Select a file to open...")).toString();
    if(path.length() != 0)
        ui->source_lineEdit->setText(path.mid(7));
}

void VEIDS::on_start_pushButton_clicked()
{
    if(ui->source_lineEdit->text().size() == 0)
    {
        QMessageBox::warning(this,"路径错误","这不是一个有效的路径，请重新选择文件",QMessageBox::Yes);
    }
    ui->start_pushButton->setEnabled(false);
    ui->stop_pushButton->setEnabled(true);
    openpose = new OpenPose(ui->source_lineEdit->text().toStdString());
    detector = openpose->detector;
    connect(openpose,SIGNAL(outputDone()),this,SLOT(refreashImg()));
    connect(openpose,SIGNAL(finished()),this,SLOT(openposeDelete()));
    connect(detector,SIGNAL(outputDone()),this,SLOT(refreashMsg()));
    openpose->start();
}

void VEIDS::on_stop_pushButton_clicked()
{
    openpose->want2exit = true;
}

void VEIDS::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
    else if(event->key() == Qt::Key_Space)
    {

    }
}

VEIDS::~VEIDS()
{
    delete ui;
}
