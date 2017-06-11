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
    void spanningGraph(vector<Edge>& edgeList);
    void EucSpanningTree(vector<Edge>& edgeList);
    void RecSpanningTree();     // called only after EucSpanningTree is called
    void steinerTree();
    void route();

    // reporting functions
    void reportPin() const;
    void reportEdge() const;
    void printSummary() const;
    void writeResult(fstream& outFile);

    // opencv depended
    void drawResult(string name) const;

private:
    int             _xmin;      // chip boundary
    int             _ymin;      // chip boundary
    int             _xmax;      // chip boundary
    int             _ymax;      // chip boundary
    size_t          _pinNum;    // number of pins
    size_t          _sPinNum;   // number of pins plus steiner points
    vector<Pin>     _pinList;   // list of pins
    vector<Edge>    _treeList;  // list of edges
};

#endif // ROUTER_H
