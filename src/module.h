/****************************************************************************
  FileName  [ module.h ]
  Synopsis  [ Define the data structures for the components on the chip. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.12 ]
****************************************************************************/
#ifndef MODULE_H
#define MODULE_H

using namespace std;

class Pin
{
public:
    Pin() {}
    Pin(int x, int y, size_t id, string name = "") :
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
        _w(w), _u(u), _cEdge(edge), _c(0) {}
    ~Query() {}

    // data members
    size_t      _w;         // neighbor pin of either s or t of the cEdge
    size_t      _u;         // either s or t
    size_t      _c;         // counter for dfs
    Edge        _cEdge;     // the edge that w is trying to connect to
    Edge        _dEdge;     // the longest edge on the created cycle
    int         _gain;      // gain from deleting edge
};

// sorting
struct SortEdgeCost
{
    bool operator () (Edge& e1, Edge& e2) {
        return (e1._cost < e2._cost);
    }
};

struct SortQueryGain
{
    bool operator () (Query& q1, Query& q2) {
        return (q1._gain > q2._gain);
    }
};

struct EdgeCmp
{
    bool operator () (const Edge& e1, const Edge& e2) const {
        return (e1._t < e2._t || (e1._t == e2._t && e1._s < e2._s));
    }
};

#endif // MODULE_H
