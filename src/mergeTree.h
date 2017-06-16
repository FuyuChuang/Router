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
    MergeTree(const vector<Pin>& pinList);
    ~MergeTree() {}

    // disjoint set operations
    void makeSet();
    void unionSet(size_t x, size_t y, size_t t);
    bool sameSet(size_t x, size_t y);
    size_t find(size_t x, vector<size_t>& parent);

    // merging tree operations
    void addEdge(const Edge& edge);

    // query operations
    void addQuery(size_t x, size_t y, const Edge& edge);
    void answerQuery();
    void answerQueryRec(size_t idx);
    void getQueryResult(vector<Query>& queryResult) const;

private:
    const vector<Pin>   _pinList;
    size_t              _pinNum;
    size_t              _mergeTop;

    // for spanning tree
    vector<size_t>      _par;           // record the parent of each node
    vector<size_t>      _rank;          // record the rank of each node

    // for binary merging tree
    vector<Node>        _mergeTree;     // binary merging tree
    vector<size_t>      _mergePar;      // record the parent of each node on tree
    vector<size_t>      _mergePtr;      // record tree roots

    // for query
    vector<Query>           _queryList;
    vector<vector<size_t> > _queryId;

    // private member functions
    bool sameEdge(const Edge& e1, const Edge& e2);
    void getQueryGain(Query& query, const Edge& cEdge, const Edge& dEdge);
};

#endif  // MERGETREE_H
