#ifndef ENITY_H
#define ENITY_H

#include "OBJ.h"

model createCube(float l = 50);

//柱体和台体使用一个类表示。棱柱可以看做底面和顶面外接圆半径相同的棱台；圆柱和圆台可以看做侧面数特别多的棱柱和棱台
class Prismoid{
public:
    Prismoid(int fn = 3, float h = 50, float r = 50, float rt = 30);
    model createPrismoid();
private:
    int faceNum;
    float height;
    //底面外接圆半径
    float r;
    //顶面外接圆半径
    float rTop;
};

class Sphere{
public:
    Sphere(float r = 50, int dens = 50);
    model createSphere();
private:
    float r;
    int density;
};

class Cone{
public:
    Cone(float r = 30, float height = 50, int faceNum = 50);
    model createCone();
private:
    float r;
    float height;
    int faceNum;
};

#endif // ENITY_H
