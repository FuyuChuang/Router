/****************************************************************************
  FileName  [ router.h ]
  Synopsis  [ Define the interface of the router. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.9 ]
****************************************************************************/
#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <fstream>
#include "module.h"
using namespace std;


class Router
{
public:
    // constructor and destructor
    Router(fstream& inFile) {
        this->parseInput(inFile);
    }
    ~Router()   { }

    // basic access methods
    int getXmin() const { return _xmin; }
    int getYmin() const { return _ymin; }
    int getXmax() const { return _xmax; }
    int getYmax() const { return _ymax; }
    int getPinNum() const { return _pinNum; }

    // modify methods
    void parseInput(fstream& inFile);
    void spanningGraph();
    void spanningTree();
    void route();

    // reporting functions
    void reportPin() const;
    void printSummary() const;
    void writeResult(fstream& outFile);

private:
    int             _xmin;      // chip boundary
    int             _ymin;      // chip boundary
    int             _xmax;      // chip boundary
    int             _ymax;      // chip boundary
    size_t          _pinNum;    // number of pins
    vector<Pin>     _pinList;   // list of pins
    vector<Edge>    _edgeList;  // list of edges
};

#endif // ROUTER_H
