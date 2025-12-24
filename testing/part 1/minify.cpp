string minify(XMLNode* node) {
    string result = "<" + node->tagName + ">";

    if (!node->innerText.empty()) {
        result += node->innerText;
    }

    for (XMLNode* child : node->children) {
        result += minify(child);
    }

    result += "</" + node->tagName + ">";
    return result;
}