#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../inc/XMLTREE.h"
#include "../inc/XMLNode.h"
#include <stack>
#include <map>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../inc/stb_image_write.h"
#define STB_EASY_FONT_IMPLEMENTATION
#include "../inc/stb_easy_font.h"
#include "../inc/XML.h"
#include <cmath> // For sin/cos
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

XMLTREE::XMLTREE()
{
    this->root = nullptr;
}

XMLTREE::~XMLTREE()
{
    if (root)
        delete root;
}

// ================= PART 1: HELPERS & PARSING =================

void XMLTREE::parseXML(string xmlText)
{
    int i = 0;
    stack<XMLNode *> Stack;
    XMLNode *head = nullptr;

    while (i < xmlText.length())
    {
        // Case 1: Closing Tag
        if (xmlText[i] == '<' && i + 1 < xmlText.length() && xmlText[i + 1] == '/')
        {
            if (!Stack.empty())
                Stack.pop();
            while (i < xmlText.length() && xmlText[i] != '>')
                i++;
            i++;
            continue;
        }

        // Case 2: Opening Tag
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

        // Case 3: Inner Text
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
    cout << "Finished parsing. Root: " << (root ? root->getTagName() : "nullptr") << endl;
}

XMLNode *XMLTREE::getRoot()
{
    return root;
}

// ================= PART 2: GRAPH BUILDING =================

// Helper: Find Index by ID String
int XMLTREE::getNodeIndexByID(string id)
{
    if (!root)
        return -1;
    vector<XMLNode *> users = root->getChildren();
    for (int i = 0; i < users.size(); i++)
    {
        for (XMLNode *c : users[i]->getChildren())
        {
            if (c->getTagName() == "id" && trim(c->getInnerText()) == id)
                return i;
        }
    }
    return -1;
}
vector<vector<XMLNode *>> XMLTREE::buildAdjacencyList()
{
    return buildFollowersList(); // For compatibility with main
}

// Team A's Logic: Who follows me?
vector<vector<XMLNode *>> XMLTREE::buildFollowersList()
{
    vector<vector<XMLNode *>> adjList;
    if (!root)
        return adjList;
    adjList.resize(root->getChildren().size());

    int idx = 0;
    for (XMLNode *each_user : root->getChildren())
    {
        for (XMLNode *c : each_user->getChildren())
        {
            if (c->getTagName() == "followers")
            {
                for (XMLNode *f : c->getChildren())
                {
                    // f is <follower>. We store it as an OUTGOING edge.
                    adjList[idx].push_back(f);
                }
                break;
            }
        }
        idx++;
    }
    return adjList;
}

// Team B's Requirement: Who do I follow? (Reverse of Adjacency List)
vector<vector<int>> XMLTREE::buildFollowingList()
{
    vector<vector<int>> incomingList;
    if (!root)
        return incomingList;
    int numUsers = root->getChildren().size();
    incomingList.resize(numUsers);

    // Get the Outgoing graph
    vector<vector<XMLNode *>> outgoingGraph = buildFollowersList();

    for (int sourceIdx = 0; sourceIdx < numUsers; sourceIdx++)
    {
        for (XMLNode *targetNode : outgoingGraph[sourceIdx])
        {
            // sourceIdx follows targetNode
            string targetIDString = "";
            for (XMLNode *c : targetNode->getChildren())
            {
                if (c->getTagName() == "id")
                    targetIDString = trim(c->getInnerText());
            }

            int targetIdx = getNodeIndexByID(targetIDString);
            if (targetIdx != -1)
            {
                // Add 'sourceIdx' to 'targetIdx's incoming list
                incomingList[targetIdx].push_back(sourceIdx);
            }
        }
    }
    return incomingList;
}

// ================= PART 3: ANALYSIS & COMMANDS =================

void XMLTREE::print_graph(vector<vector<XMLNode *>> adjList)
{
    cout << "graph begins ------------------" << endl;
    if (!root)
        return;

    for (int i = 0; i < adjList.size(); i++)
    {
        if (i >= root->getChildren().size())
            continue;

        string userName = "Unknown";
        for (XMLNode *c : root->getChildren()[i]->getChildren())
        {
            if (c->getTagName() == "name")
            {
                userName = c->getInnerText();
                break;
            }
        }

        // CORRECTED TEXT: Based on XML, this list is who they FOLLOW.
        cout << userName << " follows: ";
        for (XMLNode *f : adjList[i])
        {
            for (XMLNode *c : f->getChildren())
                if (c->getTagName() == "id")
                    cout << c->getInnerText() << " ";
        }
        cout << endl;
    }
    cout << "graph ends------------------" << endl;
}

// MOST ACTIVE: Who has the most OUTGOING edges (Follows the most people)
void XMLTREE::solveMostActive()
{
    vector<vector<XMLNode *>> outgoing = buildFollowersList();

    int maxFollows = -1;
    int idx = -1;

    for (int i = 0; i < outgoing.size(); i++)
    {
        if ((int)outgoing[i].size() > maxFollows)
        {
            maxFollows = outgoing[i].size();
            idx = i;
        }
    }

    if (idx != -1)
    {
        string name = "Unknown";
        for (XMLNode *c : root->getChildren()[idx]->getChildren())
            if (c->getTagName() == "name")
                name = c->getInnerText();
        cout << "Most Active: " << name << " (Follows " << maxFollows << " users)" << endl;
    }
    else
    {
        cout << "No active users found." << endl;
    }
}

// MOST INFLUENCER: Who has the most INCOMING edges (Is followed by the most people)
void XMLTREE::solveMostInfluencer()
{
    vector<vector<int>> incoming = buildFollowingList();

    int maxFollowers = -1;
    int idx = -1;

    for (int i = 0; i < incoming.size(); i++)
    {
        if ((int)incoming[i].size() > maxFollowers)
        {
            maxFollowers = incoming[i].size();
            idx = i;
        }
    }

    if (idx != -1)
    {
        string name = "Unknown";
        for (XMLNode *c : root->getChildren()[idx]->getChildren())
            if (c->getTagName() == "name")
                name = c->getInnerText();
        cout << "Most Influencer: " << name << " (Has " << maxFollowers << " followers)" << endl;
    }
}

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

void XMLTREE::saveGraphCSV(string filename)
{
    ofstream out(filename);
    if (!out)
        return;

    vector<string> indexToID;
    if (root)
    {
        for (XMLNode *u : root->getChildren())
        {
            for (XMLNode *c : u->getChildren())
            {
                if (c->getTagName() == "id")
                {
                    indexToID.push_back(trim(c->getInnerText()));
                    break;
                }
            }
        }
    }

    vector<vector<XMLNode *>> outgoing = buildFollowersList();
    for (int i = 0; i < outgoing.size(); i++)
    {
        if (i >= indexToID.size())
            break;
        string sourceID = indexToID[i];

        for (XMLNode *f : outgoing[i])
        {
            string targetID = "";
            for (XMLNode *c : f->getChildren())
                if (c->getTagName() == "id")
                    targetID = trim(c->getInnerText());

            if (!targetID.empty())
                out << sourceID << "," << targetID << "\n";
        }
    }
    out.close();
    cout << "Graph CSV saved to " << filename << endl;
}

/*********************************/

// Helper: Make string lowercase
string XMLTREE::toLower(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// Helper: Extract value between tags
bool XMLTREE::extractTagValue(const string &block, const string &tag, string &out)
{
    string open = "<" + tag + ">";
    string close = "</" + tag + ">";
    size_t s = block.find(open);
    if (s == string::npos)
        return false;
    s += open.length();
    size_t e = block.find(close, s);
    if (e == string::npos)
        return false;
    out = block.substr(s, e - s);
    // Remove extra whitespace
    out.erase(out.find_last_not_of(" \n\r\t") + 1);
    return true;
}

// Helper: Extract topics list
vector<string> XMLTREE::extractTopics(const string &topicsBlock)
{
    vector<string> topics;
    size_t pos = 0;
    while (true)
    {
        size_t start = topicsBlock.find("<topic>", pos);
        if (start == string::npos)
            break;
        start += 7; // Length of "<topic>"
        size_t end = topicsBlock.find("</topic>", start);
        if (end == string::npos)
            break;
        string topic = topicsBlock.substr(start, end - start);
        // Trim
        topic.erase(0, topic.find_first_not_of(" \n\r\t"));
        topic.erase(topic.find_last_not_of(" \n\r\t") + 1);
        if (!topic.empty())
            topics.push_back(topic);
        pos = end + 8;
    }
    return topics;
}

// Command: Search by Word (Body)
void XMLTREE::searchByWord(const string &xml, const string &word)
{
    string lowerWord = toLower(word);
    size_t pos = 0;
    bool found = false;

    while (true)
    {
        size_t bodyPosStart = xml.find("<body>", pos);
        if (bodyPosStart == string::npos)
            break;

        size_t bodyPosEnd = xml.find("</body>", bodyPosStart);
        if (bodyPosEnd == string::npos)
            break;

        string body = xml.substr(bodyPosStart + 6, bodyPosEnd - (bodyPosStart + 6));
        string bodyLower = toLower(body);

        pos = bodyPosEnd + 7;

        if (bodyLower.find(lowerWord) != string::npos)
        {
            // Find enclosing <user> by looking backwards
            size_t userStart = xml.rfind("<user>", bodyPosStart);
            if (userStart != string::npos)
            {
                size_t userEnd = xml.find("</user>", userStart);
                string userBlock = xml.substr(userStart, userEnd - userStart);

                string userId = "(unknown)";
                string userName = "(unknown)";

                extractTagValue(userBlock, "id", userId);
                extractTagValue(userBlock, "name", userName);

                cout << "Found match in post body:\n";
                cout << "  User: " << userName << " (id=" << userId << ")\n";
                cout << "  Body: " << trim(body) << "\n";
                cout << "------------------------\n";
                found = true;
            }
        }
    }
    if (!found)
        cout << "No posts contain the word: " << word << endl;
}

// Command: Search by Topic
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

/***********************************/

// --- DRAWING HELPERS ---
struct RGBA
{
    unsigned char r, g, b, a;
};

void putPixel(vector<unsigned char> &img, int W, int H, int x, int y, RGBA c)
{
    if (x < 0 || x >= W || y < 0 || y >= H)
        return;
    int idx = (y * W + x) * 4;
    img[idx + 0] = c.r;
    img[idx + 1] = c.g;
    img[idx + 2] = c.b;
    img[idx + 3] = c.a;
}

void drawLine(vector<unsigned char> &img, int W, int H, int x0, int y0, int x1, int y1, RGBA c)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (true)
    {
        putPixel(img, W, H, x0, y0, c);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void drawFilledCircle(vector<unsigned char> &img, int W, int H, int cx, int cy, int r, RGBA c)
{
    for (int dy = -r; dy <= r; ++dy)
    {
        for (int dx = -r; dx <= r; ++dx)
        {
            if (dx * dx + dy * dy <= r * r)
                putPixel(img, W, H, cx + dx, cy + dy, c);
        }
    }
}

// Draws rough text using STB Easy Font
void drawText(vector<unsigned char> &img, int W, int H, int x, int y, const string &text, RGBA color)
{
    static char buffer[99999]; // Buffer for vertex data
    int num_quads = stb_easy_font_print(0, 0, (char *)text.c_str(), NULL, buffer, sizeof(buffer));

    // Convert quads to pixels (Basic Point Cloud Rasterizer)
    // float* vbuf = (float*)buffer;
    // For simplicity in this project, sticking to the friend's "Box" method is safer
    // because writing a full rasterizer for quads is too complex for this snippet.
    // We will draw a small placeholder box for names.
    int cx = x;
    for (char c : text)
    {
        if (c == ' ')
        {
            cx += 4;
            continue;
        }
        // Draw a tiny 3x5 rectangle for each letter
        for (int dy = 0; dy < 5; dy++)
            for (int dx = 0; dx < 3; dx++)
                putPixel(img, W, H, cx + dx, y + dy, {0, 0, 0, 255});
        cx += 5;
    }
}

// Helper: Draws a single digit (0-9) at (x,y)
void drawDigit(vector<unsigned char> &img, int W, int H, int x, int y, int digit, RGBA c)
{
    // Simple 3x5 pixel bitmaps for digits 0-9
    static const int bitmaps[10][5] = {
        {7, 5, 5, 5, 7}, // 0
        {2, 6, 2, 2, 7}, // 1
        {7, 1, 7, 4, 7}, // 2
        {7, 1, 7, 1, 7}, // 3
        {5, 5, 7, 1, 1}, // 4
        {7, 4, 7, 1, 7}, // 5
        {7, 4, 7, 5, 7}, // 6
        {7, 1, 1, 2, 2}, // 7
        {7, 5, 7, 5, 7}, // 8
        {7, 5, 7, 1, 7}  // 9
    };

    if (digit < 0 || digit > 9)
        return;

    for (int row = 0; row < 5; row++)
    {
        int pattern = bitmaps[digit][row]; // e.g., 7 is 111 (binary)
        for (int col = 0; col < 3; col++)
        {
            // Check bit (column 2 is bit 0, column 0 is bit 2)
            if (pattern & (1 << (2 - col)))
            {
                // Draw a 2x2 pixel block for visibility
                putPixel(img, W, H, x + col * 2, y + row * 2, c);
                putPixel(img, W, H, x + col * 2 + 1, y + row * 2, c);
                putPixel(img, W, H, x + col * 2, y + row * 2 + 1, c);
                putPixel(img, W, H, x + col * 2 + 1, y + row * 2 + 1, c);
            }
        }
    }
}

// Helper: Draws a full number (e.g., "12")
void drawNumber(vector<unsigned char> &img, int W, int H, int x, int y, int number, RGBA c)
{
    string s = to_string(number);
    int spacing = 8;
    for (char digit : s)
    {
        drawDigit(img, W, H, x, y, digit - '0', c);
        x += spacing;
    }
}

/**/
// Add to XMLTREE.cpp
void XMLTREE::saveGraphPNG(string filename)
{
    int W = 1200, H = 800;
    vector<unsigned char> img(W * H * 4, 255); // White background

    // 1. Get All Users and Assign Positions (Circular Layout)
    vector<XMLNode *> users = root->getChildren();
    int n = users.size();
    if (n == 0)
        return;

    double cx = W / 2.0, cy = H / 2.0;
    double radius = min(W, H) / 2.0 - 50;
    map<int, pair<int, int>> positions; // Index -> (x, y)

    for (int i = 0; i < n; i++)
    {
        double ang = 2.0 * M_PI * i / n;
        positions[i] = {(int)(cx + radius * cos(ang)), (int)(cy + radius * sin(ang))};
    }

    // 2. Draw Edges
    vector<vector<int>> following = buildFollowingList();
    RGBA edgeColor = {200, 200, 200, 255}; // Light Gray

    for (int i = 0; i < n; i++)
    {
        auto start = positions[i];
        for (int target : following[i])
        {
            if (positions.find(target) == positions.end())
                continue;
            auto end = positions[target];
            drawLine(img, W, H, start.first, start.second, end.first, end.second, edgeColor);
        }
    }

    // 3. Draw Nodes (numbers)
    RGBA nodeColor = {100, 149, 237, 255}; // Blue
    RGBA white = {255, 255, 255, 255};     // White text

    for (int i = 0; i < n; i++)
    {
        auto pos = positions[i];

        // 1. Draw Bigger Circle (Radius 20)
        drawFilledCircle(img, W, H, pos.first, pos.second, 20, nodeColor);

        // 2. Get the User ID (Assumption: index i maps to ID i+1 for this sample)
        // Ideally, map index back to real ID string
        string idStr = "";
        for (XMLNode *c : users[i]->getChildren())
            if (c->getTagName() == "id")
                idStr = trim(c->getInnerText());

        int idNum = 0;
        try
        {
            idNum = stoi(idStr);
        }
        catch (...)
        {
        }

        // 3. Draw Number Centered inside Circle
        // Offset (-4, -5) helps center single digits
        drawNumber(img, W, H, pos.first - 4, pos.second - 5, idNum, white);
    }

    // 4. Save
    stbi_write_png(filename.c_str(), W, H, 4, img.data(), W * 4);
    cout << "Graph PNG saved to " << filename << endl;
}
