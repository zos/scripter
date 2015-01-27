#-------------------------------------------------
#
# Project created by QtCreator 2015-01-18T12:31:15
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Scripter
TEMPLATE = app


SOURCES += main.cpp \
        mainwindow.cpp \
        comm/Pipe.cpp \
        jobber/Job.cpp \
        jobber/JobController.cpp \
        jobber/JobDispatcher.cpp \
    nodder/Overlord.cpp \
    comm/TcpSocket.cpp \
    comm/TcpServer.cpp \
    nodder/Nodder.cpp \
    comm/Config.cpp

HEADERS  += mainwindow.h \
        comm/Pipe.h \
        jobber/Job.h \
        log/Log.h \
        jobber/JobController.h \
        jobber/JobDispatcher.h \
    nodder/Overlord.h \
    comm/TcpSocket.h \
    comm/TcpServer.h \
    comm/Protocol.h \
    nodder/Nodder.h \
    comm/Config.h

FORMS    += mainwindow.ui

CONFIG += c++11
