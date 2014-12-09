#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "OBJ.h"
//#include <GL/glew.h>
#include <QtOpenGL>
#include <QGLWidget>
#include <QWidget>
#include <string>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
//#include <gl/freeglut.h>


enum CursorModeType{
  ORBIT,
  PAN
};

namespace Ui {
class GLWidget;
}

class light{
public:
    bool isOn;
    GLfloat light_position[4];
    GLfloat light_ambient[4];
    GLfloat light_diffuse[4];
    GLfloat light_specular[4];
    int intensity;
    light(){
        GLfloat lp[4]={10.0, 10.0f, 10.0f, 0.0};
        GLfloat la[4]={1, 1, 1, 1.0};
        GLfloat ld[4]={0.2, 0.2, 0.2, 1.0};
        GLfloat ls[4]={0, 0, 0, 1.0};
        for(int i=0;i<4;i++){
            light_position[i]=lp[i];
            light_ambient[i]=la[i];
            light_diffuse[i]=ld[i];
            light_specular[i]=ls[i];
        }
        intensity=1;
        isOn=false;
    }
};

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    CursorModeType CursorMode;
    bool isWireframe;
    bool enableLight;
    bool addLight;
    int lightnumber;
    light lights[8];
    GLfloat *objCenter;
    unsigned int selectedID;//场景中被选中的物体
    float lastRotateMatrix[16];     //上一次的旋转矩阵

    bool  beRotate;
    float global_rotate;

    /*gluLookAt函数中第一组坐标跟第三组坐标*/
    GLdouble camera_x;
    GLdouble camera_y;
    GLdouble camera_z;

    GLdouble eyex;
    GLdouble eyey;
    GLdouble eyez;

    GLdouble upx;
    GLdouble upy;
    GLdouble upz;

    GLdouble lastupx;
    GLdouble lastupy;
    GLdouble lastupz;

    GLdouble delta_Panx;
    GLdouble delta_Panz;

    GLdouble world_center_x;
    GLdouble world_center_z;

    float CurrentAngleZ;     //当前与Z轴的夹角
    float CurrentAngleY;     //当前与Y轴的夹角

    float LastAngleZ;        //上一次与Z轴的夹角
    float LastAngleY;        //上一次与Y轴的夹角

    GLfloat PanAngle;


    explicit GLWidget(QGLWidget *parent = 0);
    ~GLWidget();

    void initOBJ();

signals:
    void model_select();
    void sendSelectLightMTL(int index);
    void signal_updateAttr(unsigned int selectedID);

//Added to select by treeView --Lkx
public slots:
    void modelSelect(unsigned int SelectedID);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void RotateViewPoint();  //用鼠标旋转视点
    void TranslateViewPoint();
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void timerEvent(QTimerEvent *);
    void setLight();
    void selectModel(int x, int y);
    void processHits(GLint hits, GLuint buffer[]);
    void drawSelectCube();
    void drawGrid(GLfloat width, int num);





    bool fullscreen;

private:
    Ui::GLWidget *ui;



    GLfloat whRatio;

    GLfloat main_scale;

    QPoint StartPoint;     //记录鼠标按下的点
    QPoint EndPoint;       //鼠标移动过程中的点



    GLfloat main_x;          //全局xyz偏移
    GLfloat main_y;
    GLfloat main_z;

    GLfloat LastMain_x;          //上一次全局xyz偏移
    GLfloat LastMain_y;
    GLfloat LastMain_z;

    GLfloat LastOBJ_x;          //上一次选中物体xyz偏移
    GLfloat LastOBJ_y;
    GLfloat LastOBJ_z;

    GLfloat LastPanAngle;

    bool objRotate;
    GLfloat objRotate_x;



};

#endif // GLWIDGET_H
