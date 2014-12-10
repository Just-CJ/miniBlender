#include "OBJ.h"
#include <fstream>
#include <sstream>
#include <QString>
#include <QDebug>
#include <QImage>
#include <QFileDialog>

#include <GL/glew.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
//#include <QtOpenGL/QtOpenGL>

//#include <GL/freeglut.h>



#define BITMAP_ID 0x4d42

using namespace std;

vector<model> models;

model::model(){
  isSelected = false;

  min_X = max_X = min_Y = max_Y = min_Z = max_Z = 0.0;

  offset_x = 0.0;
  offset_y = 0.0;
  offset_z = 0.0;

  scale_x = 1.0;
  scale_y = 1.0;
  scale_z = 1.0;

  rotateMatrix[0]=1;rotateMatrix[1]=0;rotateMatrix[2]=0;rotateMatrix[3]=0;
  rotateMatrix[4]=0;rotateMatrix[5]=1;rotateMatrix[6]=0;rotateMatrix[7]=0;
  rotateMatrix[8]=0;rotateMatrix[9]=0;rotateMatrix[10]=1;rotateMatrix[11]=0;
  rotateMatrix[12]=0;rotateMatrix[13]=0;rotateMatrix[14]=0;rotateMatrix[15]=1;

  scale = 1.0;
  objCenter[0] = 0.0;
  objCenter[1] = 0.0;
  objCenter[2] = 0.0;
  DisplayListID = 0;

  type = NOT;
}

model::~model(){
  this->vpoints.clear();
  this->vnormals.clear();
  this->vtexs.clear();
  this->faces.clear();
  //for(unsigned int i=0; i<this->mtls.size(); i++)//删除绑定纹理
      //glDeleteTextures(1, &(this->mtls[i].texID));

  this->mtls.clear();
  this->objects.clear();
  //if(DisplayListID) glDeleteLists(DisplayListID, 1);//删除显示列表
}



void loadOBJ(string fileAddr){
  //models.clear();

  model tmpmodel;
  models.push_back(tmpmodel);

  bool objStart = false;
  unsigned objSize = 0;
  unsigned v_num = 0;
  unsigned vn_num = 0;
  unsigned vt_num = 0;
  string mtlname;

  ifstream infile(fileAddr.c_str()); //打开obj文件
  string strline;
  string key, subline;
  while(getline(infile, strline)){
      if(strline.size()==0||strline[0]=='#') continue;

      istringstream is(strline);
      is>>key; //读取每行第一个单词

      if(key == "v"){  //顶点坐标
          vpoint v;
          v_num++;
          is>>v.x>>v.y>>v.z;
          if(v.x<models.back().min_X) models.back().min_X = v.x;
          if(v.x>models.back().max_X) models.back().max_X = v.x;
          if(v.y<models.back().min_Y) models.back().min_Y = v.y;
          if(v.y>models.back().max_Y) models.back().max_Y = v.y;
          if(v.z<models.back().min_Z) models.back().min_Z = v.z;
          if(v.z>models.back().max_Z) models.back().max_Z = v.z;
          models.back().vpoints.push_back(v);
        }
      else if(key == "vn"){ //法向量坐标
          vnormal vn;
          vn_num++;
          is>>vn.x>>vn.y>>vn.z;
          models.back().vnormals.push_back(vn);
        }
      else if(key == "vt"){  //纹理坐标
          vtex vt;
          is>>vt.u>>vt.v;
          vt_num++;
          models.back().vtexs.push_back(vt);
        }
      else if(key == "f"){//面
          objSize++;
          face f;
          for(int i=0; i<4; i++){
              if(is.tellg()>0){
                  is>>subline;
                  int c = count(subline.begin(), subline.end(), '/');
                  if(c == 0){ //没有'/' 只包含顶点坐标索引
                      f.index_v[i] = atoi(subline.c_str());
                      f.index_vt[i] = 0;
                      f.index_vn[i] = 0;
                    }
                  else if(c == 1){//只有一个'/' 包含顶点坐标索引和纹理坐标索引
                      f.index_v[i] = atoi(string(subline.begin(), subline.begin()+subline.find("/")).c_str());
                      f.index_vt[i] = atoi(string(subline.begin()+subline.find("/")+1, subline.end()).c_str());
                      f.index_vn[i] = 0;
                    }
                  else if(c == 2){ //有两个'/' 必然包含顶点坐标索引和法向量坐标索引
                      int first = subline.find("/");
                      int second = subline.find("/", first+1);
                      f.index_v[i] = atoi(string(subline.begin(), subline.begin()+first).c_str());

                      if((first+1)==second) f.index_vt[i] = -1; //判断是否有纹理坐标索引
                      else f.index_vt[i] = atoi(string(subline.begin()+first+1, subline.begin()+second).c_str());

                      f.index_vn[i] = atoi(string(subline.begin()+second+1, subline.end()).c_str());
                    }
                }
              else{ //没有第四个点
                  f.index_v[i] = 0;
                  f.index_vn[i] = 0;
                  f.index_vt[i] = 0;
                }
            }
          models.back().faces.push_back(f);
        }
      else if(key == "mtllib"){
            string mtlAddr;
            int offset = strline.find_first_not_of(" ", 7);
            if(offset){
              mtlAddr = string(strline.begin()+offset, strline.end());
              loadMTL(fileAddr, mtlAddr);
              }
        }
      else if(key == "usemtl"){
          if(objStart){//前一个object读取完毕
              for(unsigned int i = 0; i<models.back().mtls.size(); i++){//去材质库里匹配
                  if(models.back().mtls[i].mtlname == mtlname){
                      object obj((object&)models.back().mtls[i]); //从mtl类创建一个object类
                      obj.size = objSize;
                      obj.v_num = v_num;
                      obj.vn_num = vn_num;
                      obj.vt_num = vt_num;
                      models.back().objects.push_back(obj);
                      break;
                    }
                }
              is>>mtlname;
              objSize = 0;
              v_num = 0;
              vn_num = 0;
              vt_num = 0;
            }
          else{//第一个object
              objStart = true;
              is>>mtlname;
              objSize = 0;
              //v_num = 0;
              //vn_num = 0;
              //vt_num = 0;
            }
        }
    }
  if(objStart){//最后一个object读取完毕
      for(unsigned int i = 0; i<models.back().mtls.size(); i++){//去材质库里匹配
          if(models.back().mtls[i].mtlname == mtlname){
              object obj((object&)models.back().mtls[i]); //从mtl类创建一个object类
              obj.size = objSize;
              obj.v_num = v_num;
              obj.vn_num = vn_num;
              obj.vt_num = vt_num;
              models.back().objects.push_back(obj);
              break;
            }
        }
    }

  models.back().calObjCenter();
  models.back().genDisplayList();

  infile.close();
}

void loadMTL(string objAddr, string mtlAddr){
  QDir workdir(QString::fromStdString(objAddr));
  workdir.cdUp();
  QString mtlAbAddr = workdir.filePath(QString::fromStdString(mtlAddr));

  bool start = false;

  string mtlname;
  QString map_Kd_addr = "";
  float ambient[3] = {0.0,0.0,0.0};
  float diffuse[3] = {0.0,0.0,0.0};
  float specular[3] = {0.0,0.0,0.0};
  float emission[3] = {0.0,0.0,0.0};
  unsigned int texID = 0;

  ifstream infile(mtlAbAddr.toStdString().c_str()); //打开mtl文件
  string strline;
  string key;
  string texAddr;

  while(getline(infile, strline)){
      if(strline.size()==0||strline[0]=='#') continue;
      istringstream is(strline);
      is>>key;//读取每行第一个单词

      if(key == "newmtl"){
          if(start){//前一个mtl读取完毕
              mtl newmtl;
              newmtl.mtlname = mtlname;
              newmtl.ambient[0] = ambient[0]; newmtl.ambient[1] = ambient[1]; newmtl.ambient[2] = ambient[2];
              newmtl.diffuse[0] = diffuse[0]; newmtl.diffuse[1] = diffuse[1]; newmtl.diffuse[2] = diffuse[2];
              newmtl.specular[0] = specular[0]; newmtl.specular[1] = specular[1]; newmtl.specular[2] = specular[2];
              newmtl.emission[0] = emission[0]; newmtl.emission[1] = emission[1]; newmtl.emission[2] = emission[2];
              newmtl.texID = texID;
              newmtl.map_Kd_addr = map_Kd_addr;

              models.back().mtls.push_back(newmtl);
              is>>mtlname;
              ambient[0] = 0.0; ambient[1] = 0.0; ambient[2] = 0.0; //重新进行初始化
              diffuse[0] = 0.0; diffuse[1] = 0.0; diffuse[2] = 0.0;
              specular[0] = 0.0; specular[1] = 0.0; specular[2] = 0.0;
              emission[0] = 0.0; emission[1] = 0.0; emission[2] = 0.0;
              texID = 0;
              map_Kd_addr = "";
            }
          else{//第一个mtl
              start = true;
              is>>mtlname;
            }
        }
      else if(key == "Ka"){
          is>>ambient[0]>>ambient[1]>>ambient[2];
        }
      else if(key == "Kd"){
          is>>diffuse[0]>>diffuse[1]>>diffuse[2];
        }
      else if(key == "Ks"){
          is>>specular[0]>>specular[1]>>specular[2];
        }
      else if(key == "Ke"){
          is>>emission[0]>>emission[1]>>emission[2];
        }
      else if(key == "map_Kd"){
            int offset = strline.find_first_not_of(" ", 7);
            if(offset){
              texAddr = string(strline.begin()+offset, strline.end());
              glGenTextures(1, &texID);
              loadTex(texID, workdir, texAddr);
              map_Kd_addr = workdir.filePath(QString::fromStdString(texAddr));
              }
        }
    }
  if(start){//最后一个mtl读取完毕
      mtl newmtl;
      newmtl.mtlname = mtlname;
      newmtl.ambient[0] = ambient[0]; newmtl.ambient[1] = ambient[1]; newmtl.ambient[2] = ambient[2];
      newmtl.diffuse[0] = diffuse[0]; newmtl.diffuse[1] = diffuse[1]; newmtl.diffuse[2] = diffuse[2];
      newmtl.specular[0] = specular[0]; newmtl.specular[1] = specular[1]; newmtl.specular[2] = specular[2];
      newmtl.emission[0] = emission[0]; newmtl.emission[1] = emission[1]; newmtl.emission[2] = emission[2];
      newmtl.texID = texID;
      newmtl.map_Kd_addr = map_Kd_addr;

      models.back().mtls.push_back(newmtl);
    }
  infile.close();
}

void loadTex(unsigned int texID, QDir workdir, string texAddr){
  QString texAbAddr = workdir.filePath(QString::fromStdString(texAddr));

          QImage *image = new QImage(texAbAddr);

          unsigned char *imagedata;
          int colorMode;
          if(image->depth()==8) colorMode = GL_LUMINANCE;
          else if(image->depth()==32) colorMode = GL_RGBA;
          else return;
          imagedata = new unsigned char[image->byteCount()];
          int index = 0;
          //RGBA模式
          if(image->depth()==32){
            for(int i=image->height()-1; i>=0; i--){//从下往上扫描
                for(int j=0; j<image->width(); j++){//将BGR调整为RGB
                    imagedata[index] = image->bits()[4*(i*image->width()+j)+2];
                    imagedata[index+1] = image->bits()[4*(i*image->width()+j)+1];
                    imagedata[index+2] = image->bits()[4*(i*image->width()+j)+0];
                    imagedata[index+3] = image->bits()[4*(i*image->width()+j)+3];
                    index+=4;
                  }
              }
            }
          else if(image->depth()==8){
            for(int i=image->height()-1; i>=0; i--){//从下往上扫描
                for(int j=0; j<image->width(); j++){
                    imagedata[index] = image->bits()[(i*image->width()+j)];
                    index++;
                  }
              }
            }

          glBindTexture(GL_TEXTURE_2D, texID);

           //指定当前纹理的放大/缩小过滤方式
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

          glTexImage2D(GL_TEXTURE_2D,
                  0, 	    //mipmap层次(通常为，表示最上层)
                  colorMode,	//我们希望该纹理有红、绿、蓝数据
                  image->width(), //纹理宽带，必须是n，若有边框+2
                  image->height(), //纹理高度，必须是n，若有边框+2
                  0, //边框(0=无边框, 1=有边框)
                  colorMode,	//bitmap数据的格式
                  GL_UNSIGNED_BYTE, //每个颜色数据的类型
                  imagedata);	//bitmap数据指针

          delete image;
          delete[] imagedata;
}

void model::calObjCenter(){
  objCenter[0] = 0.0f; objCenter[1] = 0.0f; objCenter[2] = 0.0f;
  //for(unsigned int i=0; i<vpoints.size(); i++){
  //    objCenter[0] += vpoints[i].x;
  //    objCenter[1] += vpoints[i].y;
  //    objCenter[2] += vpoints[i].z;
  //  }
  objCenter[0] = (min_X+max_X)/2;
  objCenter[1] = (min_Y+max_Y)/2;
  objCenter[2] = (min_Z+max_Z)/2;

  //objCenter[0]/=vpoints.size();
  //objCenter[1]/=vpoints.size();
  //objCenter[2]/=vpoints.size();
}

void drawCoordinate()
{
    /*红色轴是X轴，绿色是Y轴，蓝色是Z轴*/
    glPushAttrib(GL_ALL_ATTRIB_BITS); //保存当前所有属性

    glLineWidth(3);

    float mat_diffuse_x[] = { 0.8f, 0.0f, 0.0f, 1.0f };
    float mat_ambient_x[] = { 0.2f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_x);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_x);
    glBegin(GL_LINES);
    glColor3f(1.0f,0.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.5,0.0,0.0);
    glEnd();
    glPushMatrix();
    glTranslatef(0.5, 0.0f, 0.0f);
    glRotatef(90.0f,0.0f,1.0f,0.0f);
    glutSolidCone(0.027,0.09,10,10);

    glPopMatrix();

    float mat_diffuse_y[] = { 0.0f, 0.8f, 0.0f, 1.0f };
    float mat_ambient_y[] = { 0.0f, 0.2f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_y);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_y);
    glBegin(GL_LINES);
    glColor3f(0.0,1.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.5,0.0);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0, 0.5f, 0.0f);
    glRotatef(-90.0f,1.0f,0.0f,0.0f);
    glutSolidCone(0.027,0.09,10,10);
    glPopMatrix();

    float mat_diffuse_z[] = { 0.0f, 0.0f, 0.8f, 1.0f };
    float mat_ambient_z[] = { 0.0f, 0.0f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_z);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_z);
    glBegin(GL_LINES);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,0.5);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0, 0.0f, 0.5f);
    glutSolidCone(0.027,0.09,10,10);
    glPopMatrix();
    glPopAttrib();
}

void model::drawOBJ(){
  glPushMatrix();
    //glRotatef(-objRotate_x, 1, 0, 0);  //自定义旋转
    glTranslatef(-objCenter[0], -objCenter[1], -objCenter[2]); //把obj的中心移到坐标中心

    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS); //保存当前所有属性
    if(objects.size()){

      unsigned int index=0;
      for(unsigned int i=0; i<objects.size(); i++){
          glMaterialfv(GL_FRONT, GL_DIFFUSE, objects[i].diffuse);
          glMaterialfv(GL_FRONT, GL_SPECULAR, objects[i].specular);
          glMaterialfv(GL_FRONT, GL_AMBIENT, objects[i].ambient);
          glMaterialfv(GL_FRONT, GL_EMISSION, objects[i].emission);

          if(objects[i].texID){ //是否有纹理
              glEnable(GL_TEXTURE_2D);
              glBindTexture(GL_TEXTURE_2D, objects[i].texID);
            }

          for(unsigned int j=0; j<objects[i].size; j++){

              glBegin(GL_POLYGON);
              glColor3f(1,1,1);

                if(faces[index].index_vn[0])//若法向量索引存在
                  glNormal3f(vnormals[faces[index].index_vn[0]-1].x, vnormals[faces[index].index_vn[0]-1].y, vnormals[faces[index].index_vn[0]-1].z);
                if(objects[i].texID)
                  glTexCoord2f(vtexs[faces[index].index_vt[0]-1].u, vtexs[faces[index].index_vt[0]-1].v);
                glVertex3f(vpoints[faces[index].index_v[0]-1].x, vpoints[faces[index].index_v[0]-1].y, vpoints[faces[index].index_v[0]-1].z);

                if(faces[index].index_vn[1])//若法向量索引存在
                  glNormal3f(vnormals[faces[index].index_vn[1]-1].x, vnormals[faces[index].index_vn[1]-1].y, vnormals[faces[index].index_vn[1]-1].z);
                if(objects[i].texID)
                  glTexCoord2f(vtexs[faces[index].index_vt[1]-1].u, vtexs[faces[index].index_vt[1]-1].v);
                glVertex3f(vpoints[faces[index].index_v[1]-1].x, vpoints[faces[index].index_v[1]-1].y, vpoints[faces[index].index_v[1]-1].z);

                if(faces[index].index_vn[2])//若法向量索引存在
                  glNormal3f(vnormals[faces[index].index_vn[2]-1].x, vnormals[faces[index].index_vn[2]-1].y, vnormals[faces[index].index_vn[2]-1].z);
                if(objects[i].texID)
                  glTexCoord2f(vtexs[faces[index].index_vt[2]-1].u, vtexs[faces[index].index_vt[2]-1].v);
                glVertex3f(vpoints[faces[index].index_v[2]-1].x, vpoints[faces[index].index_v[2]-1].y, vpoints[faces[index].index_v[2]-1].z);

                if(faces[index].index_v[3]){//若第四个点存在
                  if(faces[index].index_vn[3])//若法向量索引存在
                    glNormal3f(vnormals[faces[index].index_vn[3]-1].x, vnormals[faces[index].index_vn[3]-1].y, vnormals[faces[index].index_vn[3]-1].z);
                  if(objects[i].texID)
                    glTexCoord2f(vtexs[faces[index].index_vt[3]-1].u, vtexs[faces[index].index_vt[3]-1].v);
                  glVertex3f(vpoints[faces[index].index_v[3]-1].x, vpoints[faces[index].index_v[3]-1].y, vpoints[faces[index].index_v[3]-1].z);
                  }

              glEnd();

              index++;
            }

          if(objects[i].texID){ //是否有纹理
              glDisable(GL_TEXTURE_2D);//关闭纹理
            }
        }

      }else{

      for(unsigned int i=0; i<faces.size(); i++){
          glBegin(GL_POLYGON);
          glColor3f(1,1,1);

          if(faces[i].index_vn[0])//若法向量索引存在
            glNormal3f(vnormals[faces[i].index_vn[0]-1].x, vnormals[faces[i].index_vn[0]-1].y, vnormals[faces[i].index_vn[0]-1].z);
          glVertex3f(vpoints[faces[i].index_v[0]-1].x, vpoints[faces[i].index_v[0]-1].y, vpoints[faces[i].index_v[0]-1].z);

          if(faces[i].index_vn[1])//若法向量索引存在
            glNormal3f(vnormals[faces[i].index_vn[1]-1].x, vnormals[faces[i].index_vn[1]-1].y, vnormals[faces[i].index_vn[1]-1].z);
          glVertex3f(vpoints[faces[i].index_v[1]-1].x, vpoints[faces[i].index_v[1]-1].y, vpoints[faces[i].index_v[1]-1].z);

          if(faces[i].index_vn[2])//若法向量索引存在
            glNormal3f(vnormals[faces[i].index_vn[2]-1].x, vnormals[faces[i].index_vn[2]-1].y, vnormals[faces[i].index_vn[2]-1].z);
          glVertex3f(vpoints[faces[i].index_v[2]-1].x, vpoints[faces[i].index_v[2]-1].y, vpoints[faces[i].index_v[2]-1].z);

          if(faces[i].index_v[3]){//若第四个点存在
            if(faces[i].index_vn[3])//若法向量索引存在
              glNormal3f(vnormals[faces[i].index_vn[3]-1].x, vnormals[faces[i].index_vn[3]-1].y, vnormals[faces[i].index_vn[3]-1].z);
            glVertex3f(vpoints[faces[i].index_v[3]-1].x, vpoints[faces[i].index_v[3]-1].y, vpoints[faces[i].index_v[3]-1].z);
            }
          glEnd();
        }
      }
    glPopAttrib();
    glPopMatrix();

    //glTranslatef(objCenter[0], objCenter[1], objCenter[2]);
    //glScalef(100,100,100);
    //drawCoordinate(); //obj的中心坐标系
  glPopMatrix();
}

void model::genDisplayList(){
  GLint lid = glGenLists(1);
  glNewList(lid, GL_COMPILE);
  drawOBJ();

  glEndList();

  DisplayListID = lid;
}

void model::deleteDisplayList(){
  glDeleteLists(DisplayListID, 1);
}


void model::callDisplayList(){
  glCallList(DisplayListID);
}

void exportOBJ(QString fileName){
  QFile file(fileName);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      return;

  QTextStream out(&file);
  unsigned int pre_v_num = 0, pre_vt_num = 0, pre_vn_num = 0;
  if(models.size()){
      fileName.replace(QRegExp("(\\.obj)$"), ".mtl");
      exportMTL(fileName);//输出mtl文件
      out<<"mtllib "<<QDir(fileName).dirName()<<endl;
    }

  for(unsigned int i=0; i<models.size(); i++){//遍历输出model
      if(models[i].objects.size()>0){

          unsigned int index_v=0, index_vn=0, index_vt=0, index_f=0;
          for(unsigned int j=0; j<models[i].objects.size(); j++){//遍历输出object

              out<<"# object"<<" MODEL_"<<i<<"_OBJECT_"<<j<<endl;
              for(unsigned int k=0; k<models[i].objects[j].v_num; k++){//遍历输出v
                GLfloat x, y, z, x1, y1, z1;
                x = models[i].vpoints[index_v].x -models[i].objCenter[0];
                y = models[i].vpoints[index_v].y -models[i].objCenter[1];
                z = models[i].vpoints[index_v].z -models[i].objCenter[2];
                float *m = models[i].rotateMatrix;
                x1 = m[0]*x + m[1]*y + m[2]*z + m[3];
                y1 = m[4]*x + m[5]*y + m[6]*z + m[7];
                z1 = m[8]*x + m[9]*y + m[10]*z + m[11];
                x1 = models[i].scale*x1 + models[i].offset_x;
                y1 = models[i].scale*y1 + models[i].offset_y;
                z1 = models[i].scale*z1 + models[i].offset_z;

                //x = models[i].scale*(models[i].vpoints[index_v].x -models[i].objCenter[0])+models[i].offset_x;
                //y = models[i].scale*(models[i].vpoints[index_v].y -models[i].objCenter[1])+models[i].offset_y;
                //z = models[i].scale*(models[i].vpoints[index_v].z -models[i].objCenter[2])+models[i].offset_z;

                //out<<"v "<<models[i].vpoints[index_v].x<<" "<<models[i].vpoints[index_v].y<<" "<<models[i].vpoints[index_v].z<<endl;
                out<<"v "<<x1<<" "<<y1<<" "<<z1<<endl;
                index_v++;
                }

              out<<endl;

              for(unsigned int k=0; k<models[i].objects[j].vn_num; k++){//遍历输出vn
                GLfloat x, y, z, x1, y1, z1;
                x = models[i].vnormals[index_vn].x;
                y = models[i].vnormals[index_vn].y;
                z = models[i].vnormals[index_vn].z;
                float *m = models[i].rotateMatrix;
                x1 = m[0]*x + m[1]*y + m[2]*z + m[3];
                y1 = m[4]*x + m[5]*y + m[6]*z + m[7];
                z1 = m[8]*x + m[9]*y + m[10]*z + m[11];
                out<<"vn "<<x1<<" "<<y1<<" "<<z1<<endl;
                index_vn++;
                }

              out<<endl;

              for(unsigned int k=0; k<models[i].objects[j].vt_num; k++){//遍历输出vt
                out<<"vt "<<models[i].vtexs[index_vt].u<<" "<<models[i].vtexs[index_vt].v<<" "<<0<<endl;
                index_vt++;
                }

              out<<endl;
              out<<"usemtl "<<" MODEL_"<<i<<"_"+QString::fromStdString(models[i].objects[j].mtlname)<<endl;

              for(unsigned int k=0; k<models[i].objects[j].size; k++){//遍历输出f
                out<<"f ";
                for(unsigned int m=0; m<3; m++){
                  out<<models[i].faces[index_f].index_v[m]+pre_v_num;//顶点索引
                  if(models[i].faces[index_f].index_vt[m]){
                    out<<"/"<<models[i].faces[index_f].index_vt[m]+pre_vt_num;
                    if(models[i].faces[index_f].index_vn[m])
                      out<<"/"<<models[i].faces[index_f].index_vn[m]+pre_vn_num;
                    if(m<2) out<<" ";
                    }
                  else{
                      if(models[i].faces[index_f].index_vn[m])
                        out<<"//"<<models[i].faces[index_f].index_vn[m]+pre_vn_num;
                      if(m<2) out<<" ";
                      }
                  }
                if(models[i].faces[index_f].index_v[3]){
                    out<<" ";
                    out<<models[i].faces[index_f].index_v[3]+pre_v_num;
                    if(models[i].faces[index_f].index_vt[3]){
                      out<<"/"<<models[i].faces[index_f].index_vt[3]+pre_vt_num;
                      if(models[i].faces[index_f].index_vn[3])
                        out<<"/"<<models[i].faces[index_f].index_vn[3]+pre_vn_num;
                    }
                    else{
                        if(models[i].faces[index_f].index_vn[3])
                          out<<"//"<<models[i].faces[index_f].index_vn[3]+pre_vn_num;
                        }
                  }
                out<<endl;
                index_f++;
                }

              out<<endl;
            }

          pre_v_num+=models[i].vpoints.size();
          pre_vt_num+=models[i].vtexs.size();
          pre_vn_num+=models[i].vnormals.size();
        }
      else{//没有标记usemtl的文件
          out<<"# object"<<" MODEL_"<<i<<endl;
          for(unsigned int k=0; k<models[i].vpoints.size(); k++){//遍历输出v
            GLfloat x, y, z, x1, y1, z1;
            x = models[i].vpoints[k].x -models[i].objCenter[0];
            y = models[i].vpoints[k].y -models[i].objCenter[1];
            z = models[i].vpoints[k].z -models[i].objCenter[2];

            float *m = models[i].rotateMatrix;
            x1 = m[0]*x + m[1]*y + m[2]*z + m[3];
            y1 = m[4]*x + m[5]*y + m[6]*z + m[7];
            z1 = m[8]*x + m[9]*y + m[10]*z + m[11];
            x1 = models[i].scale*x1 + models[i].offset_x;
            y1 = models[i].scale*y1 + models[i].offset_y;
            z1 = models[i].scale*z1 + models[i].offset_z;
            out<<"v "<<x1<<" "<<y1<<" "<<z1<<endl;

            //out<<"v "<<models[i].vpoints[k].x<<" "<<models[i].vpoints[k].y<<" "<<models[i].vpoints[k].z<<endl;
            }
          out<<endl;
          for(unsigned int k=0; k<models[i].vnormals.size(); k++){//遍历输出vn
            GLfloat x, y, z, x1, y1, z1;
            x = models[i].vnormals[k].x;
            y = models[i].vnormals[k].y;
            z = models[i].vnormals[k].z;
            float *m = models[i].rotateMatrix;
            x1 = m[0]*x + m[1]*y + m[2]*z + m[3];
            y1 = m[4]*x + m[5]*y + m[6]*z + m[7];
            z1 = m[8]*x + m[9]*y + m[10]*z + m[11];
            out<<"vn "<<x1<<" "<<y1<<" "<<z1<<endl;
            }
          out<<endl;
          for(unsigned int k=0; k<models[i].vtexs.size(); k++){//遍历输出vt
            out<<"vt "<<models[i].vtexs[k].u<<" "<<models[i].vtexs[k].v<<" "<<0<<endl;
            }
          out<<endl;
          for(unsigned int k=0; k<models[i].faces.size(); k++){//遍历输出f
            out<<"f ";
            for(unsigned int m=0; m<3; m++){
              out<<models[i].faces[k].index_v[m];
              if(models[i].faces[k].index_vt[m]){
                out<<"/"<<models[i].faces[k].index_vt[m];
                if(models[i].faces[k].index_vn[m])
                  out<<"/"<<models[i].faces[k].index_vn[m];
                if(m<2) out<<" ";
                }
              else{
                  if(models[i].faces[k].index_vn[m])
                    out<<"//"<<models[i].faces[k].index_vn[m];
                  if(m<2) out<<" ";
                  }
              }
            if(models[i].faces[k].index_v[3]){
                out<<" ";
                out<<models[i].faces[k].index_v[3];
                if(models[i].faces[k].index_vt[3]){
                  out<<"/"<<models[i].faces[k].index_vt[3];
                  if(models[i].faces[k].index_vn[3])
                    out<<"/"<<models[i].faces[k].index_vn[3];
                }
                else{
                    if(models[i].faces[k].index_vn[3])
                      out<<"//"<<models[i].faces[k].index_vn[3];
                    }
              }
            out<<endl;
            }

          out<<endl;

          pre_v_num+=models[i].vpoints.size();
          pre_vt_num+=models[i].vtexs.size();
          pre_vn_num+=models[i].vnormals.size();
        }
    }

  file.close();
}

void exportMTL(QString fileName){
  QFile file(fileName);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      return;

  QDir workdir(fileName);
  workdir.cdUp();
  fileName.replace(QRegExp("(\\.mtl)$"), "");

  QTextStream out(&file);
  for(unsigned int i = 0; i<models.size(); i++){//遍历所有model
      for(unsigned int j=0; j<models[i].mtls.size(); j++){//遍历所有model的mtl
          out<<"newmtl "<<" MODEL_"<<i<<"_"+QString::fromStdString(models[i].mtls[j].mtlname)<<endl;
          out<<"Ka "<<models[i].mtls[j].ambient[0]<<" "<<models[i].mtls[j].ambient[1]<<" "<<models[i].mtls[j].ambient[2]<<endl;
          out<<"Kd "<<models[i].mtls[j].diffuse[0]<<" "<<models[i].mtls[j].diffuse[1]<<" "<<models[i].mtls[j].diffuse[2]<<endl;
          out<<"Ks "<<models[i].mtls[j].specular[0]<<" "<<models[i].mtls[j].specular[1]<<" "<<models[i].mtls[j].specular[2]<<endl;
          out<<"Ke "<<models[i].mtls[j].emission[0]<<" "<<models[i].mtls[j].emission[1]<<" "<<models[i].mtls[j].emission[2]<<endl;
          if(models[i].mtls[j].map_Kd_addr != ""){
              QDir tex(models[i].mtls[j].map_Kd_addr);
              QString texDir = QDir(fileName).dirName()+"_tex";
              workdir.setPath(workdir.absolutePath()+"/"+texDir);
              if(!workdir.exists()){
                  workdir.cdUp();
                  workdir.mkdir(texDir);
                  workdir.cd(texDir);
                }
              qDebug()<<workdir.absolutePath()+"/"+tex.dirName();
              qDebug()<<QFile::copy(models[i].mtls[j].map_Kd_addr, workdir.path()+"/"+tex.dirName());
              workdir.cdUp();
              out<<"map_Kd "<<texDir<<"\\"<<tex.dirName()<<endl;
            }
          out<<endl;
        }
    }

  file.close();
}
