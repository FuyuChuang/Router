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
#include <map>
#include <boost/pending/disjoint_sets.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "router.h"
#include "util.h"
using namespace std;
using namespace cv;

using DisjointSet = boost::disjoint_sets<size_t*, size_t*>;

auto larger = [](int x, int y) { return x > y; };
using rev_multimap = multimap<int, int, decltype(larger)>;

// get Manhattan distance
int getDistance(const Pin& p1, const Pin& p2)
{
    return (abs(p1._x - p2._x) + abs(p1._y - p2._y));
}

// check in region
bool inRegion(int r, const Pin& p1, const Pin& p2)
{
    switch(r) {
        case 1:
            return (p1._x - p1._y > p2._x - p2._y);
        case 2:
            return (p1._x - p1._y <= p2._x - p2._y);
        case 3:
            return (p1._x + p1._y < p2._x + p2._y);
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
    rev_multimap actSet1(larger);
    rev_multimap actSet2(larger);
    multimap<int, int> actSet3;
    rev_multimap actSet4(larger);

    vector<int> sorted_pin_ids;
    for (size_t i = 0, end = _pinList.size(); i < end; ++i) {
        sorted_pin_ids.push_back(i);
    }

    // Sort by x + y
    sort(sorted_pin_ids.begin(), sorted_pin_ids.end(),
         [&](int id1, int id2) {
         const Pin& pin1 = _pinList[id1];
         const Pin& pin2 = _pinList[id2];
         return pin1._x + pin1._y < pin2._x + pin2._y;
    });

    for (int current_pin_id : sorted_pin_ids) {
        const Pin& current_pin = _pinList[current_pin_id];
        actSet1.insert(make_pair(current_pin._x, current_pin_id));
        actSet2.insert(make_pair(current_pin._y, current_pin_id));

        // Region 1
        for (auto it = actSet1.lower_bound(current_pin._x); it != actSet1.end();) {
            const int pin_id = it->second;
            if (pin_id != current_pin_id) {
                const Pin& pin = _pinList[pin_id];
                if (inRegion(1, pin, current_pin)) {
                    edgeList.push_back(Edge(pin_id, current_pin_id, getDistance(pin, current_pin)));
                    it = actSet1.erase(it);
                }
                else {
                    break;
                }
            }
            else {
                ++it;
            }
        }

        // Region 2
        for (auto it = actSet2.upper_bound(current_pin._y); it != actSet2.end();) {
            const int pin_id = it->second;
            if (pin_id != current_pin_id) {
                const Pin& pin = _pinList[pin_id];
                if (inRegion(2, pin, current_pin)) {
                    edgeList.push_back(Edge(pin_id, current_pin_id, getDistance(pin, current_pin)));
                    it = actSet2.erase(it);
                }
                else {
                    break;
                }
            }
            else {
                ++it;
            }
        }
    }

    // sort by x - y
    sort(sorted_pin_ids.begin(), sorted_pin_ids.end(),
         [&](int id1, int id2) {
         const Pin& pin1 = _pinList[id1];
         const Pin& pin2 = _pinList[id2];
         return pin1._x - pin1._y < pin2._x - pin2._y;
    });

    for (int current_pin_id : sorted_pin_ids) {
        const Pin& current_pin = _pinList[current_pin_id];
        actSet3.insert(make_pair(current_pin._y, current_pin_id));
        actSet4.insert(make_pair(current_pin._x, current_pin_id));

        // Region 3
        for (auto it = actSet3.lower_bound(current_pin._y); it != actSet3.end();) {
            const int pin_id = it->second;
            if (pin_id != current_pin_id) {
                const Pin& pin = _pinList[pin_id];
                if (inRegion(3, pin, current_pin)) {
                    edgeList.push_back(Edge(pin_id, current_pin_id, getDistance(pin, current_pin)));
                    it = actSet3.erase(it);
                }
                else {
                    break;
                }
            }
            else {
                ++it;
            }
        }

        // Region 4
        for (auto it = actSet4.upper_bound(current_pin._x); it != actSet4.end();) {
            const int pin_id = it->second;
            if (pin_id != current_pin_id) {
                const Pin& pin = _pinList[pin_id];
                if (inRegion(4, pin, current_pin)) {
                    edgeList.push_back(Edge(pin_id, current_pin_id, getDistance(pin, current_pin)));
                    it = actSet4.erase(it);
                }
                else {
                    break;
                }
            }
            else {
                ++it;
            }
        }
    }

    return;
}

void Router::genSpanningTree(vector<Edge>& edgeList)
{
    _treeList.clear();
    // build up adjacency list
    /*
    vector<vector<size_t> > adjList(_pinNum);
    for (size_t i = 0, end = edgeList.size(); i < end; ++i) {
        const Edge& edge = edgeList[i];
        adjList[edge._s].push_back(edge._t);
        adjList[edge._t].push_back(edge._s);
    }
    */

    // Kruskal algorithm for spanning graph
    const int edgeNum = edgeList.size();
    sort(edgeList.begin(), edgeList.end(), SortEdgeCost());
    vector<size_t> ranks(edgeNum);
    vector<size_t> parents(edgeNum);
    DisjointSet disjoint_set(&ranks[0], &parents[0]);

    for (size_t i = 0, end = edgeList.size(); i < end; ++i) {
        disjoint_set.make_set(i);
    }

    for (size_t i = 0, end = edgeList.size(); i < end; ++i) {
        const Edge& edge = edgeList[i];
        const size_t sid = edge._s;
        const size_t tid = edge._t;
        const size_t sset = disjoint_set.find_set(sid);
        const size_t tset = disjoint_set.find_set(tid);
        if (sset != tset) {
            _treeList.push_back(edge);
            disjoint_set.link(sid, tid);
        }
    }

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
            _treeList.push_back(Edge(s._id, _pinNum, getDistance(s, newPin)));
            _treeList[i]._s = _pinNum;
            _treeList[i]._cost = getDistance(t, newPin);
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
        if (s._y == t._y) {
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
        cost += getDistance(s, t);
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
        size_t y = imgY - round(_pinList[i]._y*sf);
        if (i < _oPinNum)
            circle(image, Point(x, y), 5, Scalar(0, 128, 0), CV_FILLED);
        else
            circle(image, Point(x, y), 5, Scalar(0, 0, 128), CV_FILLED);
    }

    for (size_t i = 0, end = _treeList.size(); i < end; ++i) {
        size_t sx = round(_pinList[_treeList[i]._s]._x*sf);
        size_t sy = imgY - round(_pinList[_treeList[i]._s]._y*sf);
        size_t tx = round(_pinList[_treeList[i]._t]._x*sf);
        size_t ty = imgY - round(_pinList[_treeList[i]._t]._y*sf);
        line(image, Point(sx, sy), Point(tx, ty), Scalar(128, 0, 0));
    }

    imwrite(name + ".jpg", image);

    return;
}
