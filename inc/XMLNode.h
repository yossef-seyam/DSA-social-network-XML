#ifndef XMLNODE_H
#define XMLNODE_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class XMLNode {
private:
    string tagName;
    string innerText;
    vector<XMLNode*> children;
    XMLNode* parent;
public:
    XMLNode(string name, XMLNode* p = nullptr);
    void addChild(XMLNode* child) {
        children.push_back(child);
    }
    string getTagName();
    string getInnerText();
    vector<XMLNode*> getChildren();
    XMLNode* getParent();

    friend class XMLTREE;
    ~XMLNode();
};

#endif