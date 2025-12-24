#include "Utils.h"
#include <cctype>

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
    if (first == string::npos)
        return "";

    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}