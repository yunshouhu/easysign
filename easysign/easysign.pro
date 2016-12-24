#-------------------------------------------------
#
# Project created by QtCreator 2016-12-23T14:23:02
#
#-------------------------------------------------


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = easy_sign
TEMPLATE = app
#æ≤Ã¨±‡“Î
QMAKE_LFLAGS  = -static

RC_FILE = app.rc


SOURCES += main.cpp\
        dialog.cpp \
    mythread.cpp

HEADERS  += dialog.h \
    mythread.h

FORMS    += dialog.ui

RESOURCES += \
    easysign.qrc
