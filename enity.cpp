#include "enity.h"
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
#include <QDebug>

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
