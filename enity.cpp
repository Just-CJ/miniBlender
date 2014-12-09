#include "enity.h"
//#define GLUT_DISABLE_ATEXIT_HACK
//#include <GL/glut.h>
#include <QtOpenGL>
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
    vtex vt[4];
    vt[0].u = 0; vt[0].v = 0;
    vt[1].u = 1; vt[1].v = 0;
    vt[2].u = 1; vt[2].v = 1;
    vt[3].u = 0; vt[3].v = 1;
    vnormal vn[6];
    vn[0].values(1, 0, 0);
    vn[1].values(0, 1, 0);
    vn[2].values(0, 0, 1);
    vn[3].values(-1, 0, 0);
    vn[4].values(0, -1, 0);
    vn[5].values(0, 0, -1);
    face f[6];
    f[0].values_v(1,2,4,3);
    f[0].values_vn(1, 1, 1, 1);
    f[1].values_v(1,2,8,7);
    f[1].values_vn(2, 2, 2, 2);
    f[2].values_v(1,3,5,7);
    f[2].values_vn(3, 3, 3, 3);
    f[3].values_v(5,6,8,7);
    f[3].values_vn(4, 4, 4, 4);
    f[4].values_v(2,4,6,8);
    f[4].values_vn(6, 6, 6, 6);
    f[5].values_v(3,4,6,5);
    f[5].values_vn(5, 5, 5, 5);
    for(int i = 0; i < 6; i++){
        f[i].values_vt(1,2,3,4);
    }

    for(int i = 0; i < 8; i++){
        cube.vpoints.push_back(vp[i]);
    }
    for(int i = 0; i < 6; i++){
        cube.vnormals.push_back(vn[i]);
    }
    for(int i = 0; i < 4; i++){
        cube.vtexs.push_back(vt[i]);
    }
    for(int i = 0; i < 6; i++){
        cube.faces.push_back(f[i]);
    }

    cube.max_X = cube.max_Y = cube.max_Z = l*1.03;
    cube.min_X = cube.min_Y = cube.min_Z = -l*1.03;
    cube.entityAttr.push_back(l);
    object obj;
    mtl Tmtl;
    obj.vn_num = 6;
    obj.vt_num = 4;
    obj.v_num = 8;
    obj.size = 6;
    obj.ambient[0] = obj.ambient[1] = obj.ambient[2] = Tmtl.ambient[0] = Tmtl.ambient[1] = Tmtl.ambient[2] = 0.2;
    obj.diffuse[0] = obj.diffuse[1] = obj.diffuse[2] = Tmtl.diffuse[0] = Tmtl.diffuse[1] = Tmtl.diffuse[2] = 0.8;
    obj.emission[0] = obj.emission[1] = obj.emission[2] = Tmtl.emission[0] = Tmtl.emission[1] = Tmtl.emission[2] = 0;
    obj.specular[0] = obj.specular[1] = obj.specular[2] = Tmtl.specular[0] = Tmtl.specular[1] = Tmtl.specular[2] = 0;
    obj.texID = Tmtl.texID = 0;

    cube.objects.push_back(obj);
    cube.mtls.push_back(Tmtl);
    cube.calObjCenter();
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

    vtex vt;
    //顶面中心
    vp.values(0, 0, 0.5*height);
    vt.u = 0.5; vt.v = 0.5;
    prism.vpoints.push_back(vp);
    prism.vtexs.push_back(vt);
    //顶面坐标
    float z = height/2;
    for(int i = 0; i< faceNum; i++){
        float theta = i * 2*M_PI/faceNum;
        float x = rTop*cos(theta);
        float y = rTop*sin(theta);
        float xt = 0.5 + 0.5*cos(theta);
        float yt = 0.5 + 0.5*sin(theta);
        vp.values(x, y, z);
        vt.u = xt; vt.v = yt;
        prism.vpoints.push_back(vp);
        prism.vtexs.push_back(vt);
    }
    //底面中心
    vp.values(0, 0, -0.5*height);
    vt.u = 0.5; vt.v = 0.5;
    prism.vpoints.push_back(vp);
    prism.vtexs.push_back(vt);
    //底面坐标
    z = -height/2;
    for(int i = 0; i< faceNum; i++){
        float theta = i * 2*M_PI/faceNum;
        float x = r*cos(theta);
        float y = r*sin(theta);
        float xt = 0.5 + 0.5*cos(theta);
        float yt = 0.5 + 0.5*sin(theta);
        vp.values(x, y, z);
        vt.u = xt; vt.v = yt;
        prism.vpoints.push_back(vp);
        prism.vtexs.push_back(vt);
    }
    //对于侧面，可以导入整幅图像，所以将它的vtex设置为(0, 0)~(1, 1)
    vt.u = 0; vt.v = 0;
    prism.vtexs.push_back(vt);
    vt.v = 1;
    prism.vtexs.push_back(vt);
    vt.u = 1;
    prism.vtexs.push_back(vt);
    vt.v = 0;
    prism.vtexs.push_back(vt);

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
    //f.values_vt(0, 0, 0, 0);
    for(int i = 0; i< faceNum; i++){
        //顶面
        if(i == faceNum - 1){
            f.values_v(1, i+2, 2);
            f.values_vt(1, i+2, 2);
        }
        else{
            f.values_v(1, i+2, i+3);
            f.values_vt(1, i+2, i+3);
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
        f.values_vt(prism.vtexs.size()-3, prism.vtexs.size()-2, prism.vtexs.size()-1, prism.vtexs.size());
        f.values_vn(i+3, i+3, i+3, i+3);
        prism.faces.push_back(f);

        //底面
        if(i == faceNum - 1){
            f.values_v(faceNum+2, i+3+faceNum, 3+faceNum);
            f.values_vt(faceNum+2, i+3+faceNum, 3+faceNum);
        }
        else{
            f.values_v(faceNum+2, i+3+faceNum, i+4+faceNum);
            f.values_vt(faceNum+2, i+3+faceNum, i+4+faceNum);
        }
        f.values_vn(2, 2, 2);
        prism.faces.push_back(f);
    }

    prism.max_X = prism.max_Y = r;
    prism.min_X = prism.min_Y = -r;
    prism.max_Z = height/2;
    prism.min_Z = -height/2;
    prism.entityAttr.push_back(faceNum);
    prism.entityAttr.push_back(height);
    prism.entityAttr.push_back(r);
    prism.entityAttr.push_back(rTop);
    object obj;
    object Tmtl;
    obj.vn_num = prism.vnormals.size();
    obj.vt_num = prism.vtexs.size();
    obj.v_num = prism.vpoints.size();
    obj.size = prism.faces.size();

    obj.ambient[0] = obj.ambient[1] = obj.ambient[2] = Tmtl.ambient[0] = Tmtl.ambient[1] = Tmtl.ambient[2] = 0.2;
    obj.diffuse[0] = obj.diffuse[1] = obj.diffuse[2] = Tmtl.diffuse[0] = Tmtl.diffuse[1] = Tmtl.diffuse[2] = 0.8;
    obj.emission[0] = obj.emission[1] = obj.emission[2] = Tmtl.emission[0] = Tmtl.emission[1] = Tmtl.emission[2] = 0;
    obj.specular[0] = obj.specular[1] = obj.specular[2] = Tmtl.specular[0] = Tmtl.specular[1] = Tmtl.specular[2] = 0;
    obj.texID = Tmtl.texID = 0;
    prism.objects.push_back(obj);
    prism.mtls.push_back(Tmtl);
    prism.calObjCenter();
    prism.genDisplayList();
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
    //f.values_vt(0, 0, 0, 0);
    vtex vt;

    for(int i = 0; i<= density; i++){
        angleZ = i * stepZ;
        for(int j = 0; j<= density; j++){
            angleXY = j * stepXY;

            x = r * sin(angleZ) * cos(angleXY);
            y = r * sin(angleZ) * sin(angleXY);
            z = r * cos(angleZ);

            vt.u = (float)i/(float)density;
            vt.v = (float)j/(float)density;
            sphere.vtexs.push_back(vt);

            vp.values(x, y, z);
            sphere.vpoints.push_back(vp);
            vn.values(x, y, z);
            sphere.vnormals.push_back(vn);
        }
    }


    for(unsigned int i = 0; i< sphere.vpoints.size()-density; i++){
        f.values_v(i+1, i+1+density, i+2+density, i+2);
        f.values_vn(i+1, i+1+density, i+2+density, i+2);
        f.values_vt(i+1, i+1+density, i+2+density, i+2);
        sphere.faces.push_back(f);
    }

    sphere.entityAttr.push_back(r);
    sphere.entityAttr.push_back(density);
    sphere.max_X = sphere.max_Y = sphere.max_Z = r;
    sphere.min_X = sphere.min_Y = sphere.min_Z = -r;
    object obj;
    mtl Tmtl;
    obj.vn_num = sphere.vnormals.size();
    obj.vt_num = sphere.vtexs.size();
    obj.v_num = sphere.vpoints.size();
    obj.size = sphere.faces.size();
    obj.ambient[0] = obj.ambient[1] = obj.ambient[2] = Tmtl.ambient[0] = Tmtl.ambient[1] = Tmtl.ambient[2] = 0.2;
    obj.diffuse[0] = obj.diffuse[1] = obj.diffuse[2] = Tmtl.diffuse[0] = Tmtl.diffuse[1] = Tmtl.diffuse[2] = 0.8;
    obj.emission[0] = obj.emission[1] = obj.emission[2] = Tmtl.emission[0] = Tmtl.emission[1] = Tmtl.emission[2] = 0;
    obj.specular[0] = obj.specular[1] = obj.specular[2] = Tmtl.specular[0] = Tmtl.specular[1] = Tmtl.specular[2] = 0;
    obj.texID = Tmtl.texID = 0;

    sphere.objects.push_back(obj);
    sphere.mtls.push_back(Tmtl);
    sphere.calObjCenter();
    sphere.genDisplayList();
    return sphere;
}

Cone::Cone(float r, float height, int faceNum){
    this->r = r;
    this->height = height;
    this->faceNum = faceNum;
}

model Cone::createCone(){
    model cone;
    vpoint vp;
    vnormal vn;
    vtex vt;
    face f;

    //f.values_vn(0, 0, 0, 0);
    //f.values_vt(0, 0, 0, 0);

    //顶点
    vp.values(0, 0, height/2);
    vn.values(0, 0, 1);
    //把纹理裁成一个等腰三角形再贴上去
    vt.u = 0.5;
    vt.v = 1;

    cone.vpoints.push_back(vp);
    cone.vnormals.push_back(vn);
    cone.vtexs.push_back(vt);

    vp.values(0, 0, -height/2);
    vn.values(0, 0, -1);
    vt.u = 0.5; vt.v = 0.5;
    cone.vpoints.push_back(vp);
    cone.vnormals.push_back(vn);
    cone.vtexs.push_back(vt);
    for(int i = 0; i< faceNum; i++){
        float theta = i * 2*M_PI/faceNum;
        float x = r*cos(theta);
        float y = r*sin(theta);
        float xt = 0.5 + 0.5*cos(theta);
        float yt = 0.5 + 0.5*sin(theta);
        vp.values(x, y, -height/2);
        cone.vpoints.push_back(vp);
        vn.values(x, y, (x*x + y*y)/height);
        cone.vnormals.push_back(vn);
        vt.u = xt;
        vt.v = yt;
        cone.vtexs.push_back(vt);
    }
    for(int i = 0; i< faceNum; i++){
        if(i == faceNum - 1){
            f.values_v(1, i+3, 3);
            f.values_vn(1, i+3, 3);
            f.values_vt(1, i+3, 3);
            cone.faces.push_back(f);
            f.values_v(2, i+3, 3);
            f.values_vn(2, 2, 2);
            f.values_vt(2, 2, 2);
            cone.faces.push_back(f);
        }
        else{
            f.values_v(1, i+3, i+4);
            f.values_vn(1, i+3, i+4);
            f.values_vt(1, i+3, i+4);
            cone.faces.push_back(f);
            f.values_v(2, i+3, i+4);
            f.values_vn(2, 2, 2);
            f.values_vt(2, i+3, i+4);
            cone.faces.push_back(f);
        }
    }
    cone.entityAttr.push_back(r);
    cone.entityAttr.push_back(height);
    cone.entityAttr.push_back(faceNum);
    cone.max_X = cone.max_Y = r;
    cone.min_X = cone.min_Y = -r;
    cone.max_Z = height/2;
    cone.min_Z = -height/2;
    object obj;
    mtl Tmtl;
    obj.vn_num = cone.vnormals.size();
    obj.vt_num = cone.vtexs.size();
    obj.v_num = cone.vpoints.size();
    obj.size = cone.faces.size();
    obj.ambient[0] = obj.ambient[1] = obj.ambient[2] = Tmtl.ambient[0] = Tmtl.ambient[1] = Tmtl.ambient[2] = 0.2;
    obj.diffuse[0] = obj.diffuse[1] = obj.diffuse[2] = Tmtl.diffuse[0] = Tmtl.diffuse[1] = Tmtl.diffuse[2] = 0.8;
    obj.emission[0] = obj.emission[1] = obj.emission[2] = Tmtl.emission[0] = Tmtl.emission[1] = Tmtl.emission[2] = 0;
    obj.specular[0] = obj.specular[1] = obj.specular[2] = Tmtl.specular[0] = Tmtl.specular[1] = Tmtl.specular[2] = 0;
    obj.texID = Tmtl.texID = 0;

    cone.objects.push_back(obj);
    cone.mtls.push_back(Tmtl);
    cone.calObjCenter();
    cone.genDisplayList();
    return cone;
}
