#include "openglwindow.h"
#include "glwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    glutInit(&argc, argv); //initialize glut
    QApplication a(argc, argv);
    openglWindow w;
    //GLWidget w;
    w.show();

    return a.exec();
}
