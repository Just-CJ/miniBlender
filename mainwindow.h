#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"
#include "attributewidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GLWidget *widget;
    attributeWidget *attr;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void objectSubmit();

private slots:
    void initSpinBoxAndSlider();

    void selectedAttribute();

    void reshapeEntity();

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

    void on_actionAdd_cube_triggered();

    void on_actionAdd_prism_triggered();

    void on_actionAdd_prismoid_triggered();

    void on_actionAdd_sphere_triggered();

    void on_actionScreen_Capture_triggered();

    void on_actionAdd_cylinder_triggered();

    void on_actionAdd_cone_triggered();

    void on_actionExport_OBJ_File_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
