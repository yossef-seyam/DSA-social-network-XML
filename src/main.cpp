#include <iostream>
#include <string>
#include <vector>
#include "../inc/XML.h"
#include "../inc/XMLNode.h"
#include "../inc/XMLTREE.h"
using namespace std;

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        cout << "Error: No command provided.\n";
        cout << "Usage: xml_editor verify -i <inputfile> [-f] [-o <outputfile>]\n";
        return 0;
    }

    string command = argv[1];
    string inputFile = "";
    string outputFile = "";
    string idArg = "";
    string idsArg = "";
    string wordArg = "";
    string topicArg = "";
    bool autoFix = false;

    // 2. Parse Flags
    for (int i = 2; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "-i" && i + 1 < argc)
            inputFile = argv[++i];
        else if (arg == "-o" && i + 1 < argc)
            outputFile = argv[++i];
        else if (arg == "-f")
            autoFix = true;
        else if (arg == "-id" && i + 1 < argc)
            idArg = argv[++i];
        else if (arg == "-ids" && i + 1 < argc)
            idsArg = argv[++i];
        else if (arg == "-w" && i + 1 < argc)
            wordArg = argv[++i];
        else if (arg == "-t" && i + 1 < argc)
            topicArg = argv[++i];
    }

    if (inputFile.empty())
    {
        cout << "Error: No input file provided (-i)." << endl;
        return 0;
    }

    if (command == "verify")
    {
        string xml = readFile(inputFile);
        if (xml.empty())
        {
            cout << "Error: File could not be read or is empty.\n";
            return 0;
        }
        string result = validate(xml, autoFix);
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

    else if (command == "compress")
    {
        if (outputFile.empty())
            outputFile = "compressed.out";
        compressFile(inputFile, outputFile);
    }

    else if (command == "decompress")
    {
        if (outputFile.empty())
            outputFile = "decompressed.xml";
        decompressFile(inputFile, outputFile);
    }
    // TO DO
    else
    {
        string xml = readFile(inputFile);
        if (xml.empty())
            return 0;

        XMLTREE *tree = new XMLTREE();
        tree->parseXML(xml);

        if (!tree->getRoot())
        {
            cout << "Error: Failed to parse XML structure." << endl;
            delete tree;
            return 0;
        }

        if (command == "format")
        {

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
            if (!tree->getRoot())
            {
                cout << "Error parsing XML.\n";
                return 0;
            }

            // 3. Prettify Tree (Using Team B)
            string result = prettify(tree->getRoot(), 0);

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

            string xml = readFile(inputFile);
            if (xml.empty())
                return 0;

            // Minify logic
            string result = minify(tree->getRoot());

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

            string xml = readFile(inputFile);
            if (xml.empty())
                return 0;

            // JSON logic
            // We manually wrap the result in { } to make it a valid JSON object
            string result = "{\n" + xmlToJSON(tree->getRoot(), 1) + "\n}";

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
        // --- PART 2: GRAPH COMMANDS ---
        else if (command == "most_active")
        {
            tree->solveMostActive();
        }
        else if (command == "most_influencer")
        {
            tree->solveMostInfluencer();
        }
        else if (command == "mutual")
        {
            vector<string> ids;
            string num;
            for (char c : idsArg)
            {
                if (c == ',')
                {
                    if (!num.empty())
                        ids.push_back(num);
                    num = "";
                }
                else
                    num += c;
            }
            if (!num.empty())
                ids.push_back(num);
            tree->solveMutual(ids);
        }
        else if (command == "suggest")
        {
            if (idArg.empty())
                cout << "Error: Provide -id <user_id>" << endl;
            else
                tree->solveSuggest(idArg);
        }
        else if (command == "search")
        {
            if (!wordArg.empty())
                tree->searchByWord(xml, wordArg);
            else if (!topicArg.empty())
                tree->searchByTopic(xml, topicArg);
            else
                cout << "Error: Provide -w <word> or -t <topic>" << endl;
        }
        else if (command == "draw")
        {
            // Generates CSV for Python
            string pngFile = outputFile.empty() ? "social_network.png" : outputFile;
            string csvFile = "graph.csv";
            
            tree->saveGraphCSV(csvFile); // draw with python
            tree->saveGraphPNG(pngFile); // draw with c++
        }
        else
        {
            cout << "Unknown command: " << command << endl;
        }
        delete tree;
    }
    return 0;
}
