#include "ui_openglwindow.h"
#include "openglwindow.h"
#include "glwidget.h"
#include <QFileDialog>
#include <vector>
#include <QDebug>

using namespace std;

extern vector<model> models;

openglWindow::openglWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::openglWindow)
{
    ui->setupUi(this);
    widget = new GLWidget();
    widget->setObjectName(QStringLiteral("opengl"));
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->verticalLayout_3->addWidget(widget);
    //widget->setObjectName(QStringLiteral("widget"));
    //widget->setGeometry(QRect(0, 0, 500, 500));
    widget->setFocusPolicy(Qt::ClickFocus);

    connect(widget, SIGNAL(model_select()), this, SLOT(InitSpinBox()));
}

openglWindow::~openglWindow()
{
    delete ui;
}

void openglWindow::on_actionImport_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("open file"),
                                                    "E:/Study/ComputerGraphics/qtopengl/OBJ",
                                                    tr("OBJ file(*.obj)"));



    widget->initOBJ();
    loadOBJ(fileName.toStdString().c_str());
    //widget->objCenter = calObjCenter();
    widget->updateGL();



}

void openglWindow::on_WireFrame_BTN_clicked()
{
  if(widget->isWireframe){
      widget->isWireframe = false;
      ui->WireFrame_BTN->setText("WireFrame");
    }
  else{
      widget->isWireframe = true;
      ui->WireFrame_BTN->setText("Surface");
    }
}


void openglWindow::on_Light_BTN_clicked()
{
  if(widget->enableLight){
      widget->enableLight = false;
      ui->Light_BTN->setText("Light on");
    }
  else{
      widget->enableLight = true;
      ui->Light_BTN->setText("Light off");
    }
}

void openglWindow::on_actionExport_triggered()
{
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("save file"),
                                                  "E:/Study/ComputerGraphics/qtopengl/OBJ/export",
                                                  tr("OBJ file(*.obj)"));
  exportOBJ(fileName);

}

void openglWindow::InitSpinBox(){
  ui->doubleSpinBox->setValue(0.0);
  ui->doubleSpinBox_2->setValue(0.0);
  ui->doubleSpinBox_3->setValue(0.0);
}



void openglWindow::on_doubleSpinBox_valueChanged(double rotate_x)
{
  qDebug()<<"test";
  if(widget->selectedID){
      if(ui->doubleSpinBox_2->text().toFloat() != 0.0){
          glPushMatrix();
          glLoadMatrixf(widget->lastRotateMatrix);
          glRotatef(ui->doubleSpinBox_2->text().toFloat(), 0, 1, 0);
          glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
          glPopMatrix();
          for(int i=0; i<16; i++)
            widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
          ui->doubleSpinBox_2->setValue(0.0);
        }
      if(ui->doubleSpinBox_3->text().toFloat() != 0.0){
          glPushMatrix();
          glLoadMatrixf(widget->lastRotateMatrix);
          glRotatef(ui->doubleSpinBox_3->text().toFloat(), 0, 0, 1);
          glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
          glPopMatrix();
          for(int i=0; i<16; i++)
            widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
          ui->doubleSpinBox_3->setValue(0.0);
        }
      glPushMatrix();
      glLoadMatrixf(widget->lastRotateMatrix);
      glRotatef(rotate_x, 1, 0, 0);
      glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
      glPopMatrix();
      //models[widget->selectedID-1].rotate_x = rotate_x;
    }
}

void openglWindow::on_doubleSpinBox_2_valueChanged(double rotate_y)
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
          ui->doubleSpinBox->setValue(0.0);
        }
      if(ui->doubleSpinBox_3->text().toFloat() != 0.0){
          glPushMatrix();
          glLoadMatrixf(widget->lastRotateMatrix);
          glRotatef(ui->doubleSpinBox_3->text().toFloat(), 0, 0, 1);
          glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
          glPopMatrix();
          for(int i=0; i<16; i++)
            widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
          ui->doubleSpinBox_3->setValue(0.0);
        }
      glPushMatrix();
      glLoadMatrixf(widget->lastRotateMatrix);
      glRotatef(rotate_y, 0, 1, 0);
      glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
      glPopMatrix();
      //models[widget->selectedID-1].rotate_y = rotate_y;
    }
}


void openglWindow::on_doubleSpinBox_3_valueChanged(double rotate_z)
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
          ui->doubleSpinBox_2->setValue(0.0);
        }
      if(ui->doubleSpinBox->text().toFloat() != 0.0){
          glPushMatrix();
          glLoadMatrixf(widget->lastRotateMatrix);
          glRotatef(ui->doubleSpinBox->text().toFloat(), 1, 0, 0);
          glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
          glPopMatrix();
          for(int i=0; i<16; i++)
            widget->lastRotateMatrix[i] = models[widget->selectedID-1].rotateMatrix[i];
          ui->doubleSpinBox->setValue(0.0);
        }
      glPushMatrix();
      glLoadMatrixf(widget->lastRotateMatrix);
      glRotatef(rotate_z, 0, 0, 1);
      glGetFloatv(GL_MODELVIEW_MATRIX, models[widget->selectedID-1].rotateMatrix);
      glPopMatrix();
      //models[widget->selectedID-1].rotate_z = rotate_z;
    }
}

