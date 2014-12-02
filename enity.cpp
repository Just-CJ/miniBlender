#include "enity.h"
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
#include <QDebug>
#include <qmath.h>

model createCube()
{
    model cube;
    vpoint vp[8];
    vp[0].values( 50, 50, 50);
    vp[1].values( 50, 50,-50);
    vp[2].values( 50,-50, 50);
    vp[3].values( 50,-50,-50);
    vp[4].values(-50,-50, 50);
    vp[5].values(-50,-50,-50);
    vp[6].values(-50, 50, 50);
    vp[7].values(-50, 50,-50);
    face f[6];
    f[0].values_v(1,2,4,3);
    f[1].values_v(1,2,8,7);
    f[2].values_v(1,3,5,7);
    f[3].values_v(5,6,8,7);
    f[4].values_v(2,4,6,8);
    f[5].values_v(3,4,6,5);
    for(int i = 0; i < 6; i++){
        f[i].values_vn(0,0,0,0);
        f[i].values_vt(0,0,0,0);
    }

    for(int i = 0; i < 8; i++){
        cube.vpoints.push_back(vp[i]);
    }
    for(int i = 0; i < 6; i++){
        cube.faces.push_back(f[i]);
    }
    cube.genDisplayList();
    return cube;
}

Prismoid::Prismoid(int fn, float h, float r, float rt){
    faceNum = fn;
    height = h;
    this->r = r;
    rTop = rt;
}

model Prismoid::createPrismoid(){
    model prism;
    vpoint vp;
    //顶面中心
    vp.values(0, 0, 0.5*height);
    prism.vpoints.push_back(vp);
    //顶面坐标
    float z = height/2;
    for(int i = 0; i< faceNum; i++){
        float theta = i * 2*M_PI/faceNum;
        float x = rTop*cos(theta);
        float y = rTop*sin(theta);
        vp.values(x, y, z);
        prism.vpoints.push_back(vp);
    }
    //底面中心
    vp.values(0, 0, -0.5*height);
    prism.vpoints.push_back(vp);
    //底面坐标
    z = -height/2;
    for(int i = 0; i< faceNum; i++){
        float theta = i * 2*M_PI/faceNum;
        float x = r*cos(theta);
        float y = r*sin(theta);
        vp.values(x, y, z);
        prism.vpoints.push_back(vp);
    }

    face f;
    f.values_vn(0, 0, 0, 0);
    f.values_vt(0, 0, 0, 0);
    for(int i = 0; i< faceNum; i++){
        //顶面
        if(i == faceNum - 1){
            f.values_v(1, i+2, 2);
        }
        else{
            f.values_v(1, i+2, i+3);
        }
        prism.faces.push_back(f);

        //侧面
        if(i == faceNum - 1){
            f.values_v(i+2, 2, 3+faceNum, i+3+faceNum);
        }
        else{
            f.values_v(i+2, i+3, i+4+faceNum, i+3+faceNum);
        }
        prism.faces.push_back(f);

        //底面
        if(i == faceNum - 1){
            f.values_v(faceNum+2, i+3+faceNum, 3+faceNum);
        }
        else{
            f.values_v(faceNum+2, i+3+faceNum, i+4+faceNum);
        }
        prism.faces.push_back(f);
    }
    prism.genDisplayList();
    prism.max_X = prism.max_Y = r;
    prism.min_X = prism.min_Y = -r;
    prism.max_Z = height/2;
    prism.min_Z = -height/2;
    return prism;
}
