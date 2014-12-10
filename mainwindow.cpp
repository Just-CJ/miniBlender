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
struct Clip{
    model mod;
    bool valid;
}Clipboard;

int lightIDs[100];
//预置的材质
QString Texfile[5];
struct attribute{
    QString modelName;
    int modelIndex;
};
vector<attribute> objAttr;
vector<attribute> entityAttr;
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

    ui->actionOrbit->setCheckable(true);
    ui->actionPan->setCheckable(true);
    ui->actionOrbit->setChecked(true);
    ui->actionPan->setChecked(false);

    attr = new attributeWidget();
    ui->verticalLayout_4->addWidget(attr);
    initialCatalog();
    indexCounter = 0;

    rmDialog = new renameDialog();

    Clipboard.valid = false;

//    textureNumber=0;
    selectedLight=false;

    //绑定预置纹理
    initialTex();

    connect(widget, SIGNAL(model_select()), this, SLOT(initSpinBoxAndSlider()));
    connect(widget, SIGNAL(model_select()), this, SLOT(selectedAttribute()));
    connect(widget, SIGNAL(model_select()), this, SLOT(getSelectedItem()));
    connect(widget, SIGNAL(model_select()), this, SLOT(getSelectedTex()));
    connect(widget, SIGNAL(signal_updateAttr(uint)), this, SLOT(updateAttribute(uint)));
    connect(attr, SIGNAL(reshape()), this, SLOT(reshapeEntity()));
    connect(rmDialog, SIGNAL(rename(QString)), this, SLOT(renameOBJ(QString)));
    connect(this, SIGNAL(objectSubmit(bool)), this, SLOT(updateCatalog(bool)));
    connect(this, SIGNAL(sendSelectOBJ(unsigned int)), widget, SLOT(modelSelect(unsigned int)));
    connect(this, SIGNAL(sendMltSubmit(uint)), this, SLOT(updateCatalogMTL(uint)));
    connect(this, SIGNAL(sendSelectLightMTL(int)), this, SLOT(on_tabWidget_tabBarClicked(int)));
    connect(widget, SIGNAL(sendSelectLightMTL(int)), this, SLOT(on_tabWidget_tabBarClicked(int)));

}

void MainWindow::initialTex()
{
    Texfile[0] = ":/Texture/Dirt.bmp";
    Texfile[1] = ":/Texture/Marble.bmp";
    Texfile[2] = ":/Texture/Stone.bmp";
    Texfile[3] = ":/Texture/WallPaper.bmp";
    Texfile[4] = ":/Texture/Wood.bmp";
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::unselectLights()
{
    QTreeWidgetItem *parent = ui->treeWidget->topLevelItem(2);
    for(int i = 0; i< parent->childCount(); i++){
        QString qs = parent->child(i)->text(0);
        qs.replace("*", "");
        parent->child(i)->setText(0,qs);
    }
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
    for(unsigned int i = 0; i< entityAttr.size(); i++){
        if(entityAttr[i].modelIndex == id){
            QTreeWidgetItem *parent = ui->treeWidget->topLevelItem(1);
            curItem = parent->child(i);
        }
    }
//    qDebug()<<curItem->text(0);
//    qDebug()<<id;
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



void MainWindow::on_actionAdd_light_triggered()
{
     int i;
     for(i=0;i<8;i++) if(!widget->lights[i].isOn) break;
     widget->addLight=true;
     widget->lightnumber++;
     QTreeWidgetItem *topItem = ui->treeWidget->topLevelItem(2);
     QString name = "light" + QString::number(widget->lightnumber);
     QTreeWidgetItem *newObj = new QTreeWidgetItem(QStringList()<<name);
     topItem->addChild(newObj);
     lightIDs[widget->lightnumber-1]=i;
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
    QTreeWidgetItem *Lights = new QTreeWidgetItem(QStringList()<<"Lights");
    ui->treeWidget->addTopLevelItem(Lights);
}

void MainWindow::updateCatalog(bool EntityOrObject)
{
    if(EntityOrObject == true){
        QTreeWidgetItem *topItem = ui->treeWidget->topLevelItem(1);
        QString name = "entity" + QString::number(entityAttr.size()+1);
        attribute ent;
        ent.modelName = name;
        ent.modelIndex = indexCounter;
        QTreeWidgetItem *newObj = new QTreeWidgetItem(QStringList()<<name);
        QTreeWidgetItem *objMtl = new QTreeWidgetItem(QStringList()<<"No mtl");
        entityAttr.push_back(ent);
        newObj->addChild(objMtl);
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
        for(unsigned int i = 0; i< models[indexCounter].mtls.size(); i++){
            QTreeWidgetItem *objMtl = new QTreeWidgetItem(QStringList()<<QString::fromStdString(models[indexCounter].mtls[i].mtlname));
            newObj->addChild(objMtl);
        }
        topItem->addChild(newObj);

        indexCounter++;
    }
}

void MainWindow::updateCatalogMTL(unsigned int SelectID)
{
//    qDebug()<<SelectID;
    //首先选择控件
    int id = SelectID - 1;
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
    for(unsigned int i = 0; i< entityAttr.size(); i++){
        if(entityAttr[i].modelIndex == id){
            QTreeWidgetItem *parent = ui->treeWidget->topLevelItem(1);
            curItem = parent->child(i);
        }
    }

    curItem->child(0)->setText(0,QString::fromStdString(models[id].mtls[0].mtlname));
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parent = item->parent();
    if(parent == NULL || (parent->text(column) != "Objects" && parent->text(column) != "Entities" && parent->text(column) != "Lights"))
        return;

    int index = parent->indexOfChild(item);
    unsigned int currentModelID;
    if(parent->text(column) == "Objects"){
       currentModelID = objAttr[index].modelIndex;
       if(currentModelID + 1 == widget->selectedID)
           return;
    }
    if(parent->text(column) == "Entities"){
       currentModelID = entityAttr[index].modelIndex;
       if(currentModelID + 1 == widget->selectedID)
           return;
    }
    if(parent->text(column) == "Lights"){
       unselectLights();
       selectedLID=lightIDs[index];
       selectedLight=true;
       QString qs="*"+item->text(0);
       item->setText(0,qs);
       for(int i=0;i<parent->childCount();i++)
           if(i!=selectedLID && parent->child(i)->text(0)[0]=='*'){
               qs="";
               int l=parent->child(i)->text(0).length();
               for(int j=1;j<l;j++)
                   qs+=parent->child(i)->text(0)[j];
               parent->child(i)->setText(0,qs);
           }
       emit sendSelectLightMTL(3);
       return;
    }

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
    else if(curItem->parent()->text(0) != "Objects" && curItem->parent()->text(0) != "Entities" && curItem->parent()->text(0) != "Lights" )
        return;
    //选中当前物体
    on_treeWidget_itemDoubleClicked(curItem,0);
//    qDebug()<<widget->selectedID;

    QMenu *popMenu = new QMenu(this);
    popMenu->addAction(ui->actionSelect);
    popMenu->addAction(ui->actionDelete);
    popMenu->addAction(ui->actionRename);
    if(curItem->parent()->text(0) != "Lights" ){
        popMenu->addSeparator();
        popMenu->addAction(ui->actionCopy);
        popMenu->addAction(ui->actionPaste);
    }
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
    if(parent->text(0) == "Entities")
       currentModelID = entityAttr[index].modelIndex;
    if(parent->text(0) == "Lights"){
       unselectLights();
       selectedLID=lightIDs[index];
       selectedLight=true;
       QString qs="*"+curItem->text(0);
       curItem->setText(0,qs);
       for(int i=0;i<parent->childCount();i++)
           if(i!=selectedLID && parent->child(i)->text(0)[0]=='*'){
               qs="";
               int l=parent->child(i)->text(0).length();
               for(int j=1;j<l;j++)
                   qs+=parent->child(i)->text(0)[j];
               parent->child(i)->setText(0,qs);
           }
    }
    else {
        emit sendSelectOBJ(currentModelID + 1);
    }
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
    if(parent->text(0) == "Entities")
       currentModelID = entityAttr[index].modelIndex;
    if(parent->text(0) == "Lights"){
       widget->lights[lightIDs[index]].isOn=false;
       for(int i=0;i<8;i++)
//       qDebug()<<widget->lights[i].isOn;
       widget->lightnumber--;
       delete curItem;
       if(selectedLight && lightIDs[index]==selectedLID)
       selectedLight=false;
       return;
    }
    //qDebug()<<widget->selectedID<<" "<<curItem->text(0)<<" "<<currentModelID;

    vector<model>::iterator it = models.begin() + currentModelID;
    for(unsigned int i=0; i<models[currentModelID].mtls.size(); i++){ //删除绑定纹理
        glDeleteTextures(1, &(models[currentModelID].mtls[i].texID));
      }

    models[currentModelID].deleteDisplayList();

    models.erase(it);

    indexCounter--;

    if(parent->text(0) == "Objects") //删除objAttr和entityAttr里对应的项
       objAttr.erase(objAttr.begin()+index);
    if(parent->text(0) == "Entities")
       entityAttr.erase(entityAttr.begin()+index);

    delete curItem;

    for(unsigned int i = 0; i< objAttr.size(); i++){
        if(objAttr[i].modelIndex + 1 > (int)widget->selectedID){
            objAttr[i].modelIndex--;
        }
    }
    for(unsigned int i = 0; i< entityAttr.size(); i++){
        if(entityAttr[i].modelIndex + 1 > (int)widget->selectedID){
           entityAttr[i].modelIndex--;
        }
    }

    widget->selectedID = 0;
    curItem = NULL;
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
            //添加纹理
            QString fileName = entityAttrs.last();
            QImage tex, buf;
            if(!buf.load(fileName)){
               return;
            }
            tex = widget->convertToGLFormat(buf);
           //    int i=textureNumber;
            GLuint texture;
           //    glGenTextures(1,&texture[0]);
            glGenTextures(1,&texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理小时，采用GL_LINEAR的方法来处理
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理大时，采用GL_LINEAR的方法来处理
            vector <object>::iterator it;
            vector <mtl>::iterator itm=models.back().mtls.begin();
            for(it=models.back().objects.begin();it<models.back().objects.end();it++){
                (*itm).texID=(*it).texID=texture;
                QStringList qsl = fileName.split("/");
                QString qs = qsl.last();
                (*itm).mtlname=(*it).mtlname=qs.toStdString();
                (*itm).map_Kd_addr=(*it).map_Kd_addr=fileName;
                itm++;
            }
            models.back().deleteDisplayList();
            models.back().genDisplayList();


            emit sendMltSubmit(widget->selectedID);
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
            for(unsigned int j = 0; j< models[i].entityAttr.size(); j++){
                projectLog += (QString::number(models[i].entityAttr[j]) + "%");
            }
            projectLog += models[i].mtls[0].map_Kd_addr;
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

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if(index==2){
        int id=widget->selectedID-1;
        if(id>=0){
            vector<object>::iterator it=models[id].objects.begin();
            ui->ambient1->setValue((*it).ambient[0]);
            ui->ambient2->setValue((*it).ambient[1]);
            ui->ambient3->setValue((*it).ambient[2]);
            ui->diffuse1->setValue((*it).diffuse[0]);
            ui->diffuse2->setValue((*it).diffuse[1]);
            ui->diffuse3->setValue((*it).diffuse[2]);
            ui->specular1->setValue((*it).specular[0]);
            ui->specular2->setValue((*it).specular[1]);
            ui->specular3->setValue((*it).specular[2]);
            ui->emission1->setValue((*it).emission[0]);
            ui->emission2->setValue((*it).emission[1]);
            ui->emission3->setValue((*it).emission[2]);
        }
        else{
            ui->ambient1->setValue(0);
            ui->ambient2->setValue(0);
            ui->ambient3->setValue(0);
            ui->diffuse1->setValue(0);
            ui->diffuse2->setValue(0);
            ui->diffuse3->setValue(0);
            ui->specular1->setValue(0);
            ui->specular2->setValue(0);
            ui->specular3->setValue(0);
            ui->emission1->setValue(0);
            ui->emission2->setValue(0);
            ui->emission3->setValue(0);
        }

    }
    if(index==3){
        if(selectedLight){
            int i=selectedLID;
            light l=widget->lights[i];
            ui->lambient1->setValue(l.light_ambient[0]);
            ui->lambient2->setValue(l.light_ambient[1]);
            ui->lambient3->setValue(l.light_ambient[2]);
            ui->ldiffuse1->setValue(l.light_diffuse[0]);
            ui->ldiffuse2->setValue(l.light_diffuse[1]);
            ui->ldiffuse3->setValue(l.light_diffuse[2]);
            ui->lspecular1->setValue(l.light_specular[0]);
            ui->lspecular2->setValue(l.light_specular[1]);
            ui->lspecular3->setValue(l.light_specular[2]);
            ui->lposition1->setValue(l.light_position[0]);
            ui->lposition2->setValue(l.light_position[1]);
            ui->lposition3->setValue(l.light_position[2]);
            ui->intensity->setValue(l.intensity);
       }
       else{
            ui->lambient1->setValue(0);
            ui->lambient2->setValue(0);
            ui->lambient3->setValue(0);
            ui->ldiffuse1->setValue(0);
            ui->ldiffuse2->setValue(0);
            ui->ldiffuse3->setValue(0);
            ui->lspecular1->setValue(0);
            ui->lspecular2->setValue(0);
            ui->lspecular3->setValue(0);
            ui->lposition1->setValue(0);
            ui->lposition2->setValue(0);
            ui->lposition3->setValue(0);
            ui->intensity->setValue(0);
       }
    }
}

void MainWindow::on_ambient1_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).ambient[0]=(*it).ambient[0]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_ambient2_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).ambient[1]=(*it).ambient[1]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}


void MainWindow::on_ambient3_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;\
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).ambient[2]=(*it).ambient[2]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}


void MainWindow::on_diffuse1_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).diffuse[0]=(*it).diffuse[0]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_diffuse2_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).diffuse[1]=(*it).diffuse[1]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_diffuse3_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).diffuse[2]=(*it).diffuse[2]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_specular1_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).specular[0]=(*it).specular[0]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_specular2_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).specular[1]=(*it).specular[1]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_specular3_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
           (*itm).specular[2]=(*it).specular[2]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_emission1_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).emission[0]=(*it).emission[0]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_emission2_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).emission[1]=(*it).emission[1]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_emission3_valueChanged(double arg1)
{
    int id=widget->selectedID-1;
    if(id>=0){
        vector<object>::iterator it;
        vector<mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).emission[2]=(*it).emission[2]=arg1;
            itm++;
        }
        models[id].genDisplayList();
    }
}

void MainWindow::on_pushButton_2_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this,tr("open file"),"E:/Images",tr("BMP file(*.bmp)"));
    QImage tex, buf;
    if(!buf.load(fileName)){
        return;
    }
    tex = widget->convertToGLFormat(buf);
//    int i=textureNumber;
    GLuint texture;
//    glGenTextures(1,&texture[0]);
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理小时，采用GL_LINEAR的方法来处理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理大时，采用GL_LINEAR的方法来处理
    int id=widget->selectedID-1;
    if(id>=0){
        vector <object>::iterator it;
        vector <mtl>::iterator itm=models[id].mtls.begin();
        for(it=models[id].objects.begin();it<models[id].objects.end();it++){
            (*itm).texID=(*it).texID=texture;
            QStringList qsl = fileName.split("/");
            QString qs = qsl.last();
            (*itm).mtlname=(*it).mtlname=qs.toStdString();
            (*itm).map_Kd_addr=(*it).map_Kd_addr=fileName;
            itm++;
        }
        models[id].deleteDisplayList();
        models[id].genDisplayList();
    }
//    qDebug()<<"textureNumber"<<textureNumber;
//    textureNumber++;

    emit sendMltSubmit(widget->selectedID);
}

void MainWindow::on_lambient1_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_ambient[0]=arg1;
}

void MainWindow::on_lambient2_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_ambient[1]=arg1;
}

void MainWindow::on_lambient3_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_ambient[2]=arg1;
}

void MainWindow::on_ldiffuse1_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_diffuse[0]=arg1;
}

void MainWindow::on_ldiffuse2_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_diffuse[1]=arg1;
}

void MainWindow::on_ldiffuse3_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_diffuse[2]=arg1;
}

void MainWindow::on_lspecular1_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_specular[0]=arg1;
}

void MainWindow::on_lspecular2_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_specular[1]=arg1;
}

void MainWindow::on_lspecular3_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_specular[2]=arg1;
}

void MainWindow::on_lposition1_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_position[0]=arg1;
}

void MainWindow::on_lposition2_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_position[1]=arg1;
}

void MainWindow::on_lposition3_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].light_position[2]=arg1;
}

void MainWindow::on_intensity_valueChanged(double arg1)
{
    if(selectedLight)
        widget->lights[selectedLID].intensity=arg1;
}



void MainWindow::on_actionLight_on_off_triggered()
{
    if(widget->enableLight)
        widget->enableLight=false;
    else
        widget->enableLight=true;
}

void MainWindow::on_actionRename_triggered()
{
    rmDialog->show();
}

void MainWindow::renameOBJ(QString newName)
{
    curItem->setText(0, newName);
}

void MainWindow::on_actionCopy_triggered()
{
    if(widget->selectedID == 0)
        return;
    Clipboard.mod = models[widget->selectedID-1];
    Clipboard.mod.isSelected = false;
    Clipboard.valid = true;
}

void MainWindow::on_actionPaste_triggered()
{
    if(!Clipboard.valid)
        return;
    models.push_back(Clipboard.mod);
    if(Clipboard.mod.type == NOT)
        emit objectSubmit(false);
    else
        emit objectSubmit(true);
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    qDebug()<<index;
    if(widget->selectedID == 0)
        return;
    if(index == 0){
        for(unsigned int i = 0; i<models[widget->selectedID - 1].mtls.size(); i++){ //删除预设纹理
            if(models[widget->selectedID - 1].mtls[i].mtlname == ui->comboBox->itemText(1).toStdString() ||
                    models[widget->selectedID - 1].mtls[i].mtlname == ui->comboBox->itemText(2).toStdString() ||
                    models[widget->selectedID - 1].mtls[i].mtlname == ui->comboBox->itemText(3).toStdString() ||
                    models[widget->selectedID - 1].mtls[i].mtlname == ui->comboBox->itemText(4).toStdString() ||
                    models[widget->selectedID - 1].mtls[i].mtlname == ui->comboBox->itemText(5).toStdString())
            glDeleteTextures(1, &(models[widget->selectedID - 1].mtls[i].texID));
        }
    }
    else{
        QString fileName = Texfile[index-1];
        QImage tex, buf;
        if(!buf.load(fileName)){
            return;
        }
        tex = widget->convertToGLFormat(buf);
        GLuint texture;
        glGenTextures(1,&texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理小时，采用GL_LINEAR的方法来处理
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理大时，采用GL_LINEAR的方法来处理
        int id=widget->selectedID-1;
        if(id>=0){
            vector <object>::iterator it;
            vector <mtl>::iterator itm=models[id].mtls.begin();
            for(it=models[id].objects.begin();it<models[id].objects.end();it++){
                (*itm).texID=(*it).texID=texture;
                QString qs = ui->comboBox->currentText();
                (*itm).mtlname=(*it).mtlname=qs.toStdString();
                (*itm).map_Kd_addr=(*it).map_Kd_addr=fileName;
                itm++;
            }
            models[id].deleteDisplayList();
            models[id].genDisplayList();
        }
        emit sendMltSubmit(widget->selectedID);
    }
}

void MainWindow::getSelectedTex()
{
    if(widget->selectedID == 0)
        return;
    else{
        if(models[widget->selectedID - 1].mtls[0].mtlname == ui->comboBox->itemText(1).toStdString()){
            ui->comboBox->setCurrentIndex(1);
        }
        else if(models[widget->selectedID - 1].mtls[0].mtlname == ui->comboBox->itemText(2).toStdString()){
            ui->comboBox->setCurrentIndex(2);
        }
        else if(models[widget->selectedID - 1].mtls[0].mtlname == ui->comboBox->itemText(3).toStdString()){
            ui->comboBox->setCurrentIndex(3);
        }
        else if(models[widget->selectedID - 1].mtls[0].mtlname == ui->comboBox->itemText(4).toStdString()){
            ui->comboBox->setCurrentIndex(4);
        }
        else if(models[widget->selectedID - 1].mtls[0].mtlname == ui->comboBox->itemText(5).toStdString()){
            ui->comboBox->setCurrentIndex(5);
        }
        else{
            ui->comboBox->setCurrentIndex(0);
        }
    }
}

void MainWindow::on_actionOrbit_triggered()
{
  //ui->actionOrbit->setCheckable(true);
  ui->actionOrbit->setChecked(true);
  ui->actionPan->setChecked(false);
  widget->CursorMode = ORBIT;

  widget->world_center_x += widget->delta_Panx;
  widget->world_center_z += widget->delta_Panz;

  widget->CurrentAngleZ += widget->PanAngle;
  widget->LastAngleZ = widget->CurrentAngleZ;
  widget->PanAngle = 0;
  widget->delta_Panx = 0;
  widget->delta_Panz = 0;

  QVector3D vector1(sin(widget->CurrentAngleY)*sin(widget->CurrentAngleZ),cos(widget->CurrentAngleY),sin(widget->CurrentAngleY)*cos(widget->CurrentAngleZ));
  vector1=vector1.normalized();  //将坐标单位化
  widget->eyex=vector1.x();
  widget->eyey=vector1.y();
  widget->eyez=vector1.z();

}

void MainWindow::on_actionPan_triggered()
{
  ui->actionPan->setChecked(true);
  ui->actionOrbit->setChecked(false);
  widget->CursorMode = PAN;
  widget->lastupx = widget->upx;
  widget->lastupy = widget->upy;
  widget->lastupz = widget->upz;
}

void MainWindow::on_actionRotateAnime_triggered()
{
    if(widget->beRotate) widget->beRotate = false;
    else widget->beRotate = true;
}
