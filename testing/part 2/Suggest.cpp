// SUGGEST: Friends of Friends (Outgoing -> Outgoing)
void XMLTREE::solveSuggest(string id)
{
    int uIdx = getNodeIndexByID(id);
    if (uIdx == -1)
    {
        cout << "User not found." << endl;
        return;
    }

    vector<vector<XMLNode *>> outgoing = buildFollowersList();
    map<string, int> scores;

    // Who do I follow?
    for (XMLNode *friendNode : outgoing[uIdx])
    {
        string friendID = "";
        for (XMLNode *c : friendNode->getChildren())
            if (c->getTagName() == "id")
                friendID = trim(c->getInnerText());

        int friendIdx = getNodeIndexByID(friendID);
        if (friendIdx == -1)
            continue;

        // Who do THEY follow?
        for (XMLNode *fofNode : outgoing[friendIdx])
        {
            string fofID = "";
            for (XMLNode *c : fofNode->getChildren())
                if (c->getTagName() == "id")
                    fofID = trim(c->getInnerText());

            if (fofID == id)
                continue; // Don't suggest myself

            // Check if I already follow fofID
            bool alreadyFollow = false;
            for (XMLNode *myF : outgoing[uIdx])
            {
                string myFID = "";
                for (XMLNode *c : myF->getChildren())
                    if (c->getTagName() == "id")
                        myFID = trim(c->getInnerText());
                if (myFID == fofID)
                    alreadyFollow = true;
            }

            if (!alreadyFollow)
                scores[fofID]++;
        }
    }

    cout << "Suggestions for User " << id << ": ";
    if (scores.empty())
        cout << "None";
    for (auto const &[sID, count] : scores)
    {
        int idx = getNodeIndexByID(sID);
        string name = sID;
        if (idx != -1)
        {
            for (XMLNode *c : root->getChildren()[idx]->getChildren())
                if (c->getTagName() == "name")
                    name = c->getInnerText();
        }
        cout << name << " ";
    }
    cout << endl;
}
