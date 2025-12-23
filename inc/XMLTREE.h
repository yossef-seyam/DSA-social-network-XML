#ifndef XMLTREE_H
#define XMLTREE_H

#include "XMLNode.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <map>

using namespace std;

class XMLTREE
{
private:
    XMLNode *root;
    bool extractTagValue(const string &block, const string &tag, string &out);
    int getNodeIndexByID(string id);
    string toLower(string s);
    vector<string> extractTopics(const string &topicsBlock);



public:
    XMLTREE();
    ~XMLTREE();

    // Part 1: Parsing Helpers
    void parseXML(string xmlText);
    XMLNode *getRoot();

    // Part 2: Graph Builders
    // Returns list of FOLLOWERS (Who follows User I) - Team A's Logic
    vector<vector<XMLNode *>> buildAdjacencyList();

    // Returns list of FOLLOWING (Who User I follows) - Needed for Analysis
    vector<vector<XMLNode *>> buildFollowersList();
    vector<vector<int>> buildFollowingList();

    // Part 3: Analysis Commands
    void print_graph(vector<vector<XMLNode *>> adjList);
    void solveMostActive();
    void solveMostInfluencer();
    void solveMutual(vector<string> ids);
    void solveSuggest(string id);
    void saveGraphCSV(string filename);

    void searchByWord(const string &xml, const string &word);
    void searchByTopic(const string &xml, const string &topic);

    void saveGraphPNG(string filename);
};

#endif
