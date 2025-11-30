#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include <vector> // ��� ��� vector
#include <cctype> // ��� ��� isspace
#include "XMLNode.h"
using namespace std;

// we will use it in validate function //
struct TagInfo
{
    string name;
    int line;
};
// ================== DECLARATIONS of functions=========//

string readFile(const string &filename);
string validate(string xmlText, bool autoFix);
void writeFile(const string &filename, const string &content);
string prettify(XMLNode *node, int depth);
string minify(XMLNode *node);
string xmlToJSON(XMLNode *node, int depth, bool isArrayElement);
bool isAllWhitespace(const string &s);
string trim(const string &str);

/*
 *   Team A functions
 *
 *   String -> XMLNode
 *
 *
 */
// --- Helper for parsing to ignore whitespace ---
bool isAllWhitespace(const string &s)
{
    for (char c : s)
    {
        if (!isspace((unsigned char)c))
            return false;
    }
    return true;
}

// Helper: Removes leading and trailing whitespace/newlines
string trim(const string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

XMLNode *parseXML(string xmlText)
{
    int i = 0;
    stack<XMLNode *> Stack;
    XMLNode *head = nullptr; // Initialize to nullptr
    while (i < xmlText.length())
    {
        // Case 1: Closing Tag </...>
        if (xmlText[i] == '<' && i + 1 < xmlText.length() && xmlText[i + 1] == '/')
        {
            if (!Stack.empty())
                Stack.pop(); // Safety check
            while (i < xmlText.length() && xmlText[i] != '>')
            {
                i++;
            }
            i++;
            continue;
        }

        // Case 2: Opening Tag <...>
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

            // NOTE: Team A hardcoded "users" as root.
            // Better logic: if Stack is empty, this is the root.
            if (Stack.empty())
            {
                XMLNode *head_node = new XMLNode(name, nullptr);
                head = head_node;
                Stack.push(head);
                continue;
            }
            else
            {
                XMLNode *node = new XMLNode(name, Stack.top());
                Stack.top()->children.push_back(node);
                Stack.push(node);
            }
            continue;
        }

        // Case 3: Inner Text (Content between tags)
        string textContent;
        while (i < xmlText.length() && xmlText[i] != '<')
        {
            textContent += xmlText[i];
            i++;
        }

        //        // CHANGE: Only add text if stack is not empty AND text is not just empty space
        //        if (!Stack.empty() && !isAllWhitespace(textContent)) {
        //             Stack.top()->innerText = textContent;
        //        }
        /********** EDIT **************/
        if (!Stack.empty())
        {
            string cleanText = trim(textContent);
            if (!cleanText.empty())
            {
                Stack.top()->innerText = cleanText;
            }
        }
    }
    return head;
}

/*
 *   Team B functions
 *
 *   Prettifying
 *   Minifying
 *   XMLNode -> JSON
 *
 */

string prettify(XMLNode *node, int depth)
{

    if (!node)
        return ""; // Safety check

    string indent(depth * 4, ' ');
    string result = indent + "<" + node->tagName + ">\n";

    if (!node->innerText.empty())
    {
        result += indent + "\t" + node->innerText + "\n";
    }

    for (XMLNode *child : node->children)
    {
        result += prettify(child, depth + 1); // recursion for each child(node)
    }

    result += indent + "</" + node->tagName + ">\n";
    return result;
}

string minify(XMLNode *node)
{

    if (!node)
        return ""; // Safety check

    string result = "<" + node->tagName + ">";

    if (!node->innerText.empty())
    {
        result += node->innerText;
    }

    for (XMLNode *child : node->children)
    {
        result += minify(child);
    }

    result += "</" + node->tagName + ">";
    return result;
}

string xmlToJSON(XMLNode *node, int depth = 0, bool isArrayElement = false)
{

    if (!node)
        return ""; // Safety check

    string indent(depth * 4, ' ');
    string json = "";

    // Node without children
    if (node->children.empty())
    {
        return "\"" + node->tagName + "\": \"" + node->innerText + "\""; //"tag name": "text"
    }

    // Array ( if children has same tag )
    bool allSame = node->children.size() > 1; //??
    string tag = allSame ? node->children[0]->tagName : "";

    for (int i = 1; i < node->children.size() && allSame; i++)
    {
        if (node->children[i]->tagName != tag)
        {
            allSame = false;
            break;
        }
    }

    // Array Case
    if (allSame)
    {
        if (!isArrayElement)
            json += "\"" + node->tagName + "\": [\n";
        else
            json += "[\n";

        for (int i = 0; i < node->children.size(); i++)
        {
            ////            json += indent + "    {\n";
            json += indent + "\t" + xmlToJSON(node->children[i], depth + 2, true); // recursion
            ////            json += "\n" + indent + "    }";
            if (i < node->children.size() - 1)
                json += ",";
            json += "\n";
        }
        json += indent + "]";
        return json;
    }

    // Object Case
    if (!isArrayElement)
        json += "\"" + node->tagName + "\": {\n";
    else
        json += "{\n";

    for (int i = 0; i < node->children.size(); i++)
    {
        XMLNode *child = node->children[i];
        json += indent + "  ";
        json += xmlToJSON(child, depth + 1, false); // recursion
        if (i < node->children.size() - 1)
            json += ",";
        json += "\n";
    }
    json += indent + "}";
    return json;
}

/*
 *   Team C functions
 *
 *   Validation
 *   Main program
 *
 */

// ================== IMPLEMENTATIONS of functions ==================

// ����� �����
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

// Helper: ����� �� ���
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

// ================== validate ==================

string validate(string xmlText, bool autoFix)
{
    vector<TagInfo> st;    // ��stack
    vector<string> errors; // ����� �������
    string fixed;          // ��� ����� ������ �������� �� autoFix = true
    fixed.reserve(xmlText.size() + 100);

    int line = 1;
    int n = (int)xmlText.size();

    for (int i = 0; i < n; ++i)
    {
        char c = xmlText[i];

        if (c == '\n')
        {
            line++;
            fixed.push_back(c); // ����� ��� ��newlines �� ������ ��������
            continue;
        }

        if (c == '<')
        {
            int j = xmlText.find('>', i + 1);
            if (j == string::npos)
            {
                errors.push_back("Line " + to_string(line) +
                                 ": missing '>' for a tag.");
                if (autoFix)
                {
                    fixed += xmlText.substr(i); // ���� ������ �� �� ��
                }
                break;
            }

            // ����� ����� ������ (��� ���)
            string originalTagText = xmlText.substr(i + 1, j - i - 1);
            string tag = originalTagText;

            // ����� ��������
            while (!tag.empty() && isspace((unsigned char)tag.front()))
                tag.erase(tag.begin());
            while (!tag.empty() && isspace((unsigned char)tag.back()))
                tag.pop_back();

            // ����� declaration / comments
            if (!tag.empty() && (tag[0] == '?' || tag[0] == '!'))
            {
                fixed += "<" + originalTagText + ">";
                i = j;
                continue;
            }

            bool isClosing = false;
            if (!tag.empty() && tag[0] == '/')
            {
                isClosing = true;
                tag = tag.substr(1);
            }

            // ���� attributes �� �����
            size_t spacePos = tag.find(' ');
            if (spacePos != string::npos)
            {
                tag = tag.substr(0, spacePos);
            }

            if (tag.empty())
            {
                fixed += "<" + originalTagText + ">";
                i = j;
                continue;
            }

            if (!isClosing)
            {
                // opening tag
                st.push_back({tag, line});
                fixed += "<" + originalTagText + ">";
            }
            else
            {
                // closing tag
                if (st.empty())
                {
                    errors.push_back("Line " + to_string(line) +
                                     ": closing tag </" + tag +
                                     "> without matching opening tag.");
                    if (!autoFix)
                    {
                        fixed += "</" + tag + ">";
                    }
                }
                else if (st.back().name != tag)
                {
                    errors.push_back("Line " + to_string(line) +
                                     ": expected </" + st.back().name +
                                     "> but found </" + tag + ">.");

                    if (autoFix)
                    {
                        fixed += "</" + st.back().name + ">";
                    }
                    else
                    {
                        fixed += "</" + tag + ">";
                    }

                    st.pop_back();
                }
                else
                {
                    fixed += "</" + tag + ">";
                    st.pop_back();
                }
            }

            i = j; // ���� ��� ��'>
        }
        else
        {
            // �� ��� ���: �� ����
            fixed.push_back(c);
        }
    }

    // �� ��� ��� ����� ������ �� ��stack
    while (!st.empty())
    {
        errors.push_back("Line " + to_string(st.back().line) +
                         ": opening tag <" + st.back().name +
                         "> not closed.");
        if (autoFix)
        {
            fixed += "</" + st.back().name + ">";
        }
        st.pop_back();
    }

    // ���� �������
    if (errors.empty())
    {
        cout << "XML is VALID " << endl;
    }
    else
    {
        cout << "XML is INVALID " << endl;
        cout << "Found " << errors.size() << " error(s):" << endl;
        for (const string &e : errors)
        {
            cout << " - " << e << endl;
        }
    }

    // �� �� ���� autoFix� ���� ���� ������
    if (!autoFix)
    {
        return xmlText;
    }
    else
    {
        return fixed;
    }
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        cout << "Error: No command provided.\n";
        cout << "Usage: xml_editor verify -i <inputfile> [-f] [-o <outputfile>]\n";
        return 0;
    }

    string command = argv[1];

    // ������ ���� ������� ��� ���� ��: verify
    if (command == "verify")
    {

        string inputFile = "";
        string outputFile = "";
        bool autoFix = false;

        // ���� ���� ��arguments: -i , -f , -o
        for (int i = 2; i < argc; ++i)
        {
            string arg = argv[i];

            if (arg == "-i" && i + 1 < argc)
            {
                inputFile = argv[++i]; // ���� ��� -i �� ��� �����
            }
            else if (arg == "-f")
            {
                autoFix = true; // ����� �� autofix
            }
            else if (arg == "-o" && i + 1 < argc)
            {
                outputFile = argv[++i]; // ���� ��� -o �� ��� ��� ��output
            }
        }

        // ���� ���� ��� input file
        if (inputFile.empty())
        {
            cout << "Error: No input file provided.\n";
            cout << "Usage: xml_editor verify -i <inputfile> [-f] [-o <outputfile>]\n";
            return 0;
        }

        // ���� ����� ��� �� XML ��� �� string
        string xml = readFile(inputFile);
        if (xml.empty())
        {
            cout << "Error: File could not be read or is empty.\n";
            return 0;
        }

        // ����� validate (�������� ���� ��� ������)
        string result = validate(xml, autoFix);

        // �� �� autoFix ����
        if (autoFix)
        {
            if (outputFile.empty())
            {
                cout << "\nWarning: Auto-fix is ON but no -o <outputfile> specified.\n";
                cout << "Fixed XML will NOT be saved to a file.\n";
            }
            else
            {
                writeFile(outputFile, result);
                cout << "\nFixed XML saved to: " << outputFile << endl;
            }
        }
    }
    else if (command == "format")
    {
        string inputFile = "";
        string outputFile = "";

        for (int i = 2; i < argc; ++i)
        {
            string arg = argv[i];

            if (arg == "-i" && i + 1 < argc)
            {
                inputFile = argv[++i]; // ���� ��� -i �� ��� �����
            }
            else if (arg == "-o" && i + 1 < argc)
            {
                outputFile = argv[++i]; // ���� ��� -o �� ��� ��� ��output
            }
        }

        if (inputFile.empty())
        {
            cout << "Error: No input file provided.\n";
            return 0;
        }

        // 1. Read String
        string xml = readFile(inputFile);
        if (xml.empty())
            return 0;

        // 2. Parse String to Tree (Using Team A)
        // Check for basic validation first (Optional, but good practice)
        if (xml.find("INVALID") != string::npos)
        {
            cout << "Cannot format invalid XML. Run verify first.\n";
            return 0;
        }

        XMLNode *root = parseXML(xml);
        if (!root)
        {
            cout << "Error parsing XML.\n";
            return 0;
        }

        // 3. Prettify Tree (Using Team B)
        string result = prettify(root, 0);

        // 4. Save
        if (!outputFile.empty())
        {
            writeFile(outputFile, result);
            cout << "Formatted XML saved to " << outputFile << endl;
        }
        else
        {
            cout << result << endl; // Print to console if no output file
        }
    }
    else if (command == "mini")
    {
        string inputFile = "";
        string outputFile = "";

        for (int i = 2; i < argc; ++i)
        {
            string arg = argv[i];
            if (arg == "-i" && i + 1 < argc)
                inputFile = argv[++i];
            else if (arg == "-o" && i + 1 < argc)
                outputFile = argv[++i];
        }

        if (inputFile.empty())
        {
            cout << "Error: No input file provided.\n";
            return 0;
        }

        string xml = readFile(inputFile);
        if (xml.empty())
            return 0;

        XMLNode *root = parseXML(xml);

        // Minify logic
        string result = minify(root);

        if (!outputFile.empty())
        {
            writeFile(outputFile, result);
            cout << "Minified XML saved to " << outputFile << endl;
        }
        else
        {
            cout << result << endl;
        }
    }
    else if (command == "json")
    {
        string inputFile = "";
        string outputFile = "";

        for (int i = 2; i < argc; ++i)
        {
            string arg = argv[i];
            if (arg == "-i" && i + 1 < argc)
                inputFile = argv[++i];
            else if (arg == "-o" && i + 1 < argc)
                outputFile = argv[++i];
        }

        if (inputFile.empty())
        {
            cout << "Error: No input file provided.\n";
            return 0;
        }

        string xml = readFile(inputFile);
        if (xml.empty())
            return 0;

        XMLNode *root = parseXML(xml);

        // JSON logic
        // We manually wrap the result in { } to make it a valid JSON object
        string result = "{\n" + xmlToJSON(root, 1) + "\n}";

        if (!outputFile.empty())
        {
            writeFile(outputFile, result);
            cout << "JSON saved to " << outputFile << endl;
        }
        else
        {
            cout << result << endl;
        }
    }

    // TO DO
    else if (command == "compress")
    {
    }
    else if (command == "decompress")
    {
    }
    else if (command == "most_active")
    {
    }
    else if (command == "most_influencer")
    {
    }
    else if (command == "mutual")
    {
    }
    else if (command == "suggest")
    {
    }
    else if (command == "search")
    {
        // search with word

        // search with topic
    }

    else
    {
        cout << "Error: Unknown command: " << command << endl;
        cout << "Available commands: verify\n";
    }

    return 0;
}
