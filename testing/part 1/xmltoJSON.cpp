string xmlToJSON(XMLNode *node, int depth, bool isArrayElement)
{
    if (!node)
        return "";

    string indent(depth * 4, ' ');
    string json = "";
    vector<XMLNode *> children = node->getChildren();

    // Leaf node
    if (children.empty())
    {
        return indent + "\"" + node->getTagName() + "\": \"" + node->getInnerText() + "\"";
    }

    // Detect array
    bool allSame = true;
    string tag = children[0]->getTagName();

    for (int i = 1; i < children.size(); i++)
    {
        if (children[i]->getTagName() != tag)
        {
            allSame = false;
            break;
        }
    }

    // ---------- Array Case ----------
    if (allSame)
    {
        if (!isArrayElement)
            json += indent + "\"" + node->getTagName() + "\": [\n";
        else
            json += indent + "[\n";

        for (int i = 0; i < children.size(); i++)
        {
            json += indent + "    {\n";
            json += xmlToJSON(children[i], depth + 2, false) + "\n";
            json += indent + "    }";

            if (i < children.size() - 1)
                json += ",";
            json += "\n";
        }

        json += indent + "]";
        return json;
    }

    // ---------- Object Case ----------
    if (!isArrayElement)
        json += indent + "\"" + node->getTagName() + "\": {\n";
    else
        json += indent + "{\n";

    for (int i = 0; i < children.size(); i++)
    {
        json += xmlToJSON(children[i], depth + 1, false);
        if (i < children.size() - 1)
            json += ",";
        json += "\n";
    }

    json += indent + "}";
    return json;
}