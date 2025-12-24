
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}


bool extractBetween(const string& text, const string& startTag, const string& endTag,
                    size_t& pos, string& out) {
    size_t start = text.find(startTag, pos);
    if (start == string::npos) return false;
    start += startTag.length();

    size_t end = text.find(endTag, start);
    if (end == string::npos) return false;

    out = text.substr(start, end - start);
    pos = end + endTag.length();
    return true;
}

bool extractTagValue(const string& block, const string& tag, string& out) {
    string open = "<" + tag + ">";
    string close = "</" + tag + ">";

    size_t s = block.find(open);
    if (s == string::npos) return false;
    s += open.length();

    size_t e = block.find(close, s);
    if (e == string::npos) return false;

    out = block.substr(s, e - s);
    return true;
}

void searchByWord(const string& file, const string& word) {
    ifstream in(file);
    if (!in) {
        cout << "Cannot open file!\n";
        return;
    }

    string xml, line;
    while (getline(in, line)) xml += line;

    string lowerWord = toLower(word);
    size_t pos = 0;
    bool found = false;

    while (true) {
        size_t bodyPosStart = xml.find("<body>", pos);
        if (bodyPosStart == string::npos) break;

        size_t bodyPosEnd = xml.find("</body>", bodyPosStart);
        if (bodyPosEnd == string::npos) break;

        string body = xml.substr(bodyPosStart + 6, bodyPosEnd - (bodyPosStart + 6));
        string bodyLower = toLower(body);

        pos = bodyPosEnd + 7; 

        if (bodyLower.find(lowerWord) != string::npos) {
           
            size_t userStart = xml.rfind("<user>", bodyPosStart);
            size_t userEnd   = xml.find("</user>", userStart);

            string userBlock = xml.substr(userStart, userEnd - userStart);

            string userId = "(unknown)";
            string userName = "(unknown)";

            extractTagValue(userBlock, "id", userId);
            extractTagValue(userBlock, "name", userName);

            cout << "Found match in post body:\n";
            cout << "  User: " << userName << " (id=" << userId << ")\n";
            cout << "  Body: " << body << "\n";
            cout << "------------------------\n";

            found = true;
        }
    }

    if (!found)
        cout << "No posts contain the word: " << word << endl;
}