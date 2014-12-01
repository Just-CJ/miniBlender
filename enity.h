#ifndef ENITY_H
#define ENITY_H

#include "OBJ.h"

//class enityToModel
//{
//public:
    //On create
    model createCube();
    model createSphere();
    model createCylinder();
    model createCone();
    model createPrism();
    model createFPyramid();

    model createSphere(int densaty);
    model createCylinder(int densaty);
    model createCone(int densaty);
    model createPrism(int faceNum);
    model createFPyramid(int faceNum);
    //When update some attributes, you need to reshape them.
    //That's the density of the wire or the side face. (Cube doesn't need update)
//};

#endif // ENITY_H
