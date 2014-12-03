#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <string>
#include <QDir>

class vpoint{
  public:
    void values(float xi, float yi, float zi){
        x = xi;
        y = yi;
        z = zi;
    }
    float x, y, z;
};

class vnormal{
  public:
    void values(float xi, float yi, float zi){
        x = xi;
        y = yi;
        z = zi;
    }

    float x, y, z;
};

class vtex{
  public:
    float u, v;
};

class face{
  public:
    void values_v(int v1, int v2, int v3, int v4 = 0){
        index_v[0] = v1;
        index_v[1] = v2;
        index_v[2] = v3;
        index_v[3] = v4;
    }
    void values_vt(int v1, int v2, int v3, int v4 = 0){
        index_vt[0] = v1;
        index_vt[1] = v2;
        index_vt[2] = v3;
        index_vt[3] = v4;
    }
    void values_vn(int v1, int v2, int v3, int v4 = 0){
        index_vn[0] = v1;
        index_vn[1] = v2;
        index_vn[2] = v3;
        index_vn[3] = v4;
    }

    int index_v[4];
    int index_vt[4];
    int index_vn[4];
};

class mtl{
  public:
    std::string mtlname;
    QString map_Kd_addr;
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float emission[3];
    unsigned int texID;
};

class object : public mtl{
  public:
    unsigned int size;
    unsigned int v_num;
    unsigned int vn_num;
    unsigned int vt_num;
};

//用于检查model类型是enity还是object。对于enity可以进行重定义
enum entityType{
    NOT,
    CUBE,
    PRISMOID,
    CYLINDER,
    SPHERE
};


class model{//基本对象
  public:
    bool isSelected;
    float offset_x, offset_y, offset_z;
    float scale;
    double rotate_x, rotate_y, rotate_z;    //实际上没用
    float rotateMatrix[16];
    float min_X, max_X, min_Y, max_Y, min_Z, max_Z;
    float objCenter[3];

    std::vector<vpoint> vpoints;
    std::vector<vnormal> vnormals;
    std::vector<vtex> vtexs;
    std::vector<face> faces;
    std::vector<mtl> mtls;
    std::vector<object> objects;

    void calObjCenter();
    void genDisplayList();
    void deleteDisplayList();
    void drawOBJ();
    void callDisplayList();
    model();
    ~model();

    //Added attribute to control reshape entity. ---Lkx
    entityType type;
    std::vector<float> entityAttr;

  private:

    int DisplayListID;
};


void loadOBJ(std::string fileAddr);
void loadMTL(std::string objAddr, std::string mtlAddr);
void loadTex(unsigned int texID, QDir workdir, std::string texAddr);

void drawCoordinate();

void exportOBJ(QString fileName);
void exportMTL(QString fileName);


#endif // OBJ_H
