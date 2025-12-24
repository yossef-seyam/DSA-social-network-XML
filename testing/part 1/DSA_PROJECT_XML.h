

#include <string>
#include <vector>

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

void writeFile(const string &filename, const string &content);
string readFile(const string &filename);

// Compression Functions Declarations
void buildDictionary(const string &content, vector<Pair> &dictionary);
string replacePatternsWithTokens(const string &content, const vector<Pair> &dictionary);
void compressFile(const string &inputFile, const string &outputFile);
void parseDictionary(ifstream &input, vector<Pair> &dictionary);
string replaceTokensWithPatterns(const string &content, const vector<Pair> &dictionary);
void decompressFile(const string &inputFile, const string &outputFile);

#endif