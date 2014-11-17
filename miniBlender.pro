#-------------------------------------------------
#
# Project created by QtCreator 2014-11-03T14:40:26
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

#LIBS += -L E:\Study\ComputerGraphics\qtopengl\build-test1-Desktop_Qt_5_3_0_MSVC2013_OpenGL_64bit-Debug\debug\freeglut.lib

#LIBS += -L E:/Study/ComputerGraphics/qtopengl/test1 -l glut32

LIBS += -lglut32
#LIBS +=-lfreeglut #-L"D:\Qt\Qt5.3.0\5.3\msvc2013_64_opengl\lib\freeglut\lib"


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test1
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


#message("Location of glut32.lib" + glut32)
