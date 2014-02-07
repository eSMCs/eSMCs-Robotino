#-------------------------------------------------
#
# Project created by QtCreator 2010-10-27T16:43:40
#
#-------------------------------------------------

QMAKEFEATURES = /usr/local/qwt-6.0.0/

CONFIG   += qwt
QT       += network
#LIBS     += -Wl,-rpath,/usr/local/OpenRobotinoAPI/1/lib/linux -L/usr/local/OpenRobotinoAPI/1/lib/linux -lrec_robotino_com
#INCLUDEPATH += /usr/local/OpenRobotinoAPI/1/include

LIBS     += -Wl,-rpath,/mnt/LaCie/Alex/OpenRobotinoAPI64/1/lib/linux -L/usr/local/qwt-6.0.0/lib -lqwt -L/mnt/LaCie/Alex/OpenRobotinoAPI64/1/lib/linux -lrec_robotino_com `pkg-config --libs opencv`
#LIBS     += -Wl,-rpath,/home/maye/OpenRobotinoAPI/trunk/install/usr/local/OpenRobotinoAPI/1/lib/linux -L/usr/local/qwt-6.0.0/lib -lqwt -L/home/maye/OpenRobotinoAPI/trunk/install/usr/local/OpenRobotinoAPI/1/lib/linux -lrec_robotino_com

INCLUDEPATH += /mnt/LaCie/Alex/OpenRobotinoAPI64/1/include /usr/local/qwt-6.0.0/include /usr/include/opencv
#QMAKE_CXXFLAGS += -Dlinux32

TARGET = robotino1
TEMPLATE = app
QMAKE_CXXFLAGS += -fPIC
CMAKE_CFLAGS += -fPIC

SOURCES += main.cpp\
        mainwindow.cpp \
    robotinowindow.cpp \
    distancesensorview.cpp \
    accelerometerview.cpp \
    robotinoinfo.cpp \
    powerview.cpp \
    robotino.cpp \
    robotinocamera.cpp \
    robotinoaudio.cpp \
    audioview.cpp \
    robotinofeaturewindow.cpp \
    cameraview.cpp \
    smcwindow.cpp \
    smcthread.cpp \
    actionobservationview.cpp \
    imageseqview.cpp \
    actionvalueview.cpp

HEADERS  += mainwindow.h \
    robotinowindow.h \
    distancesensorview.h \
    accelerometerview.h \
    robotinoinfo.h \
    powerview.h \
    robotino.h \
    robotinocamera.h \
    robotinoaudio.h \
    audioview.h \
    robotinofeaturewindow.h \
    cameraview.h \
    smcwindow.h \
    smcthread.h \
    actionobservationview.h \
    imageseqview.h \
    actionvalueview.h

FORMS    += mainwindow.ui \
    robotinowindow.ui \
    robotinofeaturewindow.ui \
    smcwindow.ui

RESOURCES += \
    robotino-ressources.qrc
