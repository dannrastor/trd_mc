#pragma once

#include <cstdio>
#include <string>

class WeightingPotentialMap
{
public:
  WeightingPotentialMap();
  ~WeightingPotentialMap();

  int ReadFile(std::string fname);

  float GetV(float x, float y, float z);#

  int Dump();

protected:
  float V(int i, int j, int k) {return v[i*npy*npz + j*npz + k];};
  double BilinearInterpolation(float v11, float v12, float v21, float v22, float x1, float x2, float y1, float y2, float x, float y) ;
  double LinearInterpolation(double x, double xmin, double xmax, double ymin, double ymax);
private:
  float* X;
  float* Y;
  float* Z;
  float* v;
  unsigned int npx, npy, npz;
};

void parse_npy_header(FILE* fp, unsigned int& word_size, unsigned int*& shape, unsigned int& ndims, unsigned long& size, bool& fortran_order);

void read_array(FILE* fp, void* data, unsigned long size, unsigned int word_size);

char big_endian_test();
