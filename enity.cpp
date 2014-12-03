#include "enity.h"
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
#include <QDebug>
#include <qmath.h>

model createCube(float l)
{
    model cube;
    vpoint vp[8];
    vp[0].values( l, l, l);
    vp[1].values( l, l,-l);
    vp[2].values( l,-l, l);
    vp[3].values( l,-l,-l);
    vp[4].values(-l,-l, l);
    vp[5].values(-l,-l,-l);
    vp[6].values(-l, l, l);
    vp[7].values(-l, l,-l);
    vnormal vn[6];
    vn[0].values(1, 0, 0);
    vn[1].values(0, 1, 0);
    vn[2].values(0, 0, 1);
    vn[3].values(-1, 0, 0);
    vn[4].values(0, -1, 0);
    vn[5].values(0, 0, -1);
    face f[6];
    f[0].values_v(1,2,4,3); f[0].values_vn(1, 1, 1, 1);
    f[1].values_v(1,2,8,7); f[1].values_vn(2, 2, 2, 2);
    f[2].values_v(1,3,5,7); f[2].values_vn(3, 3, 3, 3);
    f[3].values_v(5,6,8,7); f[3].values_vn(4, 4, 4, 4);
    f[4].values_v(2,4,6,8); f[4].values_vn(6, 6, 6, 6);
    f[5].values_v(3,4,6,5); f[5].values_vn(5, 5, 5, 5);
    for(int i = 0; i < 6; i++){
        f[i].values_vt(0,0,0,0);
    }

    for(int i = 0; i < 8; i++){
        cube.vpoints.push_back(vp[i]);
    }
    for(int i = 0; i < 6; i++){
        cube.vnormals.push_back(vn[i]);
    }
    for(int i = 0; i < 6; i++){
        cube.faces.push_back(f[i]);
    }
    cube.genDisplayList();
    cube.max_X = cube.max_Y = cube.max_Z = l*1.03;
    cube.min_X = cube.min_Y = cube.min_Z = -l*1.03;
    cube.entityAttr.push_back(l);
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

    //计算法向量,index1为顶面，2为底面，剩下faceNum为侧面
    vnormal vn;
    vn.values(0, 0, 1);
    prism.vnormals.push_back(vn);
    vn.values(0, 0, -1);
    prism.vnormals.push_back(vn);
    for(int i = 0; i< faceNum; i++){
        float x1 = prism.vpoints[i+3].x;
        float y1 = prism.vpoints[i+3].y;
        float x2, y2;
        if(i != faceNum-1){
            x2 = prism.vpoints[i+4].x;
            y2 = prism.vpoints[i+4].y;
        }
        else{
            x2 = prism.vpoints[3].x;
            y2 = prism.vpoints[3].y;
        }
        float x = (x1 + x2)/2;
        float y = (y1 + y2)/2;
        vn.values(x, y, 0);
        prism.vnormals.push_back(vn);
    }

    face f;
    //f.values_vn(0, 0, 0, 0);
    f.values_vt(0, 0, 0, 0);
    for(int i = 0; i< faceNum; i++){
        //顶面
        if(i == faceNum - 1){
            f.values_v(1, i+2, 2);
        }
        else{
            f.values_v(1, i+2, i+3);
        }
        //顶面法向量都是z轴
        f.values_vn(1, 1, 1);
        prism.faces.push_back(f);

        //侧面
        if(i == faceNum - 1){
            f.values_v(i+2, 2, 3+faceNum, i+3+faceNum);
        }
        else{
            f.values_v(i+2, i+3, i+4+faceNum, i+3+faceNum);
        }
        f.values_vn(i+3, i+3, i+3, i+3);
        prism.faces.push_back(f);

        //底面
        if(i == faceNum - 1){
            f.values_v(faceNum+2, i+3+faceNum, 3+faceNum);
        }
        else{
            f.values_v(faceNum+2, i+3+faceNum, i+4+faceNum);
        }
        f.values_vn(2, 2, 2);
        prism.faces.push_back(f);
    }
    prism.genDisplayList();
    prism.max_X = prism.max_Y = r;
    prism.min_X = prism.min_Y = -r;
    prism.max_Z = height/2;
    prism.min_Z = -height/2;
    prism.entityAttr.push_back(faceNum);
    prism.entityAttr.push_back(height);
    prism.entityAttr.push_back(r);
    prism.entityAttr.push_back(rTop);
    return prism;
}

Sphere::Sphere(float r, int dens){
    this->r = r;
    this->density = dens;
}

model Sphere::createSphere(){
    model sphere;
    float stepZ = M_PI/density;
    float stepXY = 2 * M_PI/density;
    float x, y, z;

    float angleZ = 0;
    float angleXY = 0;

    vpoint vp;
    vnormal vn;
    face f;
    f.values_vt(0, 0, 0, 0);

    for(int i = 0; i<= density; i++){
        angleZ = i * stepZ;
        for(int j = 0; j<= density; j++){
            angleXY = j * stepXY;

            x = r * sin(angleZ) * cos(angleXY);
            y = r * sin(angleZ) * sin(angleXY);
            z = r * cos(angleZ);

            vp.values(x, y, z);
            sphere.vpoints.push_back(vp);
            vn.values(x, y, z);
            sphere.vnormals.push_back(vn);
        }
    }


    for(int i = 0; i< sphere.vpoints.size()-density; i++){
        f.values_v(i+1, i+1+density, i+2+density, i+2);
        f.values_vn(i+1, i+1+density, i+2+density, i+2);
        sphere.faces.push_back(f);
    }

    sphere.genDisplayList();
    sphere.entityAttr.push_back(r);
    sphere.entityAttr.push_back(density);
    sphere.max_X = sphere.max_Y = sphere.max_Z = r;
    sphere.min_X = sphere.min_Y = sphere.min_Z = -r;
    return sphere;
}
