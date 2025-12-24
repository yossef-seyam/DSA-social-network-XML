#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

class XMLNode;
class XMLTREE;

using namespace std;

vector<vector<XMLNode*>> buildFollowersList(XMLTREE* tree);

#endif

