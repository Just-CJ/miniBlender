#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
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
    int textureNumber;
    GLuint texture[10];
    int selectedLID;
    bool selectedLight;
    ~MainWindow();

signals:
    void objectSubmit(bool EntityOrObject);

    void sendSelectOBJ(unsigned int SelectID);

private slots:
    void initSpinBoxAndSlider();

    void selectedAttribute();

    void reshapeEntity();

    void getSelectedItem();

    void on_actionImport_OBJ_File_triggered();

    void on_actionWire_Solid_triggered();

    void on_actionAdd_light_triggered();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_doubleSpinBox_valueChanged(double rotate_x);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_doubleSpinBox_3_valueChanged(double arg1);

    void updateCatalog(bool EntityOrObject);

    void initialCatalog();

    void on_actionAdd_cube_triggered();

    void on_actionAdd_prism_triggered();

    void on_actionAdd_prismoid_triggered();

    void on_actionAdd_sphere_triggered();

    void on_actionScreen_Capture_triggered();

    void on_actionAdd_cylinder_triggered();

    void on_actionAdd_cone_triggered();

    void on_actionExport_OBJ_File_triggered();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_actionSelect_triggered();

    void on_actionDelete_triggered();

    void updateAttribute(unsigned int selectedID);

    void on_display_x_valueChanged(double offset_x);

    void on_display_y_valueChanged(double offset_y);

    void on_display_z_valueChanged(double offset_z);

    void on_display_scale_x_valueChanged(double scale_x);

    void on_display_scale_y_valueChanged(double scale_y);

    void on_display_scale_z_valueChanged(double scale_z);

    void on_actionOpen_Project_triggered();

    void on_actionSave_Project_triggered();

    void on_tabWidget_tabBarClicked(int index);

    void on_ambient1_valueChanged(double arg1);

    void on_ambient2_valueChanged(double arg1);

    void on_ambient3_valueChanged(double arg1);

    void on_diffuse1_valueChanged(double arg1);

    void on_diffuse2_valueChanged(double arg1);

    void on_diffuse3_valueChanged(double arg1);

    void on_specular1_valueChanged(double arg1);

    void on_specular2_valueChanged(double arg1);

    void on_specular3_valueChanged(double arg1);

    void on_emission1_valueChanged(double arg1);

    void on_emission2_valueChanged(double arg1);

    void on_emission3_valueChanged(double arg1);

    void on_lambient1_valueChanged(double arg1);

    void on_lambient2_valueChanged(double arg1);

    void on_lambient3_valueChanged(double arg1);

    void on_ldiffuse1_valueChanged(double arg1);

    void on_ldiffuse2_valueChanged(double arg1);

    void on_ldiffuse3_valueChanged(double arg1);

    void on_lspecular1_valueChanged(double arg1);

    void on_lspecular2_valueChanged(double arg1);

    void on_lspecular3_valueChanged(double arg1);

    void on_lposition1_valueChanged(double arg1);

    void on_lposition2_valueChanged(double arg1);

    void on_lposition3_valueChanged(double arg1);

    void on_intensity_valueChanged(double arg1);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
