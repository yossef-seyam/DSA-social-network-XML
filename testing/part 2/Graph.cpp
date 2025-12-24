// Graph.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Graph.h"
#include "XMLTree.h"
#include "XMLNode.h"

vector<vector<XMLNode*>> buildFollowersList(XMLTREE* tree)
{
    vector<vector<XMLNode*>> adjList;
    if (!tree)
        return adjList;

    XMLNode* root = tree->root;
    if (!root)
        return adjList;

    adjList.resize(root->getChildren().size());

    int idx = 0;
    for (XMLNode* each_user : root->getChildren())
    {
        for (XMLNode* c : each_user->getChildren())
        {
            if (c->getTagName() == "followers")
            {
                for (XMLNode* f : c->getChildren())
                {
                    adjList[idx].push_back(f);
                }
                break;
            }
        }
        idx++;
    }
    return adjList;
}
