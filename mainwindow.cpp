#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"
#include "attributewidget.h"
#include "enity.h"
#include "OBJ.h"
#include <QFileDialog>
#include <vector>
#include <QDebug>

using namespace std;
extern vector<model> models;
struct attribute{
    QString modelName;
    int modelIndex;
};
vector<attribute> objAttr;
vector<attribute> Attr;
//用于TreeView右键菜单
int indexCounter;
QTreeWidgetItem* curItem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    widget = new GLWidget();
    widget->setObjectName(QStringLiteral("opengl"));
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->verticalLayout_3->addWidget(widget);
    widget->setFocusPolicy(Qt::ClickFocus);

    attr = new attributeWidget();
    ui->verticalLayout_4->addWidget(attr);
    initialCatalog();
    indexCounter = 0;

    connect(widget, SIGNAL(model_select()), this, SLOT(initSpinBoxAndSlider()));
    connect(widget, SIGNAL(model_select()), this, SLOT(selectedAttribute()));
    connect(widget, SIGNAL(model_select()), this, SLOT(getSelectedItem()));
    connect(widget, SIGNAL(signal_updateAttr(uint)), this, SLOT(updateAttribute(uint)));
    connect(attr, SIGNAL(reshape()), this, SLOT(reshapeEntity()));
    connect(this, SIGNAL(objectSubmit(bool)), this, SLOT(updateCatalog(bool)));
    connect(this, SIGNAL(sendSelectOBJ(unsigned int)), widget, SLOT(modelSelect(unsigned int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getSelectedItem()
{
    int id = widget->selectedID-1;
    if(id < 0){
        curItem = NULL;
        return;
    }
    for(unsigned int i = 0; i< objAttr.size(); i++){
        if(objAttr[i].modelIndex == id){
            QTreeWidgetItem *parent = ui->treeWidget->topLevelItem(0);
            curItem = parent->child(i);
        }
    }
    for(unsigned int i = 0; i< Attr.size(); i++){
        if(Attr[i].modelIndex == id){
            QTreeWidgetItem *parent = ui->treeWidget->topLevelItem(1);
            curItem = parent->child(i);
        }
    }
}

void MainWindow::on_actionImport_OBJ_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("open file"),
                                                    "E:/Study/ComputerGraphics/qtopengl/OBJ",
                                                    tr("OBJ file(*.obj)"));
    if(fileName != ""){
      widget->initOBJ();
      loadOBJ(fileName.toStdString().c_str());
      models.back().file = fileName;
      widget->updateGL();

      emit objectSubmit(false);
    }
}

void MainWindow::on_actionWire_Solid_triggered()
{
    if(widget->isWireframe){
        widget->isWireframe = false;
      }
    else{
        widget->isWireframe = true;
      }
}

void MainWindow::on_actionLight_On_Off_triggered()
{
    if(widget->enableLight){
        widget->enableLight = false;
      }
    else{
        widget->enableLight = true;
      }
}

void MainWindow::initSpinBoxAndSlider()
{
    ui->doubleSpinBox->setValue(0.0);
    ui->doubleSpinBox_2->setValue(0.0);
    ui->doubleSpinBox_3->setValue(0.0);
    ui->horizontalSlider->setValue(0);
    ui->horizontalSlider_2->setValue(0);
    ui->horizontalSlider_3->setValue(0);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->doubleSpinBox->setValue(value);
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    ui->doubleSpinBox_2->setValue(value);
}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    ui->doubleSpinBox_3->setValue(value);
}

void MainWindow::on_doubleSpinBox_valueChanged(double rotate_x)
{
    if(widget->selectedID){
        if(ui->doubleSpinBox_2->text().toFloat() != 0.0){
            glPushMatrix();
            glLoadMatrixf(widget->lastRotateMatrix);
            glRotatef(ui->doubleSpinBox_2->text().toFloat(), 0, 1, 0);
            glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
            glPopMatrix();
            for(int i=0; i<16; i++)
              widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
            disconnect(ui->doubleSpinBox_2, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_2_valueChanged(double)));
            ui->doubleSpinBox_2->setValue(0.0);
            ui->horizontalSlider_2->setValue(0);
            connect(ui->doubleSpinBox_2, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_2_valueChanged(double)));
          }
        if(ui->doubleSpinBox_3->text().toFloat() != 0.0){
            glPushMatrix();
            glLoadMatrixf(widget->lastRotateMatrix);
            glRotatef(ui->doubleSpinBox_3->text().toFloat(), 0, 0, 1);
            glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
            glPopMatrix();
            for(int i=0; i<16; i++)
              widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
            disconnect(ui->doubleSpinBox_3, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_3_valueChanged(double)));
            ui->doubleSpinBox_3->setValue(0.0);
            ui->horizontalSlider_3->setValue(0);
            connect(ui->doubleSpinBox_3, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_3_valueChanged(double)));
          }
        glPushMatrix();
        glLoadMatrixf(widget->lastRotateMatrix);
        glRotatef(rotate_x, 1, 0, 0);
        glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
        glPopMatrix();
      }
}

void MainWindow::on_doubleSpinBox_2_valueChanged(double rotate_y)
{
    if(widget->selectedID){
          if(ui->doubleSpinBox->text().toFloat() != 0.0){
              glPushMatrix();
              glLoadMatrixf(widget->lastRotateMatrix);
              glRotatef(ui->doubleSpinBox->text().toFloat(), 1, 0, 0);
              glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
              glPopMatrix();
              for(int i=0; i<16; i++)
                widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
              disconnect(ui->doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_valueChanged(double)));
              ui->doubleSpinBox->setValue(0.0);
              ui->horizontalSlider->setValue(0);
              connect(ui->doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_valueChanged(double)));
            }
          if(ui->doubleSpinBox_3->text().toFloat() != 0.0){
              glPushMatrix();
              glLoadMatrixf(widget->lastRotateMatrix);
              glRotatef(ui->doubleSpinBox_3->text().toFloat(), 0, 0, 1);
              glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
              glPopMatrix();
              for(int i=0; i<16; i++)
                widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
              disconnect(ui->doubleSpinBox_3, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_3_valueChanged(double)));
              ui->doubleSpinBox_3->setValue(0.0);
              ui->horizontalSlider_3->setValue(0);
              connect(ui->doubleSpinBox_3, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_3_valueChanged(double)));
            }
          glPushMatrix();
          glLoadMatrixf(widget->lastRotateMatrix);
          glRotatef(rotate_y, 0, 1, 0);
          glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
          glPopMatrix();
        }
}

void MainWindow::on_doubleSpinBox_3_valueChanged(double rotate_z)
{
    if(widget->selectedID){
          if(ui->doubleSpinBox_2->text().toFloat() != 0.0){
              glPushMatrix();
              glLoadMatrixf(widget->lastRotateMatrix);
              glRotatef(ui->doubleSpinBox_2->text().toFloat(), 0, 1, 0);
              glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
              glPopMatrix();
              for(int i=0; i<16; i++)
                widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
              disconnect(ui->doubleSpinBox_2, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_2_valueChanged(double)));
              ui->doubleSpinBox_2->setValue(0.0);
              ui->horizontalSlider_2->setValue(0);
              connect(ui->doubleSpinBox_2, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_2_valueChanged(double)));
            }
          if(ui->doubleSpinBox->text().toFloat() != 0.0){
              glPushMatrix();
              glLoadMatrixf(widget->lastRotateMatrix);
              glRotatef(ui->doubleSpinBox->text().toFloat(), 1, 0, 0);
              glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
              glPopMatrix();
              for(int i=0; i<16; i++)
                widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
              disconnect(ui->doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_valueChanged(double)));
              ui->doubleSpinBox->setValue(0.0);
              ui->horizontalSlider->setValue(0);
              connect(ui->doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_doubleSpinBox_valueChanged(double))
                      );
            }
          glPushMatrix();
          glLoadMatrixf(widget->lastRotateMatrix);
          glRotatef(rotate_z, 0, 0, 1);
          glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
          glPopMatrix();
        }
}

void MainWindow::initialCatalog()
{
    ui->treeWidget->clear();
    QTreeWidgetItem *Objects = new QTreeWidgetItem(QStringList()<<"Objects");
    ui->treeWidget->addTopLevelItem(Objects);
    QTreeWidgetItem *Entities = new QTreeWidgetItem(QStringList()<<"Entities");
    ui->treeWidget->addTopLevelItem(Entities);
}

void MainWindow::updateCatalog(bool EntityOrObject)
{
    if(EntityOrObject == true){
        QTreeWidgetItem *topItem = ui->treeWidget->topLevelItem(1);
        QString name = "entity" + QString::number(Attr.size()+1);
        attribute ent;
        ent.modelName = name;
        ent.modelIndex = indexCounter;
        Attr.push_back(ent);
        QTreeWidgetItem *newObj = new QTreeWidgetItem(QStringList()<<name);
        topItem->addChild(newObj);

        indexCounter++;
    }
    else{
        QTreeWidgetItem *topItem = ui->treeWidget->topLevelItem(0);
        QString name = "object" + QString::number(objAttr.size()+1);
        attribute obj;
        obj.modelName = name;
        obj.modelIndex = indexCounter;
        objAttr.push_back(obj);
        QTreeWidgetItem *newObj = new QTreeWidgetItem(QStringList()<<name);
        topItem->addChild(newObj);

        indexCounter++;
    }
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parent = item->parent();
    if(parent == NULL)
        return;
    int index = parent->indexOfChild(item);
    int currentModelID;
    if(parent->text(column) == "Objects")
       currentModelID = objAttr[index].modelIndex;
    else
       currentModelID = Attr[index].modelIndex;
//    qDebug()<<currentModelID;

    emit sendSelectOBJ(currentModelID + 1);
}


void MainWindow::on_actionAdd_cube_triggered()
{
    model Cube = createCube();
    Cube.type = CUBE;
    models.push_back(Cube);

    emit objectSubmit(true);
}

void MainWindow::on_actionAdd_prism_triggered()
{
    Prismoid prismoid(3, 50, 50, 50);
    model P = prismoid.createPrismoid();
    P.type = PRISMOID;
    models.push_back(P);

    emit objectSubmit(true);
}

void MainWindow::on_actionAdd_prismoid_triggered()
{
    Prismoid prismoid;
    model P = prismoid.createPrismoid();
    P.type = PRISMOID;
    models.push_back(P);

    emit objectSubmit(true);
}

void MainWindow::on_actionAdd_sphere_triggered()
{
    Sphere sphere;
    model s = sphere.createSphere();
    s.type = SPHERE;
    models.push_back(s);

    emit objectSubmit(true);
}

void MainWindow::on_actionAdd_cylinder_triggered()
{
    Prismoid prismoid(50,50,50,50);
    model P = prismoid.createPrismoid();
    P.type = CYLINDER;
    models.push_back(P);

    emit objectSubmit(true);
}

void MainWindow::on_actionAdd_cone_triggered()
{
    Cone cone;
    model C = cone.createCone();
    C.type = CONE;
    models.push_back(C);

    emit objectSubmit(true);
}

void MainWindow::selectedAttribute()
{
    if(widget->selectedID){
        switch(models[widget->selectedID-1].type){
        case NOT:
            attr->viewable(0, 1, 0, 0, 0, 0, 0, 0, 0);
            break;
        case CUBE:
            attr->viewable(0, 0, 1, 0, 0, 0, 0, 1, 0);
            attr->RLabel->setText("边长");
            attr->R->setValue(models[widget->selectedID-1].entityAttr[0]);
            break;
        case PRISMOID:
            attr->viewable(0, 0, 1, 1, 1, 1, 1, 0, 0);
            attr->faceNum->setValue(models[widget->selectedID-1].entityAttr[0]);
            attr->faceNumLabel->setText("侧面数");
            attr->faceNum->setMinimum(0);
            attr->height->setValue(models[widget->selectedID-1].entityAttr[1]);
            attr->botR->setValue(models[widget->selectedID-1].entityAttr[2]);
            attr->bottomRLabel->setText("底面外接圆半径");
            attr->topR->setValue(models[widget->selectedID-1].entityAttr[3]);
            attr->topRLabel->setText("顶面外接圆半径");
            break;
        case SPHERE:
            attr->viewable(0, 0, 1, 0, 0, 0, 0, 1, 1);
            attr->RLabel->setText("半径");
            attr->R->setValue(models[widget->selectedID-1].entityAttr[0]);
            attr->density->setValue(models[widget->selectedID-1].entityAttr[1]);
            break;
        case CYLINDER:
            attr->viewable(0, 0, 1, 0, 0, 1, 1, 1, 0);
            attr->RLabel->setText("半径");
            attr->R->setValue(models[widget->selectedID-1].entityAttr[2]);
            attr->faceNumLabel->setText("线密度");
            attr->faceNum->setMinimum(30);
            attr->faceNum->setValue(models[widget->selectedID-1].entityAttr[0]);
            attr->height->setValue(models[widget->selectedID-1].entityAttr[1]);
        case CONE:
            attr->viewable(0, 0, 1, 0, 0, 1, 1, 1, 0);
            attr->RLabel->setText("半径");
            attr->R->setValue(models[widget->selectedID-1].entityAttr[2]);
            attr->faceNumLabel->setText("线密度");
            attr->faceNum->setMinimum(30);
            attr->faceNum->setValue(models[widget->selectedID-1].entityAttr[0]);
            attr->height->setValue(models[widget->selectedID-1].entityAttr[1]);
        default: break;
        }
    }
    else
        attr->viewable(1, 0, 0, 0, 0, 0, 0, 0, 0);
}

void MainWindow::reshapeEntity()
{
    if(models[widget->selectedID-1].type == CUBE){
        model Cube = createCube(attr->R->value());
        Cube.type = CUBE;
        models[widget->selectedID-1] = Cube;
    }
    else if(models[widget->selectedID-1].type == PRISMOID){
        Prismoid prismoid(attr->faceNum->value(), attr->height->value(), attr->botR->value(), attr->topR->value());
        model P = prismoid.createPrismoid();
        P.type = PRISMOID;
        models[widget->selectedID-1] = P;
    }
    else if(models[widget->selectedID-1].type == SPHERE){
        Sphere sphere(attr->R->value(), attr->density->value());
        model s = sphere.createSphere();
        s.type = SPHERE;
        models[widget->selectedID-1] = s;
    }
    else if(models[widget->selectedID-1].type == CYLINDER){
        Prismoid prismoid(attr->faceNum->value(), attr->height->value(), attr->R->value(), attr->R->value());
        model P = prismoid.createPrismoid();
        P.type = CYLINDER;
        models[widget->selectedID-1] = P;
    }
    else if(models[widget->selectedID-1].type == CONE){
        Cone cone(attr->R->value(), attr->height->value(), attr->faceNum->value());
        model C = cone.createCone();
        C.type = CONE;
        models[widget->selectedID-1] = C;
    }
}


void MainWindow::on_actionScreen_Capture_triggered()
{
  QImage image = widget->grabFrameBuffer(false);

  QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("save file"),
                                                    "E:/Study/ComputerGraphics/qtopengl/OBJ/export",
                                                    tr("PNG file(*.png)"));

  image.save(fileName, "png");
}

void MainWindow::on_actionExport_OBJ_File_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("save file"),
                                                    "E:/Study/ComputerGraphics/qtopengl/OBJ/export",
                                                    tr("OBJ file(*.obj)"));
    exportOBJ(fileName);
}

void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    curItem = ui->treeWidget->itemAt(pos);
    if(curItem==NULL || curItem->parent()==NULL)
        return;
    QMenu *popMenu = new QMenu(this);
    popMenu->addAction(ui->actionSelect);
    popMenu->addAction(ui->actionDelete);
    popMenu->exec(QCursor::pos());
}

void MainWindow::on_actionSelect_triggered()
{
    QTreeWidgetItem *parent = curItem->parent();
    if(parent == NULL)
        return;
    int index = parent->indexOfChild(curItem);
    int currentModelID;
    if(parent->text(0) == "Objects")
       currentModelID = objAttr[index].modelIndex;
    else
       currentModelID = Attr[index].modelIndex;

    emit sendSelectOBJ(currentModelID + 1);
}

void MainWindow::on_actionDelete_triggered()
{
    if(curItem == NULL)
        return;
    QTreeWidgetItem *parent = curItem->parent();
    if(parent == NULL)
        return;
    int index = parent->indexOfChild(curItem);
    int currentModelID;
    if(parent->text(0) == "Objects")
       currentModelID = objAttr[index].modelIndex;
    else
       currentModelID = Attr[index].modelIndex;

    vector<model>::iterator it = models.begin() + currentModelID;
    for(unsigned int i=0; i<models[currentModelID].mtls.size(); i++){ //删除绑定纹理
        glDeleteTextures(1, &(models[currentModelID].mtls[i].texID));
      }

    models[currentModelID].deleteDisplayList();

    models.erase(it);

    indexCounter--;

    if(parent->text(0) == "Objects") //删除objAttr和Attr里对应的项
       objAttr.erase(objAttr.begin()+index);
    else
       Attr.erase(Attr.begin()+index);

    delete curItem;
}

void MainWindow::updateAttribute(unsigned int selectedID){
  disconnect(ui->display_scale_x, SIGNAL(valueChanged(double)), this, SLOT(on_display_scale_x_valueChanged(double)));
  disconnect(ui->display_scale_y, SIGNAL(valueChanged(double)), this, SLOT(on_display_scale_y_valueChanged(double)));
  disconnect(ui->display_scale_z, SIGNAL(valueChanged(double)), this, SLOT(on_display_scale_z_valueChanged(double)));
  disconnect(ui->display_x, SIGNAL(valueChanged(double)), this, SLOT(on_display_x_valueChanged(double)));
  disconnect(ui->display_y, SIGNAL(valueChanged(double)), this, SLOT(on_display_y_valueChanged(double)));
  disconnect(ui->display_z, SIGNAL(valueChanged(double)), this, SLOT(on_display_z_valueChanged(double)));
  if(selectedID){//有选中的物体
        ui->display_x->setValue(models[selectedID-1].offset_x);
        ui->display_y->setValue(models[selectedID-1].offset_y);
        ui->display_z->setValue(models[selectedID-1].offset_z);

        ui->display_scale_x->setValue(models[selectedID-1].scale_x);
        ui->display_scale_y->setValue(models[selectedID-1].scale_y);
        ui->display_scale_z->setValue(models[selectedID-1].scale_z);
    }else{//没有选中的物体
      //ui->display_x->setValue(widget->main_scale);
      //ui->display_y->setValue(models[selectedID-1].offset_y);
      //ui->display_z->setValue(models[selectedID-1].offset_z);
    }

  connect(ui->display_scale_x, SIGNAL(valueChanged(double)), this, SLOT(on_display_scale_x_valueChanged(double)));
  connect(ui->display_scale_y, SIGNAL(valueChanged(double)), this, SLOT(on_display_scale_y_valueChanged(double)));
  connect(ui->display_scale_z, SIGNAL(valueChanged(double)), this, SLOT(on_display_scale_z_valueChanged(double)));
  connect(ui->display_x, SIGNAL(valueChanged(double)), this, SLOT(on_display_x_valueChanged(double)));
  connect(ui->display_y, SIGNAL(valueChanged(double)), this, SLOT(on_display_y_valueChanged(double)));
  connect(ui->display_z, SIGNAL(valueChanged(double)), this, SLOT(on_display_z_valueChanged(double)));
}

void MainWindow::on_display_x_valueChanged(double offset_x)
{
    if(widget->selectedID){
        models[widget->selectedID-1].offset_x = offset_x;
      }
}

void MainWindow::on_display_y_valueChanged(double offset_y)
{
  if(widget->selectedID){
      models[widget->selectedID-1].offset_y = offset_y;
    }
}

void MainWindow::on_display_z_valueChanged(double offset_z)
{
  if(widget->selectedID){
      models[widget->selectedID-1].offset_z = offset_z;
    }
}

void MainWindow::on_display_scale_x_valueChanged(double scale_x)
{
  if(widget->selectedID){
      models[widget->selectedID-1].scale_x = scale_x;
    }
}

void MainWindow::on_display_scale_y_valueChanged(double scale_y)
{
  if(widget->selectedID){
      models[widget->selectedID-1].scale_y = scale_y;
    }
}

void MainWindow::on_display_scale_z_valueChanged(double scale_z)
{
  if(widget->selectedID){
      models[widget->selectedID-1].scale_z = scale_z;
    }
}

void MainWindow::on_actionOpen_Project_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("open file"),
                                                    "E:/Study/ComputerGraphics/qtopengl/project",
                                                    tr("OBJ file(*.prj)"));
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream proLog(&file);
    QString line;
    while(!proLog.atEnd()){
        line = proLog.readLine();
        QStringList attrs = line.split("#");
        if(attrs.at(0) == "OBJ"){
            widget->initOBJ();
            loadOBJ(attrs.at(1).toStdString().c_str());
            widget->updateGL();
            emit objectSubmit(false);

            models.back().file = attrs.at(1);
        }
        else{
            QStringList entityAttrs = attrs.at(1).split("%");
            if(attrs.at(0).toInt() == CUBE){
                model Cube = createCube(entityAttrs.at(0).toFloat());
                Cube.type = CUBE;
                models.push_back(Cube);
            }
            else if(attrs.at(0).toInt() == PRISMOID){
                Prismoid prismoid(entityAttrs.at(0).toFloat(), entityAttrs.at(1).toFloat(), entityAttrs.at(2).toFloat(), entityAttrs.at(3).toFloat());
                model P = prismoid.createPrismoid();
                P.type = PRISMOID;
                models.push_back(P);
            }
            else if(attrs.at(0).toInt() == SPHERE){
                Sphere sphere(entityAttrs.at(0).toFloat(), entityAttrs.at(1).toFloat());
                model s = sphere.createSphere();
                s.type = SPHERE;
                models.push_back(s);
            }
            else if(attrs.at(0).toInt() == CYLINDER){
                Prismoid prismoid_c(entityAttrs.at(0).toFloat(), entityAttrs.at(1).toFloat(), entityAttrs.at(2).toFloat(), entityAttrs.at(3).toFloat());
                model P_c = prismoid_c.createPrismoid();
                P_c.type = CYLINDER;
                models.push_back(P_c);
            }
            else if(attrs.at(0).toInt() == CONE){
                Cone cone(entityAttrs.at(0).toFloat(), entityAttrs.at(1).toFloat(), entityAttrs.at(2).toFloat());
                model C = cone.createCone();
                C.type = CONE;
                models.push_back(C);
            }
            else{
                return;
            }
            emit objectSubmit(true);
        }
        models.back().offset_x = attrs.at(2).toFloat();
        models.back().offset_y = attrs.at(3).toFloat();
        models.back().offset_z = attrs.at(4).toFloat();
        models.back().scale_x = attrs.at(5).toFloat();
        models.back().scale_y = attrs.at(6).toFloat();
        models.back().scale_z = attrs.at(7).toFloat();
        for(int i = 0; i< 16; i++){
            models.back().rotateMatrix[i] = attrs.at(i + 8).toFloat();
        }
    }
    file.close();
}

void MainWindow::on_actionSave_Project_triggered()
{
    QString projectLog = "";
    for(unsigned int i = 0; i< models.size(); i++){
        if(models[i].type == NOT){
            projectLog += "OBJ#" + models[i].file + "#";
        }
        else{
            projectLog += QString::number(models[i].type) + "#";
            for(unsigned int j = 0; j< models[j].entityAttr.size(); j++){
                projectLog += (QString::number(models[i].entityAttr[j]) + "%");
            }
            projectLog += "#";
        }
        projectLog = projectLog + QString::number(models[i].offset_x) + "#" + QString::number(models[i].offset_y) + "#" + QString::number(models[i].offset_z) + "#"
                + QString::number(models[i].scale_x) + "#" + QString::number(models[i].scale_y) + "#" + QString::number(models[i].scale_z);
        for(int j = 0; j< 16; j++){
            projectLog += ("#" + QString::number(models[i].rotateMatrix[j]));
        }
        projectLog += "\n";
    }
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("save file"),
                                                    "E:/Study/ComputerGraphics/qtopengl/OBJ/project",
                                                    tr("Project file(*.prj)"));
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << projectLog;
    file.close();
}
