#-------------------------------------------------
#
# Project created by QtCreator 2016-01-08T12:38:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets bluetooth

TARGET = BT_test
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    chatclient.cpp \
    chatserver.cpp \
    lsm9ds0.cpp

HEADERS  += dialog.h \
    chatclient.h \
    chatserver.h \
    lsm9ds0.h

FORMS    += dialog.ui
