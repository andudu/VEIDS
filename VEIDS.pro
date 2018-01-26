#-------------------------------------------------
#
# Project created by QtCreator 2018-01-22T11:54:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++11

TARGET = VEIDS
TEMPLATE = app

SOURCES += main.cpp\
        VEIDS.cpp \
    gui.cpp \
    openpose.cpp \
    motiondetection.cpp \
    dispatcher.cpp

HEADERS  += VEIDS.h \
    openpose.h \
    motiondetection.h \
    dispatcher.h

FORMS    += VEIDS.ui

LIBS     += -lopenpose -lcaffe -lgflags -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lpthread

RESOURCES += \
    img/img.qrc
