#include "glwidget.h"
#include "ui_glwidget.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <QPoint>
#include <qmath.h>
#include <QVector3D>
#include <vector>
#include <QtGui>
#include <QtCore>
#include <QtOpenGL/QtOpenGL>

//#define _STDCALL_SUPPORTED
//#define GLUT_DISABLE_ATEXIT_HACK




using namespace std;

//extern vector<vpoint> vpoints;
//extern vector<vnormal> vnormals;
//extern vector<vtex> vtexs;
//extern vector<face> faces;
//extern vector<mtl> mtls;
//extern vector<object> objects;
extern vector<model> models;

void GLWidget::initOBJ(){
  main_x = 0;
  main_y = 0;
  main_z = 0;

  objRotate_x = 0.0f;
}

GLWidget::GLWidget(QGLWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::GLWidget)
{
    ui->setupUi(this);
    fullscreen = false;

    startTimer(10);//开启10ms定时器

    /*初始化成员*/
    initOBJ();

    isWireframe = false;
    enableLight = false;
    objRotate = false;

    main_scale = 1.0f;

    objRotate_x = 0.0f;

    main_x = 0;
    main_y = 0;
    main_z = 0;

    camera_x=0.0;
    camera_y=0.0;
    camera_z=0.0;

    eyex=0.1;
    eyey=0.1;
    eyez=0.1;

    upx=0;
    upy=0.1;
    upz=0;

    CurrentAngleZ=0;
    CurrentAngleY=0;
    LastAngleZ=M_PI/4;
    LastAngleY=M_PI/4;

    RotateViewPoint();
    TranslateViewPoint();
}


void GLWidget::RotateViewPoint()
{
    float avAnale=M_PI/180*0.6; //把每次移动的角度单位化

    /*把每次移动点跟开始按下鼠标记录的点作差，然后乘以avAngle,最后把上一次释放鼠标后时记录的
      角度相加起来*/
    CurrentAngleZ=-(EndPoint.x()-StartPoint.x())*avAnale;
    CurrentAngleZ+=LastAngleZ;
    CurrentAngleY=-(EndPoint.y()-StartPoint.y())*avAnale;
    CurrentAngleY+=LastAngleY;


    QVector3D vector1(sin(CurrentAngleY)*sin(CurrentAngleZ),cos(CurrentAngleY),sin(CurrentAngleY)*cos(CurrentAngleZ));
    vector1=vector1.normalized();  //将坐标单位化
    eyex=vector1.x();
    eyey=vector1.y();
    eyez=vector1.z();

    /*主要计算第三组坐标*/
    QVector3D vectorA(0,sin(CurrentAngleY),0);
    QVector3D vectorB=QVector3D(0,0,0)-QVector3D(sin(CurrentAngleY)*sin(CurrentAngleZ),0,sin(CurrentAngleY)*cos(CurrentAngleZ));
    QVector3D vectorAB=QVector3D::crossProduct(vectorA,vectorB);


    QVector3D vectorC=QVector3D(0,0,0)-vector1;
    QVector3D vector2=QVector3D::crossProduct(vectorC,vectorAB);
    vector2=vector2.normalized();
    upx=vector2.x();
    upy=vector2.y();
    upz=vector2.z();



}

void GLWidget::TranslateViewPoint(){
  GLfloat delta_x = 0.5*(EndPoint.x()-StartPoint.x());
  GLfloat delta_y = 0.5*(EndPoint.y()-StartPoint.y());

  main_x = LastMain_x + delta_x*cos(CurrentAngleZ) + delta_y*cos(CurrentAngleY)*sin(CurrentAngleZ);
  main_y = LastMain_y - delta_y*sin(CurrentAngleY);
  main_z = LastMain_z - delta_x*sin(CurrentAngleZ) + delta_y*cos(CurrentAngleY)*cos(CurrentAngleZ);
}


void GLWidget::mousePressEvent(QMouseEvent *e)
{

  switch(e->buttons()){
    case Qt::LeftButton :{
        QPoint st(e->pos());
        StartPoint=st;
      };break;
    case Qt::RightButton:{
        QPoint st(e->pos());
        StartPoint=st;
      };break;
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
  switch(e->button()){
    case Qt::LeftButton :{
        /*记录上一次的角度*/
        LastAngleZ=CurrentAngleZ;
        LastAngleY=CurrentAngleY;
      };break;
    case Qt::RightButton:{
        LastMain_x = main_x;
        LastMain_y = main_y;
        LastMain_z = main_z;
      };break;
    default:break;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    switch(e->buttons()){
      case Qt::LeftButton :{
          QPoint ed(e->pos());
          EndPoint=ed;
          RotateViewPoint();
          this->updateGL();
        };break;
      case Qt::RightButton:{
          QPoint ed(e->pos());
          EndPoint=ed;
          TranslateViewPoint();
          this->updateGL();
        };break;
      }
}

void GLWidget::wheelEvent(QWheelEvent *e)//鼠标滑轮
{
    e->delta() > 0 ? main_scale += main_scale*0.1f : main_scale -= main_scale*0.1f;
    this->updateGL();
}



//初始化
void GLWidget::initializeGL()
{

    setGeometry(300, 150, 640, 480);//设置窗口初始位置和大小
    glShadeModel(GL_FLAT);//设置阴影平滑模式
    glClearColor(0.0, 0.0, 0.0, 0);//改变窗口的背景颜色，不过我这里貌似设置后并没有什么效果
    glClearDepth(1.0);//设置深度缓存
    glEnable(GL_DEPTH_TEST);//允许深度测试
    glDepthFunc(GL_LEQUAL);//设置深度测试类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正

}



void GLWidget::timerEvent(QTimerEvent *)
{
    updateGL();
}


void GLWidget::setLight(){
  GLfloat light_position[] = {10.0, 10.0f, 10.0f, 0.0}; //设置光源
  GLfloat lmodel_ambient[] = {1.0, 1.0, 1.0, 1.0};
  glShadeModel(GL_SMOOTH);

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lmodel_ambient);


  if(enableLight) glEnable(GL_LIGHTING); //是否开启光源
  else glDisable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  float mat_diffuse_table[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  float mat_specular_table[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  float mat_ambient_table[] = { 0.2f, 0.2f, 0.2f, 1.0f };

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_table);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular_table);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_table);
}

void GLWidget::paintGL()
{
    //glClear()函数在这里就是对initializeGL()函数中设置的颜色和缓存深度等起作用
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camera_x+eyex,camera_y+eyey,camera_z+eyez,camera_x,camera_y,camera_z,upx,upy,upz);

    setLight();

    if(isWireframe)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //设置线框模式
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glTranslatef(camera_x, camera_y, camera_z);//移到相机位置
    glScalef(main_scale,main_scale,main_scale);

    glPushMatrix();
    //glTranslatef(camera_x/main_scale, camera_y/main_scale, camera_z/main_scale);//移到相机位置
    drawCoordinate(); //画一个坐标系
    glScalef(0.001,0.001,0.001);
    //glTranslatef(-2.0, -8.0, 0.0);

    glRotatef(180.0+CurrentAngleZ*180.0/M_PI, 0, 1, 0);

    glRotatef(90.0-CurrentAngleY*180.0/M_PI, 1, 0, 0);
    glTranslatef(-2.0, -8.0, 0.0);


    if(models.size()>0) models[0].callDisplayList();
    glPopMatrix();


    glScalef(0.005,0.005,0.005);
    glTranslatef(main_x, main_y, main_z);
    glTranslatef(-camera_x, -camera_y, -camera_z);//移会原世界中心

    for(unsigned int i=1; i<models.size(); i++) {
        //glTranslatef(50.0, 0.0, 0.0);
        models[i].callDisplayList();
        //models[i].drawOBJ();
      }
}

//该程序是设置opengl场景透视图，程序中至少被执行一次(程序启动时).
void GLWidget::resizeGL(int width, int height)
{
    if(0 == height)
        height = 1;//防止一条边为0
    glViewport(0, 0, (GLint)width, (GLint)height);//重置当前视口，本身不是重置窗口的，只不过是这里被Qt给封装好了
    glMatrixMode(GL_PROJECTION);//选择投影矩阵
    glLoadIdentity();//重置选择好的投影矩阵
    gluPerspective(45.0, (GLfloat)width/(GLfloat)height, 0.1f, 500.0f);//建立透视投影矩阵

    glMatrixMode(GL_MODELVIEW);//以下2句和上面出现的解释一样
    glLoadIdentity();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        //
        case Qt::Key_Space:{
          if(objRotate) objRotate = false;
          else objRotate = true;
          updateGL();
        }break;
        case Qt::Key_W:{
            float delta_x=0.4;
            camera_x -= delta_x*sin(CurrentAngleZ);// + delta_y*cos(CurrentAngleY)*sin(CurrentAngleZ);
            camera_z -= delta_x*cos(CurrentAngleZ);// + delta_y*cos(CurrentAngleY)*cos(CurrentAngleZ);
            updateGL();
          };break;
        case Qt::Key_A:{
            float delta_x=0.4;
            camera_x -= delta_x*cos(CurrentAngleZ);// + delta_y*cos(CurrentAngleY)*sin(CurrentAngleZ);
            camera_z += delta_x*sin(CurrentAngleZ);// + delta_y*cos(CurrentAngleY)*cos(CurrentAngleZ);
            updateGL();
          };break;
        case Qt::Key_S:{
            float delta_x=0.4;
            camera_x += delta_x*sin(CurrentAngleZ);// + delta_y*cos(CurrentAngleY)*sin(CurrentAngleZ);
            camera_z += delta_x*cos(CurrentAngleZ);// + delta_y*cos(CurrentAngleY)*cos(CurrentAngleZ);
            updateGL();
          };break;
        case Qt::Key_D:{
            float delta_x=0.4;
            camera_x += delta_x*cos(CurrentAngleZ);// + delta_y*cos(CurrentAngleY)*sin(CurrentAngleZ);
            camera_z -= delta_x*sin(CurrentAngleZ);// + delta_y*cos(CurrentAngleY)*cos(CurrentAngleZ);
            updateGL();
          };break;
      default:break;
      }

}

GLWidget::~GLWidget()
{
    delete ui;
}
