#-------------------------------------------------
#
# Project created by QtCreator 2014-10-18T11:57:23
#
#-------------------------------------------------

QT       += core gui network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = InsteonCommander
TEMPLATE = app


SOURCES += main.cpp mainwindow.cpp Device.cpp Insteon.cpp\
    devicecontrol.cpp

HEADERS  += mainwindow.h \
    devicecontrol.h

FORMS    += mainwindow.ui \
    devicecontrol.ui

RESOURCES += \
    InsteonCommander.qrc

ICON = power.icns
