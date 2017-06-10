/****************************************************************************
  FileName  [ router.cpp ]
  Synopsis  [ Implementation of the router. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.9 ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <cassert>
#include <set>
#include "router.h"
#include "util.h"
using namespace std;

#define INF 1000000000

int distance(const Pin& p1, const Pin& p2)
{
    return (abs(p1._x - p2._x) + abs(p1._y - p2._y));
}

bool inRegion(int r, const Pin& p1, const Pin& p2)
{
    switch(r) {
        case 1:
            return (p1._x - p1._y >= p2._x - p2._y);
        case 2:
            return (p1._x - p1._y <= p2._x - p2._y);
        case 3:
            return (p1._x + p1._y <= p2._x + p2._y);
        case 4:
            return (p1._x + p1._y >= p2._x + p2._y);
        default:
            assert(0);
            break;
    }
    return false;
}

void Router::parseInput(fstream& inFile)
{
    string str;
    // read chip boundary
    inFile >> str;
    assert(str == "Boundary");
    for (size_t i = 0; i < 3; ++i) {
        inFile >> str;
        if (i == 1) {
            str = str.substr(0, str.length()-1);
            getCoordinate(_xmin, _ymin, str);
        }
        if (i == 2) {
            getCoordinate(_xmax, _ymax, str);
        }
    }

    // read pin number
    inFile >> str;
    assert(str == "NumPins");
    for (size_t i = 0; i < 2; ++i) {
        inFile >> str;
        if (i == 1) {
            _pinNum = stoi(str);
        }
    }

    // read pins
    for (size_t i = 0; i < _pinNum; ++i) {
        string name;
        int x, y;
        for (size_t j = 0; j < 3; ++j) {
            inFile >> str;
            if (j == 0)
                assert(str == "PIN");
            else if (j == 1)
                name = str;
            else
                getCoordinate(x, y, str);
        }
        _pinList.push_back(Pin(x, y, i, name));
    }
    return;
}

void Router::spanningGraph(vector<Edge>& edgeList)
{
    // active set
    set<Pin, PinCmpX> actSet1;
    set<Pin, PinCmpY> actSet2;
    set<Pin, PinCmpX>::iterator it1, tmp_it1;
    set<Pin, PinCmpY>::iterator it2, tmp_it2;

    // temporary pin
    Pin tmp_pin;
    int tmp_cost;

    // sort by x + y
    sort(_pinList.begin(), _pinList.end(), SortPinBLUR());
    actSet1.clear();
    actSet2.clear();
    actSet1.insert(Pin(-INF, INF, _pinNum, ""));
    actSet2.insert(Pin(INF, -INF, _pinNum, ""));

    for (size_t i = 0; i < _pinNum; ++i) {
        // region 1
        it1 = --actSet1.upper_bound(_pinList[i]);
        tmp_cost = INF;
        while (inRegion(1, *it1, _pinList[i])) {
            assert(it1 != actSet1.begin());
            // cout << "region 1: " << it1->_id << " " << _pinList[i]._id << endl;
            if (distance(*it1, _pinList[i]) < tmp_cost) {
                tmp_pin = *it1;
                tmp_cost = distance(*it1, _pinList[i]);
            }
            tmp_it1 = it1;
            --it1;
            actSet1.erase(tmp_it1);
        }
        if (tmp_cost < INF) {
            edgeList.push_back(Edge(tmp_pin._id, _pinList[i]._id, tmp_cost));
        }
        actSet1.insert(_pinList[i]);

        // region 2
        it2 = --actSet2.upper_bound(_pinList[i]);
        tmp_cost = INF;
        while (inRegion(2, *it2, _pinList[i])) {
            assert(it2 != actSet2.begin());
            // cout << "region 2: " << it2->_id << " " << _pinList[i]._id << endl;
            if (distance(*it2, _pinList[i]) < tmp_cost) {
                tmp_pin = *it2;
                tmp_cost = distance(*it2, _pinList[i]);
            }
            tmp_it2 = it2;
            --it2;
            actSet2.erase(tmp_it2);
        }
        if (tmp_cost < INF) {
            edgeList.push_back(Edge(tmp_pin._id, _pinList[i]._id, tmp_cost));
        }
        actSet2.insert(_pinList[i]);
    }

    // sort by x - y
    sort(_pinList.begin(), _pinList.end(), SortPinULBR());
    actSet1.clear();
    actSet2.clear();
    actSet1.insert(Pin(-INF, -INF, _pinNum, ""));
    actSet2.insert(Pin(INF, INF, _pinNum, ""));
    for (size_t i = 0; i < _pinNum; ++i) {
        // region 4
        it1 = --actSet1.upper_bound(_pinList[i]);
        tmp_cost = INF;
        while (inRegion(4, *it1, _pinList[i])) {
            assert(it1 != actSet1.begin());
            // cout << "region 4: " << it1->_id << " " << _pinList[i]._id << endl;
            if (distance(*it1, _pinList[i]) < tmp_cost) {
                tmp_pin = *it1;
                tmp_cost = distance(*it1, _pinList[i]);
            }
            tmp_it1 = it1;
            --it1;
            actSet1.erase(tmp_it1);
        }
        if (tmp_cost < INF) {
            edgeList.push_back(Edge(tmp_pin._id, _pinList[i]._id, tmp_cost));
        }
        actSet1.insert(_pinList[i]);

        // region 3
        it2 = actSet2.lower_bound(_pinList[i]);
        tmp_cost = INF;
        while (inRegion(3, *it2, _pinList[i])) {
            assert(it2 != actSet2.end());
            // cout << "region 3: " << it2->_id << " " << _pinList[i]._id << endl;
            if (distance(*it2, _pinList[i]) < tmp_cost) {
                tmp_pin = *it2;
                tmp_cost = distance(*it2, _pinList[i]);
            }
            tmp_it2 = it2;
            ++it2;
            actSet2.erase(tmp_it2);
        }
        if (tmp_cost < INF)
            edgeList.push_back(Edge(tmp_pin._id, _pinList[i]._id, tmp_cost));
        actSet2.insert(_pinList[i]);
    }

    // reset order
    std::sort(_pinList.begin(), _pinList.end(), SortPinId());

    return;
}

void Router::EucSpanningTree(vector<Edge>& edgeList)
{

    return;
}

void Router::RecSpanningTree()
{

    return;
}

void Router::steinerTree()
{

    return;
}

void Router::route()
{
    vector<Edge> edgeList;
    this->spanningGraph(edgeList);
    _treeList = edgeList;

    return;
}

// reporting functions
void Router::reportPin() const
{
    cout << "Number of pins: " << _pinNum << endl;
    assert(_pinNum == _pinList.size());
    for (size_t i = 0; i < _pinNum; ++i) {
        cout << left << setw(6) << _pinList[i]._name << " ("
             << _pinList[i]._x << "," << _pinList[i]._y << ")" << endl;
    }
    return;
}

void Router::reportEdge() const
{
    cout << "Number of edges: " << _treeList.size() << endl;
    for (size_t i = 0, end = _treeList.size(); i < end; ++i) {
        cout << left << setw(6) << i << setw(8) << _treeList[i]._s
             << setw(8) << _treeList[i]._t << endl;
    }

    return;
}

void Router::printSummary() const
{

    return;
}

void Router::writeResult(fstream& outFile)
{

    return;
}
