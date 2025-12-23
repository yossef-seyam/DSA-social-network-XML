
#ifndef XMLNODE_H
#define XMLNODE_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

// THIS IS OUR STANDARD DATA STRUCTURE
class XMLNode
{
public:
    string tagName;             // Example: "user", "id", "post"
    string innerText;           // Example: "1", "user1", "Lorem ipsum..."
    vector<XMLNode *> children; // The list of children tags (A vector of pointers)
    XMLNode *parent;            // Pointer to the parent (needed for going back up)

    // Constructor: simplified initialization
    XMLNode(string name, XMLNode *p = nullptr)
    {
        tagName = name;
        parent = p;
        innerText = "";
    }

    // Helper to add a child easily
    void addChild(XMLNode *child)
    {
        children.push_back(child);
    }

    // Destructor to clean up memory (Optional but good practice)
    ~XMLNode()
    {
        for (XMLNode *child : children)
        {
            delete child;
        }
    }
};

#endif

