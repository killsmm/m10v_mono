#include "widget.h"
#include "ui_widget.h"
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include "json.h"

#define CAMERA_PIC_PATH ":/res/zhaoxiangji.png"
#define VIDEO_PIC_PATH ":/res/shipinmulushipin.png"

#define SD_STATE_EXIST ":/res/sdcard_state_exist.png"
#define SD_STATE_NO_EXIST ":/res/sdcard_state_nonexist.png"

static void hideResolutionLayout(Ui::Widget *ui){
    ui->label_265_res->hide();
    ui->label_2640_res->hide();
    ui->label_jpeg_res->hide();
    ui->label_2641_res->hide();
    ui->label_14->hide();
    ui->label_15->hide();
    ui->label_16->hide();
    ui->label_17->hide();
    ui->label_18->hide();
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    ui->label_camera_model->setPixmap(QPixmap(CAMERA_PIC_PATH));

    hideResolutionLayout(ui);

    setPalette(QPalette(Qt::green));

    msgReceiver = new MessageReceiver();
    connect(msgReceiver, SIGNAL(workStatusRecived(QString)), this, SLOT(onStatusMsgReceived(QString)));
    connect(msgReceiver, SIGNAL(gpsMsgReceived(QString)), this, SLOT(onGPSMsgReceived(QString)));
    connect(msgReceiver, SIGNAL(multimediaMsgReceived(QString)), this, SLOT(onMultiMediaMsgReceived(QString)));
    msgReceiver->start_receive();
    ChanageSDExistStateSlot(true);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start(1000);
}

Widget::~Widget()
{
    delete ui;
}
void Widget::ChangePicSlot()
{
    static bool flag = false;
    flag = !flag;
    OSDLayerVisibleSlot(flag);
}

void Widget::OSDLayerVisibleSlot(bool hide)
{
//    ui->frame->setVisible(hide);
//    ui->frame_2->setVisible(hide);
//    ui->frame_3->setVisible(hide);
}
void Widget::ChanageSDExistStateSlot(bool exist)
{
     QImage img_mainicon;
    if(exist)
    {
        img_mainicon.load(SD_STATE_EXIST);
    }
    else
    {
        img_mainicon.load(SD_STATE_NO_EXIST);
    }
    img_mainicon.scaled(ui->label_sdcard_exist->size(), Qt::KeepAspectRatio); //
    ui->label_sdcard_exist->setScaledContents(true);
    ui->label_sdcard_exist->setPixmap(QPixmap::fromImage(img_mainicon));
}
void Widget::ChanageSDInfoSlot(int used, int free)
{

}

void Widget::ChanageISOSlot(int iso)
{

}
void Widget::ChanageVersionSlot(QString sver, QString hver)
{

}
void Widget::ChanageCompanyNameSlot(QString name)
{

}

void Widget::showJpegCaptured()
{

}

void Widget::updateTime(){
    ui->label_time->setText(QDate::currentDate().toString("yy-MM-dd") + QTime::currentTime().toString(" hh:mm:ss"));
}

static QString getStringFromJson(QtJson::JsonObject json, QString level_one, QString level_two, QString level_three){
    if(level_three == ""){
        return json[level_one].toMap()[level_two].toString();
    }else {
        return json[level_one].toMap()[level_two].toMap()[level_three].toString();
    }
}

static bool getBoolFromJson(QtJson::JsonObject json, QString level_one, QString level_two, QString level_three){
    if(level_three == ""){
        return json[level_one].toMap()[level_two].toBool();
    }else {
        return json[level_one].toMap()[level_two].toMap()[level_three].toBool();
    }
}

void Widget::onGPSMsgReceived(QString msg){
    bool succeed = false;
    QtJson::JsonObject result = QtJson::parse(msg, succeed).toMap();
    if (succeed){
        if (result["cmd"] == "GPS"){
            ui->label_latitude->setText(getStringFromJson(result, "data", "location", "latitude"));
            ui->label_longitude->setText(getStringFromJson(result, "data", "location", "longitude"));
            ui->label_altitude->setText(getStringFromJson(result, "data", "location", "altitude"));
            ui->label_roll->setText(getStringFromJson(result, "data", "angles", "roll"));
            ui->label_pitch->setText(getStringFromJson(result, "data", "angles", "pitch"));
            ui->label_yaw->setText(getStringFromJson(result, "data", "angles", "yaw"));
            ui->label_vn->setText(getStringFromJson(result, "data", "velocity", "NV"));
            ui->label_ve->setText(getStringFromJson(result, "data", "velocity", "EV"));
            ui->label_vu->setText(getStringFromJson(result, "data", "velocity", "UV"));
        }
    }
}

void Widget::onMultiMediaMsgReceived(QString msg){
    ui->label_message->setText(msg);
}

void Widget::onStatusMsgReceived(QString msg){
    bool succeed = false;
    QtJson::JsonObject result = QtJson::parse(msg, succeed).toMap();
    if (succeed){
        if (result["cmd"] == "workStatus"){
            ui->label_sw_ver->setText(getStringFromJson(result, "data", "Version", "sw_version"));
            ui->label_2640_res->setText(getStringFromJson(result, "data", "format_h264_0", "resolve"));
            ui->label_2641_res->setText(getStringFromJson(result, "data", "format_h264_1", "resolve"));
            ui->label_265_res->setText(getStringFromJson(result, "data", "format_h265", "resolve"));
            ui->label_jpeg_res->setText(getStringFromJson(result, "data", "photo_resolve", ""));
            ui->label_zoom->setText(getStringFromJson(result, "data", "digital_zoom", "ratio"));
            ui->label_zoom_x->setText(getStringFromJson(result, "data", "digital_zoom", "center_pos_x"));
            ui->label_zoom_y->setText(getStringFromJson(result, "data", "digital_zoom", "center_pos_y"));
            ui->label_shutter_speed->setText(getStringFromJson(result, "data", "shutterspeed_value", ""));
            ui->label_ae_mode->setText(getStringFromJson(result, "data", "ae_mode", ""));
            if(getStringFromJson(result, "data", "ae_mode", "") == "Auto"){
                ui->label_iso->setText(getStringFromJson(result, "data", "auto_iso_value", ""));
            }else{
                ui->label_iso->setText(getStringFromJson(result, "data", "manual_iso_value", ""));
            }
            ui->label_pic_captured->setText(getStringFromJson(result, "data", "photo_total", ""));
//            ui->label_pic_remain->setText(getStringFromJson(result, "data", "photo_free", ""));
            ui->label_pic_remain->setText(QString::number(getStringFromJson(result, "data", "sd_free_space", "").toLongLong()/1024/1024/10));
            ui->label_sd_space->setText(QString::number(
                                            getStringFromJson(result, "data", "sd_total_space", "").toDouble()/1024/1024/1024)
                                        );
            ui->label_sd_free->setText(QString::number(
                                            getStringFromJson(result, "data", "sd_free_space", "").toDouble()/1024/1024/1024)
                                        );
            ui->label_sdcard_exist->setVisible(getBoolFromJson(result, "data", "sd_visible", ""));
        }else if (result["cmd"] == "DeviceStatus"){

            ui->label_input_pwr->setText(getStringFromJson(result, "data", "total_voltage", ""));
            bool tf_working = result["data"].toMap()["tf_working"].toBool();
            ui->label_sdcard_exist->setVisible(tf_working);
        }
    }
}
