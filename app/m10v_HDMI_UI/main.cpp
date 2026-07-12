#include "widget.h"
#include <QApplication>
#include <QObject>
#include <QTextCodec>
#include <QSplashScreen>
#include <QPixmap>
#include <QDateTime>
#include <QThread>
#include "QWSServer"
#include <QTimer>
#include <stdlib.h>
#include <QDebug>


int main(int argc, char *argv[])
{
    QWSServer::setBackground(QBrush(Qt::transparent)); //make the framebuffer transparent
//    QWSServer::screenSaverActivate(true);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    QTextCodec::setCodecForTr(codec);

    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QApplication a(argc, argv);

//    QPixmap pixmap("/home/book/work/estone/company_logo.jpg");
//    QSplashScreen* splash = new QSplashScreen(pixmap);
//    splash->resize(QSize(1920,1080));
////    splash->show();
//    a.processEvents();

//    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
//    splash->showMessage(QObject::tr("主界面初始化中..."),topRight, Qt::white);//显示信息

    Widget w;
//    external external("192.168.0.0");
//    QObject::connect(&external, SIGNAL(UpdateOSDLayer(EVENT_CMD_E,void*)), &w,SLOT(UpdateOSDLayerSlot(EVENT_CMD_E,void*)));
    qDebug() << "show widget";
    w.show();



//    QDateTime n=QDateTime::currentDateTime();
//    QDateTime now;
//    do{
//        now=QDateTime::currentDateTime();
//    } while (n.secsTo(now)<=2);//3为需要延时的秒数
//    splash->finish(&w);

    return a.exec();
}
