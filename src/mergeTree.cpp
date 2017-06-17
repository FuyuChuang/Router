/****************************************************************************
  FileName  [ mergeTree.cpp ]
  Synopsis  [ Implementation of the merging tree. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.15 ]
****************************************************************************/
#include <numeric>
#include <iostream>
#include <cassert>
#include <algorithm>
#include "mergeTree.h"
using namespace std;

MergeTree::MergeTree(const vector<Pin>& pinList) :
    _pinList(pinList), _mergeCount(pinList.size())
{
    size_t pinNum = _pinList.size();

    // disjoint-set setup
    this->makeSet(pinNum);

    // binary merging tree setup
    _mergeTree.resize(2 * pinNum - 1);
    _mergePar.resize(2 * pinNum - 1);
    _mergeRoot.resize(pinNum);
    iota(_mergePar.begin(), _mergePar.end(), 0);
    iota(_mergeRoot.begin(), _mergeRoot.end(), 0);

    // query setup
    _queryId.resize(pinNum);
}

// disjoint-set operations
void MergeTree::makeSet(size_t n)
{
    _par.resize(n);
    _rank.resize(n);
    fill(_rank.begin(), _rank.end(), 0);
    iota(_par.begin(), _par.end(), 0);
    return;
}

void MergeTree::unionSet(size_t x, size_t y, size_t t)
{
    if (_rank[x] < _rank[y]) {
        _par[x] = y;
        _mergeRoot[y] = t;
    }
    else if (_rank[x] > _rank[y]) {
        _par[y] = x;
        _mergeRoot[x] = t;
    }
    else {
        _par[x] = y;
        _rank[y] += 1;
        _mergeRoot[y] = t;
    }
    return;
}

bool MergeTree::sameSet(size_t x, size_t y)
{
    return (findSet(x) == findSet(y));
}

size_t MergeTree::findSet(size_t x)
{
    if (_par[x] == x) return x;
    return findSet(_par[x]);
}

// binary merging tree operations
void MergeTree::addEdge(const Edge& edge)
{
    size_t x = findSet(edge._s);
    size_t y = findSet(edge._t);
    size_t xRoot = _mergeRoot[x];
    size_t yRoot = _mergeRoot[y];
    this->unionSet(x, y, _mergeCount);
    _mergeTree[_mergeCount++] = Node(edge, xRoot, yRoot);

    return;
}

size_t MergeTree::findLca(size_t x)
{
    if (_mergePar[x] == x) return x;
    return findLca(_mergePar[x]);
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
    answerQueryRec(_mergeCount - 1);
    for (size_t i = 0, end = _queryList.size(); i < end; ++i) {
        Query& query = _queryList[i];
        const Edge& cEdge = query._cEdge;
        const Edge& dEdge = query._dEdge;
        getQueryGain(query, cEdge, dEdge);
    }
    return;
}

void MergeTree::answerQueryRec(size_t idx)
{
    if (idx < _pinList.size()) {
        // vertice
        for (size_t i = 0, end = _queryId[idx].size(); i < end; ++i) {
            Query& query = _queryList[_queryId[idx][i]];
            query._c += 1;
            if (query._c == 2) {
                size_t n = (idx == query._w)? query._u: query._w;
                size_t lca = findLca(n);
                query._dEdge = _mergeTree[lca]._edge;
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

void MergeTree::getQueryList(vector<Query>& queryList) const
{
    queryList.clear();
    for (size_t i = 0, end = _queryList.size(); i < end; ++i) {
        if (_queryList[i]._gain > 0) {
            queryList.push_back(_queryList[i]);
        }
    }
    sort(queryList.begin(), queryList.end(), SortQueryGain());

    return;
}

// private member functions
bool MergeTree::sameEdge(const Edge& e1, const Edge& e2)
{
    return (e1._s == e2._s && e1._t == e2._t);
}

void MergeTree::getQueryGain(Query& query, const Edge& cEdge, const Edge& dEdge)
{
    if (sameEdge(cEdge, dEdge)) {
        query._gain = -1;
        return;
    }

    int x = _pinList[query._w]._x;
    int y = _pinList[query._w]._y;
    int sx = _pinList[cEdge._s]._x;
    int sy = _pinList[cEdge._s]._y;
    int tx = _pinList[cEdge._t]._x;
    int ty = _pinList[cEdge._t]._y;

    query._gain = dEdge._cost;
    query._gain -= (x > max(sx, tx))? abs(x - max(sx, tx)): 0;
    query._gain -= (x < min(sx, tx))? abs(x - min(sx, tx)): 0;
    query._gain -= (y > max(sy, ty))? abs(y - max(sy, ty)): 0;
    query._gain -= (y < min(sy, ty))? abs(y - min(sy, ty)): 0;

    return;
}
