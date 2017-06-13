/****************************************************************************
  FileName  [ router.cpp ]
  Synopsis  [ Implementation of the router. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.9 ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cassert>
#include <set>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "router.h"
#include "util.h"
using namespace std;
using namespace cv;

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
            _oPinNum = _pinNum;
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

void Router::genSpanningGraph(vector<Edge>& edgeList)
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
        if (tmp_cost < INF) {
            edgeList.push_back(Edge(tmp_pin._id, _pinList[i]._id, tmp_cost));
        }
        actSet2.insert(_pinList[i]);
    }

    // reset order
    std::sort(_pinList.begin(), _pinList.end(), SortPinId());

    return;
}

void Router::genSpanningTree(vector<Edge>& edgeList)
{
    // build up adjacency list
    vector<vector<size_t> > adjList(_pinNum);
    for (size_t i = 0, end = edgeList.size(); i < end; ++i) {
        const Edge& edge = edgeList[i];
        adjList[edge._s].push_back(edge._t);
        adjList[edge._t].push_back(edge._s);
    }

    // Kruskal algorithm for spanning graph

    // TODO: delete
    _treeList = edgeList;

    return;
}

void Router::genSteinerTree()
{

    return;
}

void Router::rectilinearize()
{
    for (size_t i = 0, end = _treeList.size(); i < end; ++i) {
        Edge& edge = _treeList[i];
        const Pin& s = _pinList[edge._s];
        const Pin& t = _pinList[edge._t];
        if (s._x != t._x && s._y != t._y) {
            Pin newPin(s._x, t._y, _pinNum, "");
            // Note: _pinList.push_back(newPin) should never done before using s and t
            // Due to the redistribution of memory of a vector, calling s and t after
            // push_back() will cause a core dump
            _treeList.push_back(Edge(s._id, _pinNum, distance(s, newPin)));
            _treeList[i]._s = _pinNum;
            _treeList[i]._cost = distance(t, newPin);
            _pinList.push_back(newPin);
            ++_pinNum;
        }
    }

    return;
}

void Router::route()
{
    vector<Edge> edgeList;

    _start = clock();
    this->genSpanningGraph(edgeList);
    this->genSpanningTree(edgeList);
    this->genSteinerTree();
    this->rectilinearize();
    _stop = clock();

    this->printSummary();
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
    cout << "NumRoutedPins = " << _oPinNum << endl;
    cout << "WireLength = " << this->getCost(_treeList) << endl;
    cout << "Time = " << (double)(_stop - _start) / CLOCKS_PER_SEC << " secs " << endl;
    return;
}

void Router::writeResult(fstream& outFile)
{
    outFile << "NumRoutedPins = " << _oPinNum << endl;
    outFile << "WireLength = " << this->getCost(_treeList) << endl;

    for (size_t i = 0, end = _treeList.size(); i < end; ++i) {
        const Pin& s = _pinList[_treeList[i]._s];
        const Pin& t = _pinList[_treeList[i]._t];
        assert(s._x == t._x || s._y == t._y);
        if (s._x == t._x) {
            outFile << "H-line (" << s._x << "," << s._y << ") (" << t._x << "," << t._y << ")" << endl;
        }
        else {
            outFile << "V-line (" << s._x << "," << s._y << ") (" << t._x << "," << t._y << ")" << endl;
        }
    }

    return;
}

long long Router::getCost(const vector<Edge>& treeList) const
{
    long long cost = 0;

    for (size_t i = 0, end = treeList.size(); i < end; ++i) {
        const Pin& s = _pinList[treeList[i]._s];
        const Pin& t = _pinList[treeList[i]._t];
        cost += distance(s, t);
    }

    return cost;
}

// opencv depended
void Router::drawResult(string name) const
{
    // scaling factor
    double sf = 1;
    sf *= (_xmax >= 1000)? pow(0.1, round(log10(_xmax)))*1000: 10;

    size_t imgX = round(_xmax * sf);
    size_t imgY = round(_ymax * sf);
    Mat image(imgY, imgX, CV_8UC3);
    image.setTo(Scalar(255, 255, 255));

    for (size_t i = 0, end = _pinList.size(); i < end; ++i) {
        size_t x = round(_pinList[i]._x*sf);
        size_t y = round(_pinList[i]._y*sf);
        if (i < _oPinNum)
            circle(image, Point(y, x), 5, Scalar(0, 128, 0), CV_FILLED);
        else
            circle(image, Point(y, x), 5, Scalar(0, 0, 128), CV_FILLED);
    }

    for (size_t i = 0, end = _treeList.size(); i < end; ++i) {
        size_t sx = round(_pinList[_treeList[i]._s]._x*sf);
        size_t sy = round(_pinList[_treeList[i]._s]._y*sf);
        size_t tx = round(_pinList[_treeList[i]._t]._x*sf);
        size_t ty = round(_pinList[_treeList[i]._t]._y*sf);
        line(image, Point(sy, sx), Point(ty, tx), Scalar(128, 0, 0));
    }

    imwrite(name + ".jpg", image);

    return;
}
