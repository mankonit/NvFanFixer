#-------------------------------------------------
#
# Project created by QtCreator 2017-03-17T21:28:37
#
#-------------------------------------------------

QT       += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NvFanFixer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/nvinterface.cpp \
    src/fanrevvingdetector.cpp \
    src/singleapplication.cpp

HEADERS  += src/mainwindow.h \
    src/nvinterface.h \
    src/fanrevvingdetector.h \
    src/singleapplication.h

FORMS    += src/mainwindow.ui

LIBS += -L$$PWD/lib/nvapi/amd64/ -lnvapi64

INCLUDEPATH += $$PWD/lib/nvapi/amd64
DEPENDPATH += $$PWD/lib/nvapi/amd64

PRE_TARGETDEPS += $$PWD/lib/nvapi/amd64/nvapi64.lib

RESOURCES += \
    res.qrc

DISTFILES += \
    howto.txt

RC_ICONS = res/nv1sq_green.ico

