#include <string>
#include <vector>
#include "XMLNode.h"
#include "XMLTREE.h"
using namespace std;
#ifndef DSA_PROJECT_XML_H
#define DSA_PROJECT_XML_H

struct Pair
{
    string token;
    string pattern;
};
struct TagInfo {
    string name;
    int line;
    bool hasTextContent; 
};

// ================== DECLARATIONS of functions=========//
//helpers
string readFile(const string &filename);
string validate(string xmlText, bool autoFix);
void writeFile(const string &filename, const string &content);
string prettify(XMLNode *node, int depth);
string minify(XMLNode *node);
string xmlToJSON(XMLNode *node, int depth = 0, bool isArrayElement = false);
bool isAllWhitespace(const string &s);
string trim(const string &str);
string getDirectory(const string& filepath);
string combinePaths(const string& dir, const string& filename);
bool isAbsolutePath(const string& path);
string getFilename(const string& path);



// Compression Functions Declarations
void buildDictionary(const string &content, vector<Pair> &dictionary);
string replacePatternsWithTokens(const string &content, const vector<Pair> &dictionary);
void compressFile(const string &inputFile, const string &outputFile);
void parseDictionary(ifstream &input, vector<Pair> &dictionary);
string replaceTokensWithPatterns(const string &content, const vector<Pair> &dictionary); // where is the implementation??
void decompressFile(const string &inputFile, const string &outputFile);


void parse_commands(int argc, char *argv[]); // where is the implementation??




#endif //DSA_PROJECT_XML_H