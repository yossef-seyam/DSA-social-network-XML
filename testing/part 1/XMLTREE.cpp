#include "XMLTree.h"
#include "XMLNode.h"
#include "Utils.h"

void XMLTREE::parseXML(string xmlText)
{
    int i = 0;
    stack<XMLNode *> Stack;
    XMLNode *head = nullptr;

    while (i < xmlText.length())
    {
        // Closing tag
        if (xmlText[i] == '<' && i + 1 < xmlText.length() && xmlText[i + 1] == '/')
        {
            if (!Stack.empty())
                Stack.pop();

            while (i < xmlText.length() && xmlText[i] != '>')
                i++;
            i++;
            continue;
        }

        // Opening tag
        if (xmlText[i] == '<')
        {
            string name;
            i++;
            while (i < xmlText.length() && xmlText[i] != '>')
            {
                name += xmlText[i];
                i++;
            }
            i++;

            if (Stack.empty())
            {
                head = new XMLNode(name, nullptr);
                Stack.push(head);
            }
            else
            {
                XMLNode *node = new XMLNode(name, Stack.top());
                Stack.top()->addChild(node);
                Stack.push(node);
            }
            continue;
        }

        // Inner text
        string textContent;
        while (i < xmlText.length() && xmlText[i] != '<')
        {
            textContent += xmlText[i];
            i++;
        }

        if (!Stack.empty())
        {
            string cleanText = trim(textContent);
            if (!cleanText.empty())
                Stack.top()->innerText = cleanText;
        }
    }

    root = head;
    cout << "Finished parsing. Root: "
         << (root ? root->getTagName() : "nullptr") << endl;
}