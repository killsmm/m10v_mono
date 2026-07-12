#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "messagereceiver.h"
//typedef enum {
//   CameraModel_Camera,
//   CameraModel_Video,

//}CameraModel;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    MessageReceiver *msgReceiver;
    void ChangePicSlot(void);

    void OSDLayerVisibleSlot(bool hide);

    void ChanageSDExistStateSlot(bool exist);
    void ChanageSDInfoSlot(int used, int free);
//    void ChanageCameraModeSlot(CameraMode model);
    void ChanageVersionSlot(QString sver, QString hver);
    void ChanageISOSlot(int iso);
    void ChanageCompanyNameSlot(QString name);
private slots:
//    void UpdateOSDLayerSlot( EVENT_CMD_E event_cmd, void* arg );
    void onStatusMsgReceived(QString msg);
    void onGPSMsgReceived(QString msg);
    void onMultiMediaMsgReceived(QString msg);
    void showJpegCaptured();
    void updateTime();
};

#endif // WIDGET_H
