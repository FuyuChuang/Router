/****************************************************************************
  FileName  [ module.h ]
  Synopsis  [ Define the data structures for the components on the chip. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.12 ]
****************************************************************************/
#ifndef MODULE_H
#define MODULE_H

#include <string>
using namespace std;

class Pin
{
public:
    Pin() {}
    Pin(int x, int y, size_t id, string name) :
        _x(x), _y(y), _id(id), _name(name) {}
    ~Pin() {}

    // data members
    int         _x;         // x-coordinate of the pin
    int         _y;         // y-coordinate of the pin
    size_t      _id;        // id of the pin
    string      _name;      // name of the pin
};

class Edge
{
public:
    Edge() {}
    Edge(size_t s, size_t t, int cost) :
        _s(s), _t(t), _cost(cost) {}
    ~Edge() {}

    // data members
    int         _cost;      // cost of the edge
    size_t      _s;         // source
    size_t      _t;         // sink
};

class Query
{
public:
    Query(size_t w, size_t u, const Edge& edge) :
        _w(w), _u(u), _cEdge(edge) {}
    ~Query() {}

    // data members
    size_t      _w;         // neighbor pin of either s or t of the cEdge
    size_t      _u;         // either s or t
    Edge        _cEdge;     // the edge that w is trying to connect to
    Edge        _dEdge;     // the longest edge on the created cycle (to be queried)
    int         _gain;      // gain from deleting edge
};

// key_compare for std::set (rb-tree)
struct PinCmpXInc
{
    bool operator () (const Pin& p1, const Pin& p2) const {
        return (p1._x < p2._x);
    }
};

struct PinCmpYInc
{
    bool operator () (const Pin& p1, const Pin& p2) const {
        return (p1._y < p2._y);
    }
};

struct PinCmpXDec
{
    bool operator () (const Pin& p1, const Pin& p2) const {
        return !(p1._x < p2._x);
    }
};

struct PinCmpYDec
{
    bool operator () (const Pin& p1, const Pin& p2) const {
        return !(p1._y < p2._y);
    }
};

// sorting
struct SortPinId
{
    bool operator () (Pin& p1, Pin& p2) {
        return (p1._id < p2._id);
    }
};

struct SortEdgeCost
{
    bool operator () (Edge& e1, Edge& e2) {
        return (e1._cost < e2._cost);
    }
};

// bottom-left to upper-right
// find nearest points in R1, R2
struct SortPinBLUR
{
    bool operator () (Pin& p1, Pin& p2) {
        return ((p1._x + p1._y) < (p2._x + p2._y));
    }
};

// upper-left to bottom-right
// find nearest points in R3, R4
struct SortPinULBR
{
    bool operator () (Pin& p1, Pin& p2) {
        return ((p1._x - p1._y) < (p2._x - p2._y));
    }
};

struct SortQueryGain
{
    bool operator () (Query& q1, Query& q2) {
        return (q1._gain < q2._gain);
    }
};

#endif // MODULE_H
