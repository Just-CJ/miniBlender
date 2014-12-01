#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GLWidget *widget;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void initSpinBoxAndSlider();

    void on_actionImport_OBJ_File_triggered();

    void on_actionWire_Solid_triggered();

    void on_actionLight_On_Off_triggered();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_doubleSpinBox_valueChanged(double rotate_x);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_doubleSpinBox_3_valueChanged(double arg1);

    void updateCatalog();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
