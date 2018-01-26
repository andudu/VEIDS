#include "VEIDS.h"
#include "ui_VEIDS.h"
#include <QFileDialog>

VEIDS::VEIDS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VEIDS)
{
    ui->setupUi(this);
    ui->source_comboBox->addItem("摄像头");
    ui->source_comboBox->addItem("视频文件");
    ui->source_comboBox->addItem("Rtsp直播源");
    ui->IMAGE->setScaledContents(true);
    ui->select_pushButton->setVisible(false);
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<msgOutput>("msgOutput");
}

void VEIDS::refreashImg()
{
    cv::Mat picMat;
    cv::cvtColor(openpose->outputImage,picMat,CV_BGR2RGB);
    QImage picQImage = QImage((uchar*) picMat.data, picMat.cols, picMat.rows, QImage::Format_RGB888);
    QPixmap picQPixmap = QPixmap::fromImage(picQImage);
    ui->IMAGE->setPixmap(picQPixmap);
    ui->IMAGE->show();
}

void VEIDS::refreashMsg(msgOutput outputMsg)
{
    ui->number->setText(QString::number(outputMsg.num));
    switch(outputMsg.state)
    {
    case 1:
        ui->state->setText("异常");
        break;
    default:
        ui->state->setText("正常");
        break;
    }
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
    case 0: ui->select_pushButton->setVisible(false);
        ui->source_lineEdit->setText("0");
        break;
    case 2: ui->select_pushButton->setVisible(false);
        ui->source_lineEdit->setText("rtsp://");
        break;
    default: ui->select_pushButton->setVisible(true);
        ui->source_lineEdit->setText("");
        break;
    }
}

void VEIDS::on_select_pushButton_clicked()
{
    ui->source_lineEdit->setText(QFileDialog::getOpenFileUrl(this,tr("Select a file to open...")).toString().mid(7));
}

void VEIDS::on_start_pushButton_clicked()
{
    ui->start_pushButton->setEnabled(false);
    ui->stop_pushButton->setEnabled(true);
    openpose = new OpenPose(ui->source_lineEdit->text().toStdString());
    connect(openpose,SIGNAL(outputDone()),this,SLOT(refreashImg()));
    connect(openpose,SIGNAL(finished()),this,SLOT(openposeDelete()));
    openpose->start();
}

void VEIDS::on_stop_pushButton_clicked()
{
    openpose->want2exit = true;
}

VEIDS::~VEIDS()
{
    delete ui;
}
