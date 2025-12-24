string prettify(XMLNode* node, int depth) {
    string indent(depth *4, ' ');
    string result = indent + "<" + node->tagName + ">\n";

    if (!node->innerText.empty()) {
        result += indent + "\t" + node->innerText + "\n";
    }

    for (XMLNode* child : node->children) {
        result += prettify(child, depth + 1);
    }

    result += indent + "</" + node->tagName + ">\n";
    return result;
}