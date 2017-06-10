/****************************************************************************
  FileName  [ module.h ]
  Synopsis  [ Define the data structures for the components on the chip. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.9 ]
****************************************************************************/
#ifndef MODULE_H
#define MODULE_H

#include <string>
using namespace std;

class Pin
{
public:
    Pin()   { }
    Pin(int x, int y, size_t id, string name) :
        _x(x), _y(y), _id(id), _name(name)   { }
    ~Pin()  { }

    // data members
    int         _x;         // x-coordinate of the pin
    int         _y;         // y-coordinate of the pin
    size_t      _id;        // id of the pin
    string      _name;      // name of the pin
};


class Edge
{
public:
    Edge(size_t s, size_t t, int cost) :
        _s(s), _t(t), _cost(cost)    {}
    ~Edge() {}

    // data members
    int         _cost;      // cost of the edge
    size_t      _s;         // source
    size_t      _t;         // sink
};

// key_compare for std::set
struct PinCmpX
{
    bool operator () (const Pin& p1, const Pin& p2) const {
        return (p1._x < p2._x);
    }
};

struct PinCmpY
{
    bool operator () (const Pin& p1, const Pin& p2) const {
        return (p1._y < p2._y);
    }
};

struct SortPinId
{
    bool operator () (Pin& p1, Pin& p2) {
        return (p1._id < p2._id);
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

#endif // MODULE_H
