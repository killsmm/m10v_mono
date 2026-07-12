#include "caminfoudpserver.h"
#include <QThread>
#include <QtNetwork/QUdpSocket>
#include <QEventLoop>
#include <socket_report.h>
#include <rtos_control_consts.h>

CamInfoUdpServer::CamInfoUdpServer(QObject *parent) :
    QObject(parent)
{
    this->udpSocket = new QUdpSocket();
    this->report = new union socket_report;
    QObject::connect(udpSocket, SIGNAL(readyRead()), SLOT(tryToReadData()));
}

CamInfoUdpServer::~CamInfoUdpServer()
{
    delete udpSocket;
    delete report;
}

bool CamInfoUdpServer::init(){
    return udpSocket->bind(QHostAddress::Any,
                           SOCKET_BRD_PORT,
                           QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
}

void CamInfoUdpServer::tryToReadData(){

    int recv_len = 0;
    if(udpSocket->hasPendingDatagrams()){
        recv_len = udpSocket->readDatagram((char*)report, sizeof(struct socket_report_face_info));
        if(recv_len < 0){
            qDebug("udpSocket->readDatagram error");
            return;
        }
        if(report->normal.report_id != SOCKET_REPORT_ID){
            qDebug("report id error %x",report->normal.report_id);
            return;
        }
        switch(report->normal.cmd){
        case CMD_CAM_INFO_STREAM_MODE:
            qDebug("CMD_CAM_INFO_STREAM_MODE");
            emit(streamModeChanged(report->normal.params[0],
                    report->normal.params[1],
                    report->normal.params[2]));
            break;
        case CMD_CAM_INFO_FACES:
//            qDebug("CMD_CAM_INFO_FACES");
            emit(faceCountChanged(report->face_info.resolution_x,
                                  report->face_info.resolution_y,
                                  report->face_info.face_count,
                                  report->face_info.face));
            break;
        case CMD_CAM_INFO_TRACING_STATUS:
            qDebug("CMD_CAM_INFO_TRACING_STATUS");
            emit(tracingStatusChanged(report->normal.params[0]));
            break;
        case CMD_CAM_INFO_UPDATE_MODE:
            qDebug("CMD_CAM_INFO_UPDATE_MODE");
            emit(updateModeChanged(report->normal.params[0]));
            break;
        }
    }else{
        qDebug("no data pendding");
    }
}
