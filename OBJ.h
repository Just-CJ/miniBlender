#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <string>
#include <QDir>


class vpoint{
  public:
    float x, y, z;
};

class vnormal{
  public:
    float x, y, z;
};

class vtex{
  public:
    float u, v;
};

class face{
  public:
    int index_v[4];
    int index_vt[4];
    int index_vn[4];
};

class mtl{
  public:
    std::string mtlname;
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float emission[3];
    unsigned int texID;
};

class object : public mtl{
  public:
    unsigned int size;
};

class model{//基本对象
  public:
    std::vector<vpoint> vpoints;
    std::vector<vnormal> vnormals;
    std::vector<vtex> vtexs;
    std::vector<face> faces;
    std::vector<mtl> mtls;
    std::vector<object> objects;

    void calObjCenter();
    void genDisplayList();
    void drawOBJ();
    void callDisplayList();

    ~model();

  private:
    float objCenter[3];
    int DisplayListID;
};


void loadOBJ(std::string fileAddr);
void loadMTL(std::string objAddr, std::string mtlAddr);
void loadTex(unsigned int texID, QDir workdir, std::string texAddr);

void drawCoordinate();





#endif // OBJ_H
