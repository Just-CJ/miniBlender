#include "ui_openglwindow.h"
#include "openglwindow.h"
#include "glwidget.h"
#include <QFileDialog>

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
