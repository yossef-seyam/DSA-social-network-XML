## How to Build

1. Open terminal in the root folder.
2. Run: `g++ src/main.cpp src/XMLTREE.cpp src/XMLNode.cpp -o bin/xml_editor.exe`
3. Double click `run.bat` to start!

## Commands

g++ -std=c++17 main.cpp XMLTREE.cpp XML.cpp XMLNode.cpp -o xml_editor.exe

xml_editor verify -i sample_v.xml -f -o fixed.xml
xml_editor format -i sample_f.xml -o formatted.xml
xml_editor mini -i sample_m.xml -o mini.xml
xml_editor json -i sample.xml -o output.json
xml_editor compress -i sample.xml -o compressed_file.comp
xml_editor decompress -i compressed_file.comp -o output_file.xml

xml_editor draw -i sample.xml -o output_file.jpg

xml_editor most_active -i sample.xml
xml_editor most_influencer -i sample.xml
xml_editor mutual -i sample.xml -ids 1,2
xml_editor suggest -i sample.xml -id 1

xml_editor search -w lorem. -i sample.xml
xml_editor search -t sports -i sample.xml
