#ifndef ENITY_H
#define ENITY_H

#include "OBJ.h"

model createCube(float r = 50);

//棱柱和棱台使用一个类表示。棱柱可以看做底面和顶面外接圆半径相同的棱台。
class Prismoid{
public:
    Prismoid(int fn = 3, float h = 50, float r = 50, float rt = 30);
    model createPrismoid();
    int faceNum;
    float height;
    //底面外接圆半径
    float r;
    //顶面外接圆半径
    float rTop;
};

#endif // ENITY_H
