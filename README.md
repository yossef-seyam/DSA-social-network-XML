## How to Build

1. Open terminal in the root folder.
2. Run: `g++ src/main.cpp src/XMLTREE.cpp src/XMLNode.cpp -o bin/xml_editor.exe`
3. Double click `run.bat` to start!

## Commands

g++ -std=c++17 main.cpp XMLTREE.cpp XML.cpp XMLNode.cpp -o xml_editor.exe

xml_editor.exe verify -i sample_v.xml -f -o fixed.xml
xml_editor.exe format -i sample_f.xml -o formatted.xml
xml_editor.exe mini -i sample_m.xml -o mini.xml
xml_editor.exe json -i sample.xml -o output.json
xml_editor.exe compress -i sample.xml -o output_file.comp
xml_editor.exe decompress -i output_file.comp -o output_file.xml

xml_editor.exe draw -i sample.xml -o output_file.jpg

xml_editor.exe most_active -i sample.xml
xml_editor.exe most_influencer -i sample.xml
xml_editor.exe mutual -i sample.xml -ids 1,2
xml_editor.exe suggest -i sample.xml -id 1

xml_editor.exe search -w lorem. -i sample.xml
xml_editor.exe search -t sports -i sample.xml
