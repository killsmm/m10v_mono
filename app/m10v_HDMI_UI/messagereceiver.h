#ifndef MESSAGERECEIVER_H
#define MESSAGERECEIVER_H

#include <QObject>
#include "zmq.hpp"
#include <QThread>
#include <memory>
#include <QMap>

#define MUTIMEDIA_SERVER_MQ "tcp://127.0.0.1:8101"
#define CAMERA_SERVER_MQ "tcp://127.0.0.1:8100"
#define GPS_MQ "ipc:///tmp/gps"

class MessageReceiver : public QObject
{
    Q_OBJECT
public:
    explicit MessageReceiver( QObject *parent = nullptr);
    ~MessageReceiver();
    void start_receive();
    void stop_receive();
    class MsgRecvThread : public QThread {
    public:
        MsgRecvThread(MessageReceiver *receiver, QObject *parent = nullptr);
        void setThreadAbort(bool is_abort);
    private:
        MessageReceiver *receiver;
        void run();
        bool threadAbort;
        zmq::context_t *zmqContext;
    } *msgRecvThread;
signals:
    void workStatusRecived(QString msg);
    void statusMsgReceived(int aa);
    void multimediaMsgReceived(QString msg);
    void gpsMsgReceived(QString msg);
};

#endif // MESSAGERECEIVER_H
