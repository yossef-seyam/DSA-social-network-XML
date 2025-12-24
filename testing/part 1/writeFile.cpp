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