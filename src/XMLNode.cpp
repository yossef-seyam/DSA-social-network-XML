//
// Created by omara on 12/10/2025.
//

#include "../inc/XMLNode.h"

XMLNode::XMLNode(string name, XMLNode *p) {
    tagName = name;
    parent = p;
    innerText = "";
}

string XMLNode::getTagName() {
    return tagName;
}

string XMLNode::getInnerText() {
    return innerText;
}

vector<XMLNode*> XMLNode::getChildren() {
    return children;
}

XMLNode* XMLNode::getParent() {
    return parent;
}


XMLNode::~XMLNode() {
    for (XMLNode* child : children) {
        delete child;
    }
}


