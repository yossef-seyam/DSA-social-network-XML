/*************** Compress / decompress functions *******************/
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "DSA_PROJECT_XML.h"
#include <algorithm>
 
using namespace std;

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