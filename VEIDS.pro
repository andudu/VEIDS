#-------------------------------------------------
#
# Project created by QtCreator 2018-01-22T11:54:23
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++11

TARGET = VEIDS
TEMPLATE = app

SOURCES += main.cpp\
        VEIDS.cpp \
    gui.cpp \
    openpose.cpp \
    motiondetection.cpp

HEADERS  += VEIDS.h \
    openpose.h \
    motiondetection.h

FORMS    += VEIDS.ui

QMAKE_CXXFLAGS += -fopenmp

LIBS     += -lopenpose -lcaffe -lgflags -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lpthread -fopenmp

RESOURCES += \
    img/img.qrc \
    sound/sound.qrc
