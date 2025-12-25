#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include <map>


// We turn the graph adjacency list into a csv format and write it to a csv file
// the python library matplot uses this file to draw the graph connections

void XMLTREE::saveGraphCSV(string filename)
{
    ofstream out(filename);
    if (!out)
        return;

    vector<string> indexToID;
    if (root)
    {
        for (XMLNode *u : root->getChildren())
        {
            for (XMLNode *c : u->getChildren())
            {
                if (c->getTagName() == "id")
                {
                    indexToID.push_back(trim(c->getInnerText()));
                    break;
                }
            }
        }
    }

    vector<vector<XMLNode *>> outgoing = buildFollowersList();
    for (int i = 0; i < outgoing.size(); i++)
    {
        if (i >= indexToID.size())
            break;
        string sourceID = indexToID[i];

        for (XMLNode *f : outgoing[i])
        {
            string targetID = "";
            for (XMLNode *c : f->getChildren())
                if (c->getTagName() == "id")
                    targetID = trim(c->getInnerText());

            if (!targetID.empty())
                out << sourceID << "," << targetID << "\n";
        }
    }
    out.close();
    cout << "Graph CSV saved to " << filename << endl;
}