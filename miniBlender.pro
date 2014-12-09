#-------------------------------------------------
#
# Project created by QtCreator 2014-11-03T14:40:26
#
#-------------------------------------------------

QT       += core gui
QT       += opengl 


LIBS += -lglut32
LIBS += -LC:\glut
#LIBS += -lglew32
LIBS += -lfreeglut 


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = miniBlender
TEMPLATE = app


SOURCES += main.cpp\
    glwidget.cpp \
    OBJ.cpp \
    mainwindow.cpp \
    enity.cpp \
    attributewidget.cpp \
    renamedialog.cpp

HEADERS  += \
    glwidget.h \
    OBJ.h \
    mainwindow.h \
    enity.h \
    attributewidget.h \
    renamedialog.h

FORMS    += \
    glwidget.ui \
    mainwindow.ui \
    attributewidget.ui \
    renamedialog.ui

RESOURCES += \
    Icons.qrc \
    Textures.qrc


