#include "messagereceiver.h"
#include "QtDebug"
#include "json.h"

MessageReceiver::MsgRecvThread::MsgRecvThread(MessageReceiver *receiver, QObject *parent){
    this->receiver = receiver;
}

void MessageReceiver::MsgRecvThread::run(){
    static int count = 0;
    zmqContext = new zmq::context_t();

    zmq::socket_t zmqMultiMediaSocket(*zmqContext, zmq::socket_type::sub);
    zmqMultiMediaSocket.connect(MUTIMEDIA_SERVER_MQ);
    char filter[] = "jpeg";
    zmqMultiMediaSocket.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

    zmq::socket_t zmqSocketCameraServer(*zmqContext, zmq::socket_type::sub);
    zmqSocketCameraServer.connect(CAMERA_SERVER_MQ);
    char filter2[] = "";
    zmqSocketCameraServer.setsockopt(ZMQ_SUBSCRIBE, filter2, strlen(filter2));

    zmq::socket_t zmqSocketGPS(*zmqContext, zmq::socket_type::sub);
    zmqSocketGPS.connect(GPS_MQ);
    char filter3[] = "";
    zmqSocketGPS.setsockopt(ZMQ_SUBSCRIBE, filter3, strlen(filter3));

    zmq::message_t msg(2048);
    while (!this->threadAbort) {
        if(zmqSocketCameraServer.recv(&msg, ZMQ_DONTWAIT)){
            emit receiver->workStatusRecived(QString((char*)msg.data())) ;
        }
        if(zmqMultiMediaSocket.recv(&msg, ZMQ_DONTWAIT)){
            emit receiver->multimediaMsgReceived(QString((char*)msg.data()));
        }
        if(zmqSocketGPS.recv(&msg, ZMQ_DONTWAIT)){
            if(count > 50){
                emit receiver->gpsMsgReceived(QString((char*)msg.data()));
                count = 0;
            }
            count++;
        }
    }
    zmqMultiMediaSocket.close();
    zmqSocketCameraServer.close();
    zmqSocketGPS.close();
    zmqContext->close();
}

void MessageReceiver::MsgRecvThread::setThreadAbort(bool is_abort){
    this->threadAbort = is_abort;
}

MessageReceiver::MessageReceiver(QObject *parent) : QObject(parent)
{
    msgRecvThread = new MsgRecvThread(this);
}

MessageReceiver::~MessageReceiver()
{
    this->stop_receive();
    delete msgRecvThread;
}


void MessageReceiver::start_receive()
{  
    msgRecvThread->setThreadAbort(FALSE);
    msgRecvThread->start();
}

void MessageReceiver::stop_receive()
{
    msgRecvThread->setThreadAbort(TRUE);
    msgRecvThread->wait();
}


