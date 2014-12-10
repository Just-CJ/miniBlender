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
//#include <QtGui>
#include <QMouseEvent>
#include <QtCore>
//#include <QtOpenGL/QtOpenGL>

#define SELBUFSIZE 512

using namespace std;


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

    addLight=false;
    lightnumber=0;

    startTimer(10);//开启10ms定时器

    /*初始化成员*/
    initOBJ();

    CursorMode = ORBIT;
    PanAngle = 0.0;

    beRotate = false;
    global_rotate = 0.0;

    selectedID = 0;//0表示没有物体被选中

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

    world_center_x = 0.0;
    world_center_z = 0.0;

    CurrentAngleZ=0;
    CurrentAngleY=0;
    LastAngleZ=M_PI/4;
    LastAngleY=M_PI/4;

    RotateViewPoint();
    TranslateViewPoint();
}


void GLWidget::RotateViewPoint()
{
    if(CursorMode == ORBIT){
      float avAnale=M_PI/180*0.6; //把每次移动的角度单位化

      /*把每次移动点跟开始按下鼠标记录的点作差，然后乘以avAngle,最后把上一次释放鼠标后时记录的
        角度相加起来*/
      //if(!selectedID){
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
    else if(CursorMode == PAN){
              float avAnale=M_PI/180*0.6; //把每次移动的角度单位化

              /*把每次移动点跟开始按下鼠标记录的点作差，然后乘以avAngle,最后把上一次释放鼠标后时记录的
              角度相加起来*/
              PanAngle = -(EndPoint.x()-StartPoint.x())*avAnale;
              PanAngle += LastPanAngle;

              GLfloat upx_new = lastupx*cos(-PanAngle)-lastupz*sin(-PanAngle);
              GLfloat upz_new = lastupx*sin(-PanAngle)+lastupz*cos(-PanAngle);

              GLfloat tmpx = main_scale*sin(CurrentAngleY)*sin(CurrentAngleZ)*cos(-PanAngle)-main_scale*sin(CurrentAngleY)*cos(CurrentAngleZ)*sin(-PanAngle);
              GLfloat tmpz = main_scale*sin(CurrentAngleY)*sin(CurrentAngleZ)*sin(-PanAngle)+main_scale*sin(CurrentAngleY)*cos(CurrentAngleZ)*cos(-PanAngle);

              delta_Panx = main_scale*sin(CurrentAngleY)*sin(CurrentAngleZ) - tmpx;
              delta_Panz = main_scale*sin(CurrentAngleY)*cos(CurrentAngleZ) - tmpz;

              upx = upx_new;
              upz = upz_new;
          }
}

void GLWidget::TranslateViewPoint(){
  GLfloat delta_x = 0.5*(EndPoint.x()-StartPoint.x());
  GLfloat delta_y = 0.5*(EndPoint.y()-StartPoint.y());

  main_x = LastMain_x + delta_x*cos(CurrentAngleZ) + delta_y*cos(CurrentAngleY)*sin(CurrentAngleZ);
  main_y = LastMain_y - delta_y*sin(CurrentAngleY);
  main_z = LastMain_z - delta_x*sin(CurrentAngleZ) + delta_y*cos(CurrentAngleY)*cos(CurrentAngleZ);

  if(selectedID){
    models[selectedID-1].offset_x = LastOBJ_x + delta_x*cos(CurrentAngleZ) + delta_y*cos(CurrentAngleY)*sin(CurrentAngleZ);
    models[selectedID-1].offset_y = LastOBJ_y - delta_y*sin(CurrentAngleY);
    models[selectedID-1].offset_z = LastOBJ_z - delta_x*sin(CurrentAngleZ) + delta_y*cos(CurrentAngleY)*cos(CurrentAngleZ);
    }
  emit signal_updateAttr(selectedID);
}


void GLWidget::processHits(GLint hits, GLuint buffer[]){
     int i;
     unsigned int j;
     GLuint names, *ptr;
     GLuint minZ;
     unsigned int SelectedID = 0;

     ptr = (GLuint *) buffer;
     for (i = 0; i < hits; i++) {  /* for each hit  */
        names = *ptr;
        ptr++;
        if(i==0){//至少存在一个命中时
            minZ = *ptr; ptr+=2;
            for (j = 0; j < names; j++) {  /* for each name */
               SelectedID = *ptr; ptr++;
            }
          }
        else if(*ptr<minZ){ //有两个以上命中时进行比较
            minZ = *ptr; ptr+=2;
            for (j = 0; j < names; j++) {  /* for each name */
               SelectedID = *ptr; ptr++;
            }
          }
        else{
            ptr+=2;
            for (j = 0; j < names; j++) {  /* for each name */
               ptr++;
            }
          }
     }

     if(SelectedID){//存在命中对象
         if(selectedID != SelectedID && selectedID!=0){
             models[selectedID-1].isSelected = false;//选中了别的对象，取消前一个对象的选中状态，保证只有一个对象被选中
             models[SelectedID-1].isSelected = true;
             selectedID = SelectedID;
             for(int i=0; i<16; i++)
               lastRotateMatrix[i] = models[selectedID-1].rotateMatrix[i];
             emit model_select();
           }
         else if(selectedID == 0){
             models[SelectedID-1].isSelected = true;
             selectedID = SelectedID;
             for(int i=0; i<16; i++)
               lastRotateMatrix[i] = models[selectedID-1].rotateMatrix[i];
             emit model_select();
           }
         else if(selectedID == SelectedID){
             models[selectedID-1].isSelected = false;//选中的物体再次双击时取消选中
             selectedID = 0;
             emit model_select();
           }
       }
}

void GLWidget::modelSelect(unsigned int SelectedID){
    if(selectedID != SelectedID && selectedID!=0){
        models[selectedID-1].isSelected = false;//选中了别的对象，取消前一个对象的选中状态，保证只有一个对象被选中
        models[SelectedID-1].isSelected = true;
        selectedID = SelectedID;
        for(int i=0; i<16; i++)
          lastRotateMatrix[i] = models[selectedID-1].rotateMatrix[i];
        emit model_select();
    }
    else if(selectedID == 0){
        models[SelectedID-1].isSelected = true;
        selectedID = SelectedID;
        for(int i=0; i<16; i++)
          lastRotateMatrix[i] = models[selectedID-1].rotateMatrix[i];
        emit model_select();
    }
    else if(selectedID == SelectedID){
        models[selectedID-1].isSelected = false;//选中的物体再次双击时取消选中
        selectedID = 0;
        emit model_select();
    }

    //emit sendSelectLightMTL(2);
    //emit signal_updateAttr(selectedID);

}

void GLWidget::selectModel(int x, int y){
  GLuint selectBuf[SELBUFSIZE];
  GLint hits;
  GLint viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);//获取当前视口边界
  glSelectBuffer (SELBUFSIZE, selectBuf);
  (void) glRenderMode(GL_SELECT);

  glInitNames();
  glPushName(0);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5.0, 5.0, viewport);//5*5的区域进行选取

  gluPerspective(45.0, whRatio, 0.1f, 500.0f);//建立透视投影矩阵
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(camera_x+world_center_x+eyex*main_scale,
            camera_y+eyey*main_scale,
            camera_z+world_center_z+eyez*main_scale,
            camera_x+delta_Panx+world_center_x,
            camera_y,
            camera_z+delta_Panz+world_center_z,
            upx,upy,upz);

  glTranslatef(camera_x+world_center_x, camera_y, camera_z+world_center_z);//移到相机位置
  glRotatef(global_rotate, 0, 1, 0); //全局旋转动画
  glTranslatef(-camera_x-world_center_x, -camera_y, -camera_z-world_center_z);//移回原世界中心


  glScalef(0.005,0.005,0.005);

  for(unsigned int i=0; i<models.size(); i++){
      glLoadName(i+1);
      glPushMatrix();
      glTranslatef(models[i].offset_x, models[i].offset_y, models[i].offset_z);
      glScalef(models[i].scale_x, models[i].scale_y, models[i].scale_z);
      glMultMatrixf(models[i].rotateMatrix);

      models[i].callDisplayList();
      glPopMatrix();
    }

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  hits = glRenderMode(GL_RENDER);
  if(hits)//命中
    processHits(hits, selectBuf);
  else{//没有命中
      if(selectedID) models[selectedID-1].isSelected = false; //取消原先物体的选中状态
      selectedID = 0;
      emit model_select();
    }

  //emit sendSelectLightMTL(2);
  emit signal_updateAttr(selectedID);
  glMatrixMode(GL_MODELVIEW);
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
        if(selectedID){
            LastOBJ_x = models[selectedID-1].offset_x;
            LastOBJ_y = models[selectedID-1].offset_y;
            LastOBJ_z = models[selectedID-1].offset_z;
          }
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
        LastPanAngle=PanAngle;
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
    if(selectedID){
        if(e->delta() > 0) {
            models[selectedID-1].scale_x += models[selectedID-1].scale_x*0.1f;
            models[selectedID-1].scale_y += models[selectedID-1].scale_y*0.1f;
            models[selectedID-1].scale_z += models[selectedID-1].scale_z*0.1f;
        }else{
            models[selectedID-1].scale_x -= models[selectedID-1].scale_x*0.1f;
            models[selectedID-1].scale_y -= models[selectedID-1].scale_y*0.1f;
            models[selectedID-1].scale_z -= models[selectedID-1].scale_z*0.1f;
        }
      }
    else if(CursorMode == ORBIT) e->delta() > 0 ? main_scale -= main_scale*0.1f : main_scale += main_scale*0.1f;

    emit signal_updateAttr(selectedID);
    this->updateGL();
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *e){
  switch(e->buttons()){
      case Qt::LeftButton :{
        selectModel(e->pos().x(), e->pos().y());
      };break;
      default:break;
    }
}



//初始化
void GLWidget::initializeGL()
{
    setGeometry(0, 0, 910, 660);//设置窗口初始位置和大小
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
  glShadeModel(GL_SMOOTH);
  if(!enableLight)
      glDisable(GL_LIGHTING);
  else
      glEnable(GL_LIGHTING);
  for(int i=0;i<8;i++){
  if(addLight==true && lights[i].isOn==false){
  addLight=false;
  lights[i].isOn=true;
  }
  if(!lights[i].isOn){
      switch(i){
      case 0:{
              glDisable(GL_LIGHT0);
          break;}
      case 1:{
              glDisable(GL_LIGHT1);
          break;}
      case 2:{
              glDisable(GL_LIGHT2);
          break;}
      case 3:{
          glDisable(GL_LIGHT3);
          break;}
      case 4:{
          glDisable(GL_LIGHT4);
          break;}
      case 5:{
          glDisable(GL_LIGHT5);
          break;}
      case 6:{
          glDisable(GL_LIGHT6);
          break;}
      case 7:{
          glDisable(GL_LIGHT7);
          }
      }
  }
  if(lights[i].isOn){
  switch(i+1){
  case 1:{
          glLightfv(GL_LIGHT0,GL_AMBIENT,lights[0].light_ambient);
          glLightfv(GL_LIGHT0,GL_DIFFUSE,lights[0].light_diffuse);
          glLightfv(GL_LIGHT0,GL_SPECULAR,lights[0].light_specular);
          glLightfv(GL_LIGHT0,GL_POSITION,lights[0].light_position);
          glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,lights[0].intensity);
          glEnable(GL_LIGHT0);
      break;}
  case 2:{
          glLightfv(GL_LIGHT1,GL_AMBIENT,lights[1].light_ambient);
          glLightfv(GL_LIGHT1,GL_DIFFUSE,lights[1].light_diffuse);
          glLightfv(GL_LIGHT1,GL_SPECULAR,lights[1].light_specular);
          glLightfv(GL_LIGHT1,GL_POSITION,lights[1].light_position);
          glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,lights[1].intensity);
          glEnable(GL_LIGHT1);
      break;}
  case 3:{
          glLightfv(GL_LIGHT2,GL_AMBIENT,lights[2].light_ambient);
          glLightfv(GL_LIGHT2,GL_DIFFUSE,lights[2].light_diffuse);
          glLightfv(GL_LIGHT2,GL_SPECULAR,lights[2].light_specular);
          glLightfv(GL_LIGHT2,GL_POSITION,lights[2].light_position);
          glLightf(GL_LIGHT2,GL_CONSTANT_ATTENUATION,lights[2].intensity);
          glEnable(GL_LIGHT2);
      break;}
  case 4:{
      glLightfv(GL_LIGHT3,GL_AMBIENT,lights[3].light_ambient);
      glLightfv(GL_LIGHT3,GL_DIFFUSE,lights[3].light_diffuse);
      glLightfv(GL_LIGHT3,GL_SPECULAR,lights[3].light_specular);
      glLightfv(GL_LIGHT3,GL_POSITION,lights[3].light_position);
      glLightf(GL_LIGHT3,GL_CONSTANT_ATTENUATION,lights[3].intensity);
      glEnable(GL_LIGHT3);
      break;}
  case 5:{
      glLightfv(GL_LIGHT4,GL_AMBIENT,lights[4].light_ambient);
      glLightfv(GL_LIGHT4,GL_DIFFUSE,lights[4].light_diffuse);
      glLightfv(GL_LIGHT4,GL_SPECULAR,lights[4].light_specular);
      glLightfv(GL_LIGHT4,GL_POSITION,lights[4].light_position);
      glLightf(GL_LIGHT4,GL_CONSTANT_ATTENUATION,lights[4].intensity);
      glEnable(GL_LIGHT4);
      break;}
  case 6:{
      glLightfv(GL_LIGHT5,GL_AMBIENT,lights[5].light_ambient);
      glLightfv(GL_LIGHT5,GL_DIFFUSE,lights[5].light_diffuse);
      glLightfv(GL_LIGHT5,GL_SPECULAR,lights[5].light_specular);
      glLightfv(GL_LIGHT5,GL_POSITION,lights[5].light_position);
      glLightf(GL_LIGHT5,GL_CONSTANT_ATTENUATION,lights[5].intensity);
      glEnable(GL_LIGHT5);
      break;}
  case 7:{
      glLightfv(GL_LIGHT6,GL_AMBIENT,lights[6].light_ambient);
      glLightfv(GL_LIGHT6,GL_DIFFUSE,lights[6].light_diffuse);
      glLightfv(GL_LIGHT6,GL_SPECULAR,lights[6].light_specular);
      glLightfv(GL_LIGHT6,GL_POSITION,lights[6].light_position);
      glLightf(GL_LIGHT6,GL_CONSTANT_ATTENUATION,lights[6].intensity);
      glEnable(GL_LIGHT6);
      break;}
  case 8:{
      glLightfv(GL_LIGHT7,GL_AMBIENT,lights[7].light_ambient);
      glLightfv(GL_LIGHT7,GL_DIFFUSE,lights[7].light_diffuse);
      glLightfv(GL_LIGHT7,GL_SPECULAR,lights[7].light_specular);
      glLightfv(GL_LIGHT7,GL_POSITION,lights[7].light_position);
      glLightf(GL_LIGHT7,GL_CONSTANT_ATTENUATION,lights[7].intensity);
      glEnable(GL_LIGHT7);
      }
  }
  }
  }

  glEnable(GL_DEPTH_TEST);

  float mat_diffuse_table[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  float mat_specular_table[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  float mat_ambient_table[] = { 0.2f, 0.2f, 0.2f, 1.0f };

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_table);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular_table);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_table);
}

void GLWidget::drawSelectCube(){
  glLineWidth(3);
  glBegin(GL_LINE_LOOP);
  glColor4f(0.5, 0.5, 0.5, 1.0);
  glVertex3f(-0.5,-0.5,-0.5);
  glVertex3f(0.5,-0.5,-0.5);
  glVertex3f(0.5,-0.5,0.5);
  glVertex3f(-0.5,-0.5,0.5);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex3f(-0.5,0.5,-0.5);
  glVertex3f(0.5,0.5,-0.5);
  glVertex3f(0.5,0.5,0.5);
  glVertex3f(-0.5,0.5,0.5);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(-0.5,0.5,-0.5);
  glVertex3f(-0.5,-0.5,-0.5);
  glVertex3f(0.5,0.5,-0.5);
  glVertex3f(0.5,-0.5,-0.5);
  glVertex3f(0.5,0.5,0.5);
  glVertex3f(0.5,-0.5,0.5);
  glVertex3f(-0.5,0.5,0.5);
  glVertex3f(-0.5,-0.5,0.5);
  glEnd();

  glColor4f(0.5, 0.5, 0.5, 0.2);
  glutSolidCube(1);
}

void GLWidget::drawGrid(GLfloat width, int num){
  //GLfloat width = 0.1;
  //int num = 20;
  glPushMatrix();
    glBegin(GL_LINES);
    glColor3f(0.5,0.5,0.5);
    glVertex3f(num*width, 0, 0);
    glVertex3f(-num*width, 0, 0);
    glVertex3f(0.0, 0, num*width);
    glVertex3f(0.0, 0, -num*width); //先画一个十字

    for(int i = 1; i<=num; i++){
        glVertex3f(num*width, 0, i*width);//x轴两侧
        glVertex3f(-num*width, 0, i*width);
        glVertex3f(num*width, 0, -i*width);//x轴两侧
        glVertex3f(-num*width, 0, -i*width);
        glVertex3f(i*width, 0, num*width);//z轴两侧
        glVertex3f(i*width, 0, -num*width);
        glVertex3f(-i*width, 0, num*width);//z轴两侧
        glVertex3f(-i*width, 0, -num*width);
      }
    glEnd();
  glPopMatrix();
}

void GLWidget::paintGL()
{
    //glClear()函数在这里就是对initializeGL()函数中设置的颜色和缓存深度等起作用
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camera_x+world_center_x+eyex*main_scale,
                  camera_y+eyey*main_scale,
                  camera_z+world_center_z+eyez*main_scale,
                  camera_x+delta_Panx+world_center_x,
                  camera_y,
                  camera_z+delta_Panz+world_center_z,
                  upx,upy,upz);

    setLight();

    if(isWireframe)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //设置线框模式
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glTranslatef(camera_x+world_center_x, camera_y, camera_z+world_center_z);//移到相机位置

    glPushMatrix();//-----------------------漫游中心----------------------------------
    glTranslatef((delta_Panx), 0, (delta_Panz));//移到相机位置
    drawCoordinate(); //画一个坐标系
    glPopMatrix();//-------------------------------------

    glRotatef(global_rotate, 0, 1, 0); //全局旋转动画

    glTranslatef(-camera_x-world_center_x, -camera_y, -camera_z-world_center_z);//移回原世界中心
    glScalef(0.005,0.005,0.005);

    drawGrid(0.1*200, 20);//画一个网格平面

    for(unsigned int i=0; i<models.size(); i++) {//循环绘制model
        glPushMatrix();
        glTranslatef(models[i].offset_x, models[i].offset_y, models[i].offset_z);//model自身的位移
        //glScalef(models[i].scale, models[i].scale, models[i].scale);//model自身的放大缩小
        glScalef(models[i].scale_x, models[i].scale_y, models[i].scale_z);

        glMultMatrixf(models[i].rotateMatrix);//乘上旋转矩阵

        models[i].callDisplayList();//调用model自己的显示列表

        if(models[i].isSelected){//表示选中的框
            glPushMatrix();
            glPushAttrib(GL_ALL_ATTRIB_BITS);
              glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);glDisable(GL_LIGHTING);
              glBlendFunc(GL_SRC_ALPHA, GL_ONE); // 基于源象素alpha通道值的半透明混合函数
              glScalef(models[i].max_X-models[i].min_X, models[i].max_Y-models[i].min_Y, models[i].max_Z-models[i].min_Z);
              drawSelectCube();
            glPopAttrib();
            glPopMatrix();
            glScalef(100,100,100);
            glDisable(GL_DEPTH_TEST);//让坐标系总显示在最前
            glScalef(1.0/models[i].scale_x, 1.0/models[i].scale_y, 1.0/models[i].scale_z);
            drawCoordinate(); //obj的中心坐标系
            glEnable(GL_DEPTH_TEST);
          }
        glPopMatrix();
      }


    if(beRotate) global_rotate+=1.0f;
}

//该程序是设置opengl场景透视图，程序中至少被执行一次(程序启动时).
void GLWidget::resizeGL(int width, int height)
{
    if(0 == height)
        height = 1;//防止一条边为0
    glViewport(0, 0, (GLint)width, (GLint)height);//重置当前视口
    glMatrixMode(GL_PROJECTION);//选择投影矩阵
    glLoadIdentity();//重置选择好的投影矩阵
    whRatio = (GLfloat)width/(GLfloat)height;
    gluPerspective(45.0, whRatio, 0.1f, 500.0f);//建立透视投影矩阵

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
            camera_x -= delta_x*sin(CurrentAngleZ);
            camera_z -= delta_x*cos(CurrentAngleZ);
            updateGL();
          };break;
        case Qt::Key_A:{
            float delta_x=0.4;
            camera_x -= delta_x*cos(CurrentAngleZ);
            camera_z += delta_x*sin(CurrentAngleZ);
            updateGL();
          };break;
        case Qt::Key_S:{
            float delta_x=0.4;
            camera_x += delta_x*sin(CurrentAngleZ);
            camera_z += delta_x*cos(CurrentAngleZ);
            updateGL();
          };break;
        case Qt::Key_D:{
            float delta_x=0.4;
            camera_x += delta_x*cos(CurrentAngleZ);
            camera_z -= delta_x*sin(CurrentAngleZ);
            updateGL();
          };break;
      default:break;
      }

}

GLWidget::~GLWidget()
{
    delete ui;
}
