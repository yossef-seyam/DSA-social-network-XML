
// MUTUAL: Who do we BOTH follow? (Intersection of Outgoing Lists)
void XMLTREE::solveMutual(vector<string> ids)
{
    if (ids.empty())
        return;
    vector<vector<XMLNode*>> graph = buildFollowersList();

    // Get Outgoing lists as IDs
    int firstIdx = getNodeIndexByID(ids[0]);
    if (firstIdx == -1)
    {
        cout << "User " << ids[0] << " not found." << endl;
        return;
    }

    // Collect IDs user 1 follows
    vector<string> mutuals;
    for (XMLNode* f : graph[firstIdx])
    {
        for (XMLNode *c : f->getChildren())
            if (c->getTagName() == "id")
                mutuals.push_back(trim(c->getInnerText()));
    }

    for (size_t i = 1; i < ids.size(); i++)
    {
        int idx = getNodeIndexByID(ids[i]);
        if (idx == -1)
            continue;

        vector<string> current;
        for (XMLNode *f : buildFollowersList()[idx])
        {
            for (XMLNode *c : f->getChildren())
                if (c->getTagName() == "id")
                    current.push_back(trim(c->getInnerText()));
        }

        vector<string> intersection;
        for (string m : mutuals)
        {
            for (string c : current)
            {
                if (m == c)
                {
                    intersection.push_back(m);
                    break;
                }
            }
        }
        mutuals = intersection;
    }

    cout << "Mutual Connections (Users they both follow): ";
    if (mutuals.empty())
        cout << "None";
    for (string s : mutuals)
    {
        // Resolve ID to Name for display
        int mIdx = getNodeIndexByID(s);
        if (mIdx != -1)
        {
            for (XMLNode *c : root->getChildren()[mIdx]->getChildren())
                if (c->getTagName() == "name")
                    cout << c->getInnerText() << " ";
        }
        else
        {
            cout << s << " ";
        }
    }
    cout << endl;
}