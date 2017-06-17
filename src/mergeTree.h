/****************************************************************************
  FileName  [ mergeTree.h ]
  Synopsis  [ Define the data structure of the merging tree. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.6.14 ]
****************************************************************************/
#ifndef MERGETREE_H
#define MERGETREE_H

#include <vector>
#include "module.h"
using namespace std;

class Node
{
    friend class MergeTree;

public:
    Node()  {}
    Node(const Edge& edge, size_t left, size_t right, int gain = 0) :
        _edge(edge), _left(left), _right(right), _gain(gain) {}
    ~Node() {}

private:
    Edge        _edge;
    size_t      _left;
    size_t      _right;
    int         _gain;
};

class MergeTree
{
public:
    // constructor and destructor
    // MergeTree() {}
    MergeTree(const vector<Pin>& pinList);
    ~MergeTree() {}

    // disjoint set operations
    void makeSet(size_t n);
    void unionSet(size_t x, size_t y, size_t t);
    bool sameSet(size_t x, size_t y);
    size_t findSet(size_t x);

    // merging tree operations
    void addEdge(const Edge& edge);
    size_t findLca(size_t x);

    // query operations
    void addQuery(size_t x, size_t y, const Edge& edge);
    void answerQuery();
    void answerQueryRec(size_t idx);
    void getQueryList(vector<Query>& queryList) const;

private:
    const vector<Pin>   _pinList;

    // for spanning tree
    vector<size_t>      _par;           // record the parent of each node
    vector<size_t>      _rank;          // record the rank of each node

    // for binary merging tree
    size_t              _mergeCount;
    vector<Node>        _mergeTree;     // binary merging tree
    vector<size_t>      _mergePar;      // record the parent of each node on tree
    vector<size_t>      _mergeRoot;     // record tree roots

    // for query
    vector<Query>           _queryList;
    vector<vector<size_t> > _queryId;

    // private member functions
    bool sameEdge(const Edge& e1, const Edge& e2);
    void getQueryGain(Query& query, const Edge& cEdge, const Edge& dEdge);
};

#endif  // MERGETREE_H
