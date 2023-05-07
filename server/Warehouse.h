#ifndef _WAREHOUSE_H
#define _WAREHOUSE_H
#include <iostream>
using namespace std;

class Warehouse {
 public:
  int x;
  int y;
  int id;
  Warehouse(int x, int y, int ID) : x(x), y(y), id(ID) {}
  ~Warehouse() {}
};

#endif