/****************************************************************************
  FileName  [ util.cpp ]
  Synopsis  [ Implementation of utility functions. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.9 ]
****************************************************************************/
#include "util.h"
using namespace std;

void getCoordinate(int& x, int& y, string& str)
{
    size_t pos = str.find_first_of(',');
    size_t last = str.find_last_of(')');
    x = stoi(str.substr(1, pos-1));
    y = stoi(str.substr(pos+1, last-pos-1));
    return;
}
