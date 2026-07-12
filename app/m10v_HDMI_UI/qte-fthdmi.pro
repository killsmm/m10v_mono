#-------------------------------------------------
#
# Project created by QtCreator 2022-03-07T08:32:06
#
#-------------------------------------------------

QT += core gui
QT += network
#QT += websockets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lzmq -lsodium
LIBS += -L/opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/usr/lib
INCLUDEPATH += /opt/poky/2.2/sysroots/cortexa7-poky-linux-gnueabi/usr/include

TARGET = qte-fthdmi
TEMPLATE = app




# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    json.cpp \
    main.cpp \
    widget.cpp \
    messagereceiver.cpp

HEADERS += \
    json.h \
    messagereceiver.h \
    widget.h \
    messagereceiver.h

FORMS += \
        widget.ui

RESOURCES += \
    resources.qrc

target.path = /run/SD/mmcblk0p1
INSTALLS += target
