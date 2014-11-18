#-------------------------------------------------
#
# Project created by QtCreator 2014-11-03T14:40:26
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

LIBS += -lglut32



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = miniBlender
TEMPLATE = app


SOURCES += main.cpp\
        openglwindow.cpp \
    glwidget.cpp \
    OBJ.cpp

HEADERS  += openglwindow.h \
    glwidget.h \
    OBJ.h

FORMS    += openglwindow.ui \
    glwidget.ui


