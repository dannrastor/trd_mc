#include "WeightingPotentialMap.hh"

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <float.h>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <cstring>

using namespace std;

void parse_npy_header(FILE* fp, unsigned int& word_size, unsigned int*& shape, unsigned int& ndims, unsigned long& size, bool& fortran_order) {
    char buffer[256];
    size_t res = fread(buffer,sizeof(char),11,fp);
    if(res != 11)
        throw std::runtime_error("parse_npy_header: failed fread");
    std::string header = fgets(buffer,256,fp);
    assert(header[header.size()-1] == '\n');
    //   cout << "* " << header << endl;
    int loc1, loc2;

    //fortran order
    loc1 = header.find("fortran_order")+16;
    fortran_order = (header.substr(loc1,5) == "True" ? true : false);

    //shape
    loc1 = header.find("(");
    loc2 = header.find(")");
    std::string str_shape = header.substr(loc1+1,loc2-loc1-1);
    if(str_shape[str_shape.size()-1] == ',') ndims = 1;
    else ndims = std::count(str_shape.begin(),str_shape.end(),',')+1;
    shape = new unsigned int[ndims];
    for(unsigned int i = 0;i < ndims;i++) {
        loc1 = str_shape.find(",");
        shape[i] = atoi(str_shape.substr(0,loc1).c_str());
        str_shape = str_shape.substr(loc1+1);
    }

    //endian, word size, data type
    //byte order code | stands for not applicable.
    //not sure when this applies except for byte array
    loc1 = header.find("descr")+9;
    bool littleEndian = (header[loc1] == '<' || header[loc1] == '|' ? true : false);
    assert(littleEndian);

    //char type = header[loc1+1];
    //assert(type == map_type(T));

    std::string str_ws = header.substr(loc1+2);
    loc2 = str_ws.find("'");
    word_size = atoi(str_ws.substr(0,loc2).c_str());

    size = 1;
    for(unsigned int i = 0; i < ndims;i++) size *= shape[i];
}


void read_array(FILE* fp, void* data, unsigned long size, unsigned int word_size)
{
  size_t nread = fread(data,word_size,size,fp);
  if(nread != size)
    throw std::runtime_error("load_the_npy_file: failed fread");
}

char big_endian_test() {
    unsigned char x[] = {1,0};
    short y = *(short*) x;
    return y == 1 ? '<' : '>';
}

//===========================================


WeightingPotentialMap::WeightingPotentialMap()
{
  X=0;
  Y=0;
  Z=0;
  v=0;
}

WeightingPotentialMap::~WeightingPotentialMap()
{
  if(X) delete X;
  if(Y) delete Y;
  if(Z) delete Z;
  if(v) delete v;

}

int WeightingPotentialMap::ReadFile(std::string fname)
{
  unsigned int* shape;
  unsigned int ndims, word_size;
  unsigned long size;
  bool fortran_order;

  FILE* fp =  fopen(fname.c_str(),"r");

  // Read X
  parse_npy_header(fp, word_size,shape,ndims,size,fortran_order);
  X = new float[size];
  read_array(fp, X, size, word_size);
  npx = size;

  // read Y
  parse_npy_header(fp,word_size,shape,ndims,size,fortran_order);
  Y = new float[size];
  read_array(fp, Y, size, word_size);
  npy = size;

  // read Z
  parse_npy_header(fp,word_size,shape,ndims,size,fortran_order);
  Z = new float[size];
  read_array(fp, Z, size, word_size);
  npz = size;

  // read WP
  parse_npy_header(fp,word_size,shape,ndims,size,fortran_order);
  v = new float[size];
  read_array(fp, v, size, word_size);

  int i,j,k;
  for(i=0; i<npx; i++)
    for(j=0; j<npy; j++)
      for(k=0; k<npz; k++)
	{
	  int idx = i*npy*npz + j*npz + k;
	  // printf("%lessd %d %d %.6f\n",i,j,k,v[idx]);
	}

  fclose(fp);
}

int WeightingPotentialMap::Dump()
{
  int i,j,k;
  for(i=0; i<npx; i++)
    for(j=0; j<npy; j++)
      for(k=0; k<npz; k++)
	{
	  int idx = i*npy*npz + j*npz + k;
	  printf("%d %d %d %.6f\n",i,j,k,v[idx]);
	}
}

float WeightingPotentialMap::GetV(float x, float y, float z)
{
  int i=0;
  int j=0;
  int k=0;

  if( x<=X[0] || x>X[npx-1] || y<=Y[0] || y>Y[npy-1] || z<=Z[0] || z>Z[npz-1] )
    return 0;
    //	throw std::runtime_error("X,Y,Z beyond the domain");

  while(X[i]<x){i++;}
  while(Y[j]<y){j++;}
  while(Z[k]<z){k++;}

  //=======================
  // trilinear interpolation
  float x1 = X[i-1];
  float x2 = X[i];
  float y1 = Y[j-1];
  float y2 = Y[j];
  float z1 = Z[k-1];
  float z2 = Z[k];

  float Q000 = V(i-1,j-1,k-1);
  float Q100 = V(i,j-1,k-1);
  float Q010 = V(i-1,j,k-1);
  float Q110 = V(i,j,k-1);
  float Q001 = V(i-1,j-1,k);
  float Q101 = V(i,j-1,k);
  float Q011 = V(i-1,j,k);
  float Q111 = V(i,j,k);

  float B1 = BilinearInterpolation(Q000, Q010, Q100, Q110, x1, x2, y1, y2, x, y);
  float B2 = BilinearInterpolation(Q001, Q011, Q101, Q111, x1, x2, y1, y2, x, y);

  float Q = LinearInterpolation(z, z1, z2, B1, B2);
  //=======================

  /*
  printf("(%f,%f,%f,%f)-----(%f,%f,%f,%f)\n",x1,y2,z2,Q011,x2,y2,z2,Q111);
  printf("    /                                                       /\n");
  printf("(%f,%f,%f,%f)-----(%f,%f,%f,%f)\n",x1,y1,z2,Q001,x2,y1,z2,Q101);
  printf("   |                                                       |\n");
  printf("(%f,%f,%f,%f)-----(%f,%f,%f,%f)\n",x1,y2,z1,Q010,x2,y2,z1,Q110);
  printf("    /                                                       /\n");
  printf("(%f,%f,%f,%f)-----(%f,%f,%f,%f)\n",x1,y1,z1,Q000,x2,y1,z1,Q100);
  printf("\n");
  printf("(%f,%f,%f,%f)\n",x,y,z,Q);
*/

  return Q;
}

double WeightingPotentialMap::LinearInterpolation(double x, double xmin, double xmax, double ymin, double ymax)
{
  return ymin + (x-xmin)*(ymax-ymin)/(xmax-xmin);
}

/*
(x1,y2,v12)-----(x2,y2,v22)
    |               |
    |               |
(x1,y1,v11)-----(x2,y1,v21)
 */


double WeightingPotentialMap::BilinearInterpolation(float v11, float v12, float v21, float v22, float x1, float x2, float y1, float y2, float x, float y)
{
  float Qxy1 = LinearInterpolation(x,x1,x2,v11,v21);
  float Qxy2 = LinearInterpolation(x,x1,x2,v12,v22);
  float Q = LinearInterpolation(y,y1,y2,Qxy1,Qxy2);
  return Q;
}
