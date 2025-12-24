#ifndef XMLTREE_H
#define XMLTREE_H

#include <string>
#include <vector>
#include <stack>
#include <iostream>

using namespace std;

class XMLNode; // forward declaration

class XMLTREE
{
public:
    void parseXML(string xmlText);
    vector<vector<XMLNode *>> buildAdjacencyList();

private:
    XMLNode *root = nullptr;
};

#endif