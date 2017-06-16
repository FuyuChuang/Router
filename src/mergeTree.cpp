/****************************************************************************
  FileName  [ mergeTree.cpp ]
  Synopsis  [ Implementation of the merging tree. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.15 ]
****************************************************************************/
#include <numeric>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include "mergeTree.h"
using namespace std;

MergeTree::MergeTree(const vector<Pin>& pinList) :
    _pinList(pinList), _pinNum(pinList.size()), _mergeTop(pinList.size())
{
    // disjoint-set setup
    this->makeSet();

    // binary merging tree setup
    _mergeTree.resize(2 * _pinNum - 1);
    _mergePar.resize(2 * _pinNum - 1);
    _mergePtr.resize(_pinNum);
    iota(_mergePtr.begin(), _mergePtr.end(), 0);

    // query setup
    _queryId.resize(_pinNum);
}

// disjoint-set operations
void MergeTree::makeSet()
{
    _par.resize(_pinNum);
    _rank.resize(_pinNum);
    fill(_rank.begin(), _rank.end(), 0);
    iota(_par.begin(), _par.end(), 0);
    return;
}

void MergeTree::unionSet(size_t x, size_t y, size_t t)
{
    if (_rank[x] < _rank[y]) {
        _par[x] = y;
        _mergePtr[y] = t;
    }
    else if (_rank[x] > _rank[y]) {
        _par[y] = x;
        _mergePtr[x] = t;
    }
    else {
        _par[x] = y;
        _rank[y] += 1;
        _mergePtr[y] = t;
    }
    return;
}

bool MergeTree::sameSet(size_t x, size_t y)
{
    return (find(x, _par) == find(y, _par));
}

size_t MergeTree::find(size_t x, vector<size_t>& par)
{
    if (par[x] == x) return x;
    return find(par[x], par);
}

// binary merging tree operations
void MergeTree::addEdge(const Edge& edge)
{
    size_t rx = find(edge._s, _par);
    size_t ry = find(edge._t, _par);
    size_t xPtr = _mergePtr[rx];
    size_t yPtr = _mergePtr[ry];
    this->unionSet(rx, ry, _mergeTop);
    _mergeTree[_mergeTop++] = Node(edge, xPtr, yPtr);

    return;
}

// query operations
void MergeTree::addQuery(size_t w, size_t u, const Edge& edge)
{
    _queryList.push_back(Query(w, u, edge));
    _queryId[w].push_back(_queryList.size() - 1);
    _queryId[u].push_back(_queryList.size() - 1);

    return;
}

void MergeTree::answerQuery()
{
    iota(_mergePar.begin(), _mergePar.end(), 0);

    answerQueryRec(_mergeTop - 1);
    for (size_t i = 0, end = _queryList.size(); i < end; ++i) {
        Query& query = _queryList[i];
        const Edge& cEdge = query._cEdge;
        const Edge& dEdge = query._dEdge;
        if (sameEdge(cEdge, dEdge)) {
            query._gain = -1;
            continue;
        }
        getQueryGain(query, cEdge, dEdge);
    }
    return;
}

void MergeTree::answerQueryRec(size_t idx)
{
    if (idx < _pinNum) {
        // vertice
        for (size_t i = 0, end = _queryId[idx].size(); i < end; ++i) {
            Query& query = _queryList[_queryId[idx][i]];
            query._counter += 1;
            if (query._counter == 2) {
                size_t n = (idx == query._w)? query._u: query._w;
                size_t root = find(n, _mergePar);
                query._dEdge = _mergeTree[root]._edge;
            }
        }
    }
    else {
        // edge
        answerQueryRec(_mergeTree[idx]._left);
        _mergePar[_mergeTree[idx]._left] = idx;
        answerQueryRec(_mergeTree[idx]._right);
        _mergePar[_mergeTree[idx]._right] = idx;
    }

    return;
}

void MergeTree::getQueryResult(vector<Query>& queryResult) const
{
    queryResult.clear();
    for (size_t i = 0, end = _queryList.size(); i < end; ++i) {
        if (_queryList[i]._gain <= 0)
            continue;
        queryResult.push_back(_queryList[i]);
    }
    sort(queryResult.begin(), queryResult.end(), SortQueryGain());

    return;
}

// private member functions
bool MergeTree::sameEdge(const Edge& e1, const Edge& e2)
{
    return (e1._s == e2._s && e1._t == e2._t);
}

void MergeTree::getQueryGain(Query& query, const Edge& cEdge, const Edge& dEdge)
{
    int x = _pinList[query._w]._x;
    int y = _pinList[query._w]._y;
    int sx = _pinList[cEdge._s]._x;
    int sy = _pinList[cEdge._s]._y;
    int tx = _pinList[cEdge._t]._x;
    int ty = _pinList[cEdge._t]._y;

    query._gain = dEdge._cost;
    if (x >= max(sx, tx)) {
        query._gain -= (x - max(sx, tx));
    }
    else if (x <= min(sx, tx)) {
        query._gain -= (min(sx, tx) - x);
    }
    if (y >= max(sy, ty)) {
        query._gain -= (y - max(sy, ty));
    }
    else if (y <= min(sy, ty)) {
        query._gain -= (min(sy, ty) - y);
    }

    return;
}
