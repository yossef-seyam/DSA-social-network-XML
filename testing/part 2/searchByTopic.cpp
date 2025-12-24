void XMLTREE::searchByTopic(const string &xml, const string &topic)
{
    string lowerTargetTopic = toLower(topic);
    size_t pos = 0;
    bool found = false;
    int postCount = 0;

    // Find all user blocks
    while (true)
    {
        size_t userStart = xml.find("<user>", pos);
        if (userStart == string::npos)
            break;
        size_t userEnd = xml.find("</user>", userStart);
        if (userEnd == string::npos)
            break;

        string userBlock = xml.substr(userStart, userEnd - userStart + 7); // + length of </user>

        string userId, userName;
        extractTagValue(userBlock, "id", userId);
        extractTagValue(userBlock, "name", userName);

        // Find posts inside this user
        size_t postPos = 0;
        while (true)
        {
            size_t postStart = userBlock.find("<post>", postPos);
            if (postStart == string::npos)
                break;
            size_t postEnd = userBlock.find("</post>", postStart);
            if (postEnd == string::npos)
                break;

            string postBlock = userBlock.substr(postStart, postEnd - postStart + 7);
            string postBody;
            extractTagValue(postBlock, "body", postBody);

            size_t topicsStart = postBlock.find("<topics>");
            if (topicsStart != string::npos)
            {
                size_t topicsEnd = postBlock.find("</topics>", topicsStart);
                if (topicsEnd != string::npos)
                {
                    string topicsBlock = postBlock.substr(topicsStart + 8, topicsEnd - (topicsStart + 8));
                    vector<string> topics = extractTopics(topicsBlock);

                    bool topicFound = false;
                    for (const string &t : topics)
                    {
                        if (toLower(t) == lowerTargetTopic)
                        {
                            topicFound = true;
                            break;
                        }
                    }

                    if (topicFound)
                    {
                        cout << "Post #" << ++postCount << ":\n";
                        cout << "  User: " << userName << " (id=" << userId << ")\n";
                        cout << "  Topics: ";
                        for (size_t i = 0; i < topics.size(); ++i)
                        {
                            cout << topics[i] << (i + 1 < topics.size() ? ", " : "");
                        }
                        cout << "\n  Body: " << trim(postBody) << "\n";
                        cout << "------------------------\n";
                        found = true;
                    }
                }
            }
            postPos = postEnd + 7;
        }
        pos = userEnd + 7;
    }

    if (!found)
        cout << "No posts contain the topic: " << topic << endl;
    else
        cout << "Found " << postCount << " post(s) with topic: \"" << topic << "\"\n";

}
