#include <bits/stdc++.h>
using namespace std;

// ----------------- Helpers used by validate (same order as your code) -----------------

static inline void ltrim(string &s) {
    size_t p = 0;
    while (p < s.size() && isspace((unsigned char)s[p])) p++;
    s.erase(0, p);
}

static inline void rtrim(string &s) {
    while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
}

static inline string trim_copy(string s) {
    ltrim(s);
    rtrim(s);
    return s;
}

static inline int countNewLines(const string &s) {
    return (int)count(s.begin(), s.end(), '\n');
}

// find '>' for a tag, but ignore '>' that are inside quotes
static int findTagEndRespectQuotes(const string &xml, int startIdx) {
    bool inQuote = false;
    char q = 0;
    int n = (int)xml.size();

    for (int k = startIdx; k < n; ++k) {
        char ch = xml[k];
        if (!inQuote && (ch == '"' || ch == '\'')) {
            inQuote = true;
            q = ch;
        } else if (inQuote && ch == q) {
            inQuote = false;
        } else if (!inQuote && ch == '>') {
            return k;
        }
    }
    return -1;
}

// ----------------- Needed by validate -----------------
// If you already have TagInfo in XML.h, keep yours and remove this.
struct TagInfo {
    string name;
    int line;
    bool hasTextContent;
};

// ----------------- validate (as in your code) -----------------

string validate(string xmlText, bool autoFix) {
    vector<TagInfo> st;
    vector<string> errors;

    string fixed;
    fixed.reserve(xmlText.size() + 256); // Pre-allocate memory

    int line = 1;
    int n = (int)xmlText.size();

    for (int i = 0; i < n; ++i) {
        char c = xmlText[i];

        // ---------------------------------------------------------
        // A. Handle Non-Tag Content (Text)
        // ---------------------------------------------------------
        if (c != '<') {
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

        // Ignore Declarations (<?xml ...?>) and other <! ... >
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

        if (!isClosing) {
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
                tagName = "id";     // Switch logical name
                wholeTag = "<id>";  // Overwrite the actual text string
            }

            fixed += wholeTag;

            if (!selfClosing) {
                TagInfo info;
                info.name = tagName;
                info.line = tagLine;
                info.hasTextContent = false; // Reset for new tag
                st.push_back(info);
            }
        } else {
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
            } else if (foundDeep) {
                // 2. Missing Intermediate Tags (e.g. Missing </followers>)
                // We found the tag deeper, so we must close everything above it.
                while (!st.empty() && st.back().name != tagName) {
                    errors.push_back("Line " + to_string(tagLine) + ": Missing closing tag </" + st.back().name + ">.");
                    if (autoFix) fixed += "</" + st.back().name + ">";
                    st.pop_back();
                }
                fixed += wholeTag;
                st.pop_back();
            } else {
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
        for (const auto& err : errors) cout << " - " << err << endl;
    }

    return autoFix ? fixed : xmlText;
}
