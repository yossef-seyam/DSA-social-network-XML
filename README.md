# XML Editor - Social Network Analyzer

A command-line tool for XML parsing, validation, and social network analysis.

## Features

- **XML Operations**: Verify, format, minify, and convert XML to JSON
- **Compression**: Compress and decompress XML files
- **Social Network Analysis**: Find influencers, mutual friends, and suggest connections
- **Visualization**: Generate graph visualizations from XML data
- **Search**: Search posts by word or topic

## Build

```bash
g++ -std=c++17 src/main.cpp src/XMLTREE.cpp src/XML.cpp src/XMLNode.cpp -o bin/xml_editor.exe
```

Or simply run `run.bat` after building.

## Usage

```bash
xml_editor <command> -i <input_file> [options]
```

### XML Commands

| Command | Description | Example |
|---------|-------------|---------|
| `verify` | Check XML validity | `xml_editor verify -i file.xml -f -o fixed.xml` |
| `format` | Pretty-print XML | `xml_editor format -i file.xml -o formatted.xml` |
| `mini` | Minify XML | `xml_editor mini -i file.xml -o mini.xml` |
| `json` | Convert to JSON | `xml_editor json -i file.xml -o output.json` |
| `compress` | Compress XML | `xml_editor compress -i file.xml -o file.comp` |
| `decompress` | Decompress file | `xml_editor decompress -i file.comp -o file.xml` |

### Network Analysis Commands

| Command | Description | Example |
|---------|-------------|---------|
| `draw` | Visualize network | `xml_editor draw -i file.xml -o graph.jpg` |
| `most_active` | Find most active user | `xml_editor most_active -i file.xml` |
| `most_influencer` | Find top influencer | `xml_editor most_influencer -i file.xml` |
| `mutual` | Find mutual friends | `xml_editor mutual -i file.xml -ids 1,2` |
| `suggest` | Suggest connections | `xml_editor suggest -i file.xml -id 1` |
| `search` | Search posts | `xml_editor search -w <word> -i file.xml` |
|  |  | `xml_editor search -t <topic> -i file.xml` |

## Project Structure

```
├── src/          # Source files
├── inc/          # Header files
├── data/         # Sample XML files
├── bin/          # Compiled binaries
└── gui/          # GUI components
```

## Requirements

- C++17 compatible compiler (g++, clang++, MSVC)

## License

MIT License
