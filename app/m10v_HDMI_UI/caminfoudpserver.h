#ifndef CAMINFOUDPSERVER_H
#define CAMINFOUDPSERVER_H

#define SOCKET_BRD_PORT 6001
#define MAX_DATA_LEN    50

#include <QtNetwork/QUdpSocket>
#include <QThread>
#include <QLabel>
#include <socket_report.h>

class CamInfoUdpServer : public QObject
{
    Q_OBJECT
public:
    CamInfoUdpServer(QObject *parent = 0);
    ~CamInfoUdpServer();
    bool init();
public slots:
    void tryToReadData();
signals:
    void faceCountChanged(int resolution_x,
                          int resolution_y,
                          int face_count,
                          struct face_pos *face);
    void streamModeChanged(unsigned char stream_mode,
                           unsigned char zone_left,
                           unsigned char zone_right);
    void tracingStatusChanged(unsigned char status);
    void updateModeChanged(bool update_mode);
private:
    QUdpSocket *udpSocket;
    union socket_report *report;
};

#endif // CAMINFOUDPSERVER_H
