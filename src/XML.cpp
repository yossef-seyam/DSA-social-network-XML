#include  "../inc/XML.h"
// #include <bits/stdc++.h>
#include <string>
#include <sstream>
#include <fstream>
bool isAllWhitespace(const string &s)
{
    for (char c : s)
    {
        if (!isspace((unsigned char)c))
            return false;
    }
    return true;
}

string trim(const string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}


/*********




*/


static inline void ltrim(string &s)
{
    size_t p = 0;
    while (p < s.size() && isspace((unsigned char)s[p])) p++;
    s.erase(0, p);
}

static inline void rtrim(string &s)
{
    while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
}

static inline string trim_copy(string s)
{
    ltrim(s);
    rtrim(s);
    return s;
}

static inline int countNewLines(const string &s)
{
    return (int)count(s.begin(), s.end(), '\n');
}

// find '>' for a tag, but ignore '>' that are inside quotes
static int findTagEndRespectQuotes(const string &xml, int startIdx)
{
    bool inQuote = false;
    char q = 0;
    int n = (int)xml.size();

    for (int k = startIdx; k < n; ++k)
    {
        char ch = xml[k];
        if (!inQuote && (ch == '"' || ch == '\''))
        {
            inQuote = true;
            q = ch;
        }
        else if (inQuote && ch == q)
        {
            inQuote = false;
        }
        else if (!inQuote && ch == '>')
        {
            return k;
        }
    }
    return -1;
}




/*********


*/









string prettify(XMLNode *node, int depth)
{

    if (!node)
        return ""; // Safety check

    string indent(depth * 4, ' ');
    string result = indent + "<" + node->getTagName() + ">\n";

    if (!node->getInnerText().empty())
    {
        result += indent + "\t" + node->getInnerText() + "\n";
    }
    vector<XMLNode *> children = node->getChildren();
    for (XMLNode *child : children)
    {
        result += prettify(child, depth + 1); // recursion for each child(node)
    }

    result += indent + "</" + node->getTagName() + ">\n";
    return result;
}
string minify(XMLNode *node)
{

    if (!node)
        return ""; // Safety check

    string result = "<" + node->getTagName() + ">";

    if (!node->getInnerText().empty())
    {
        result += node->getInnerText();
    }
    vector<XMLNode *> children = node->getChildren();

    for (XMLNode *child : children)
    {
        result += minify(child);
    }

    result += "</" + node->getTagName() + ">";
    return result;
}

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

string readFile(const string &filename)
{
    ifstream file(filename);
    if (!file)
    {
        cout << "Error: Could not open file " << filename << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf(); // ���� �� ����� ����� �� �� buffer
    return buffer.str();    // ���� �� string
}

void writeFile(const string &filename, const string &content)
{
    ofstream out(filename);
    if (!out)
    {
        cout << "Error: Could not write to file " << filename << endl;
        return;
    }
    out << content;
    out.close();
}

string validate(string xmlText, bool autoFix)
{
    vector<TagInfo> st;    
    vector<string> errors; 
    string fixed;
    fixed.reserve(xmlText.size() + 256); // Pre-allocate memory

    int line = 1;
    int n = (int)xmlText.size();

    for (int i = 0; i < n; ++i)
    {
        char c = xmlText[i];

        // ---------------------------------------------------------
        // A. Handle Non-Tag Content (Text)
        // ---------------------------------------------------------
        if (c != '<')
        {
            fixed.push_back(c);
            if (c == '\n') line++;
            
            // LOGIC: If we see actual text (not just spaces), mark the current tag as "Text Only"
            if (!st.empty() && !isspace((unsigned char)c)) {
                st.back().hasTextContent = true;
            }
            continue;
        }

        int tagLine = line; 

        // ---------------------------------------------------------
        // B. Handle Special XML Blocks (Comments / CDATA)
        // ---------------------------------------------------------
        // Skip Comments 
        // if (i + 3 < n && xmlText.substr(i, 4) == "", i + 4){
        //     size_t endPos = xmlText.find("]]>", i + 4);
        //     if (endPos == string::npos) { /* Handle Error if needed */ break; }
        //     string chunk = xmlText.substr(i, (endPos - i) + 3);
        //     fixed += chunk;
        //     line += countNewLines(chunk);
        //     i = (int)(endPos + 2);
        //     continue;
        // }
        // Skip CDATA <![CDATA[ ... ]]>
        if (i + 8 < n && xmlText.substr(i, 9) == "<![CDATA[") {
            size_t endPos = xmlText.find("]]>", i + 9);
            if (endPos == string::npos) { /* Handle Error if needed */ break; }
            string chunk = xmlText.substr(i, (endPos - i) + 3);
            fixed += chunk;
            line += countNewLines(chunk);
            i = (int)(endPos + 2);
            continue;
        }

        // ---------------------------------------------------------
        // C. Parse the Tag
        // ---------------------------------------------------------
        int j = findTagEndRespectQuotes(xmlText, i + 1);
        if (j == -1) { /* Fatal Error */ break; }

        string wholeTag = xmlText.substr(i, j - i + 1);
        string inner = xmlText.substr(i + 1, j - i - 1);
        string tag = trim_copy(inner);

        // Ignore Declarations (<?xml ...?>)
        if (!tag.empty() && (tag[0] == '?' || tag[0] == '!')) {
            fixed += wholeTag;
            line += countNewLines(wholeTag);
            i = j;
            continue;
        }

        // Determine Tag Type
        bool isClosing = false;
        if (!tag.empty() && tag[0] == '/') {
            isClosing = true;
            tag.erase(tag.begin());
            ltrim(tag);
        }

        bool selfClosing = false;
        if (!isClosing) {
            string tmp = trim_copy(tag);
            if (!tmp.empty() && tmp.back() == '/') {
                selfClosing = true;
                tmp.pop_back();
                rtrim(tmp);
                tag = tmp;
            }
        }

        // Clean Tag Name
        string tagName;
        for (char ch : tag) {
            if (isspace((unsigned char)ch) || ch == '/') break;
            tagName.push_back(ch);
        }
        if (tagName.empty()) { i = j; continue; }

        // ---------------------------------------------------------
        // D. Stack Validation Logic
        // ---------------------------------------------------------

        if (!isClosing) 
        {
            // [OPENING TAG]
            
            // *** GREEDY IMPLICIT CLOSE ***
            // If the current parent has text (e.g. <id>1), and we see a new tag <name>,
            // The user definitely forgot to close <id>. Close it NOW.
            if (!st.empty() && st.back().hasTextContent) {
                errors.push_back("Line " + to_string(tagLine) + ": Implicitly closing <" + st.back().name + "> due to mixed content.");
                if (autoFix) fixed += "</" + st.back().name + ">";
                st.pop_back();
            }

            if (autoFix && !st.empty() && st.back().name == "follower" && tagName != "id") {
                errors.push_back("Line " + to_string(tagLine) + ": Invalid tag <" + tagName + "> inside <follower>. Correcting to <id>.");
                tagName = "id";         // Switch logical name
                wholeTag = "<id>";      // Overwrite the actual text string
            }

            fixed += wholeTag;
            
            if (!selfClosing) {
                TagInfo info;
                info.name = tagName;
                info.line = tagLine;
                info.hasTextContent = false; // Reset for new tag
                st.push_back(info);
            }
        } 
        else 
        {
            // [CLOSING TAG]
            
            bool matchTop = (!st.empty() && st.back().name == tagName);
            bool foundDeep = false;
            
            
            if (!matchTop && !st.empty()) {
                // Check if this tag exists deeper in the stack
                for (int k = (int)st.size() - 1; k >= 0; --k) {
                    if (st[k].name == tagName) { foundDeep = true; break; }
                }
            }

            if (matchTop) {
                // 1. Perfect Match
                fixed += wholeTag;
                st.pop_back();
            } 
            else if (foundDeep) {
                // 2. Missing Intermediate Tags (e.g. Missing </followers>)
                // We found the tag deeper, so we must close everything above it.
                while (!st.empty() && st.back().name != tagName) {
                    errors.push_back("Line " + to_string(tagLine) + ": Missing closing tag </" + st.back().name + ">.");
                    if (autoFix) fixed += "</" + st.back().name + ">";
                    st.pop_back();
                }
                fixed += wholeTag;
                st.pop_back();
            } 
            else {
                // 3. Stray Tag (e.g. <name>...</id>)
                // This tag is not in the stack at all. It is a typo or garbage.
                // We IGNORE it (do not add wholeTag to fixed).
                errors.push_back("Line " + to_string(tagLine) + ": Mismatched/Stray closing tag </" + tagName + ">.");
                // Implicit delete by NOT doing `fixed += wholeTag`
            }
        }

        line += countNewLines(wholeTag);
        i = j;
    }

    // ---------------------------------------------------------
    // E. Final Cleanup (Close remaining open tags)
    // ---------------------------------------------------------
    while (!st.empty()) {
        errors.push_back("Line " + to_string(st.back().line) + ": Unclosed tag <" + st.back().name + "> at end of file.");
        if (autoFix) fixed += "</" + st.back().name + ">";
        st.pop_back();
    }

    // Report
    if (errors.empty()) cout << "XML is VALID" << endl;
    else {
        cout << "XML is INVALID. Found " << errors.size() << " error(s)." << endl;
        for(const auto& err : errors) cout << " - " << err << endl;
    }

    return autoFix ? fixed : xmlText;
}



/*************** Compress / decompress functions *******************/

void buildDictionary(const string &content, vector<Pair> &dictionary)
{
    size_t pos = 0;
    int tokenIndex = 1;
    while ((pos = content.find('<', pos)) != string::npos)
    {
        size_t end = content.find('>', pos);
        if (end != string::npos)
        {
            string tag = content.substr(pos, end - pos + 1);
            bool exists = false;
            for (const auto &entry : dictionary)
            {
                if (entry.pattern == tag)
                {
                    exists = true;
                    break;
                }
            }
            if (!exists)
                dictionary.push_back({"T" + to_string(tokenIndex++), tag});
            pos = end + 1;
        }
        else
            break;
    }
}

string replacePatternsWithTokens(const string &content, const vector<Pair> &dictionary)
{
    string compressedContent = content;
    for (const auto &entry : dictionary)
    {
        size_t pos = 0;
        while ((pos = compressedContent.find(entry.pattern, pos)) != string::npos)
        {
            compressedContent.replace(pos, entry.pattern.length(), entry.token);
            pos += entry.token.length();
        }
    }
    return compressedContent;
}

void compressFile(const string &inputFile, const string &outputFile)
{
    string content = readFile(inputFile);
    if (content.empty())
        return;

    vector<Pair> dictionary;
    buildDictionary(content, dictionary);
    string compressedContent = replacePatternsWithTokens(content, dictionary);

    ofstream output(outputFile);
    if (!output)
    {
        cout << "Error writing file.\n";
        return;
    }

    // Write dictionary
    for (const auto &entry : dictionary)
    {
        output << entry.token << "->" << entry.pattern << "\n";
    }
    output << "---\n"; // Separator
    output << compressedContent;
    output.close();
    cout << "Compression completed.\n";
}

void parseDictionary(ifstream &input, vector<Pair> &dictionary)
{
    string line;
    while (getline(input, line))
    {
        if (line == "---")
            break;
        size_t arrowPos = line.find("->");
        if (arrowPos != string::npos)
        {
            dictionary.push_back({line.substr(0, arrowPos), line.substr(arrowPos + 2)});
        }
    }
}
// edited algorithm
string replaceTokensWithPatterns(const string &content, vector<Pair> &dictionary)
{
    string decompressed = content;

    // SORT logic: We want to process "T10" before "T1"
    sort(dictionary.begin(), dictionary.end(), [](const Pair &a, const Pair &b)
         { return a.token.length() > b.token.length(); });

    for (const auto &entry : dictionary)
    {
        size_t pos = 0;
        while ((pos = decompressed.find(entry.token, pos)) != string::npos)
        {
            decompressed.replace(pos, entry.token.length(), entry.pattern);
            pos += entry.pattern.length();
        }
    }
    return decompressed;
}

void decompressFile(const string &inputFile, const string &outputFile)
{
    ifstream input(inputFile);
    if (!input)
    {
        cout << "Error opening input file.\n";
        return;
    }

    vector<Pair> dictionary;
    parseDictionary(input, dictionary);

    string line, compressedContent;
    while (getline(input, line))
        compressedContent += line + "\n";

    // Remove last newline added by loop if necessary, but keep simplistic for now
    string decompressed = replaceTokensWithPatterns(compressedContent, dictionary);

    writeFile(outputFile, decompressed);
    cout << "Decompression completed.\n";
}

void parse_commands(int argc, char *argv[]) {
    cout<<"lgrd;mlgk,";
}





