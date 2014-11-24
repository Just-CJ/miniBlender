#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QMainWindow>
#include "glwidget.h"

namespace Ui {
class openglWindow;
}

class openglWindow : public QMainWindow
{
    Q_OBJECT

public:
    GLWidget *widget;

    explicit openglWindow(QWidget *parent = 0);
    ~openglWindow();



private slots:
    void on_actionImport_triggered();

    void on_WireFrame_BTN_clicked();

    void on_Light_BTN_clicked();

    void on_actionExport_triggered();

    void InitSpinBox();

    void on_doubleSpinBox_valueChanged(double rotate_x);

    void on_doubleSpinBox_3_valueChanged(double rotate_z);

    void on_doubleSpinBox_2_valueChanged(double rotate_y);

private:
    Ui::openglWindow *ui;
};

#endif // OPENGLWINDOW_H
