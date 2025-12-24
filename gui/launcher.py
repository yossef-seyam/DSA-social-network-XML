import os
import subprocess
import sys
import platform

def main():
    print("================================================")
    print("   DSA Project: XML Editor & Graph Analysis     ")
    print("================================================")
    print("Please select your interface:")
    print("1. GUI Mode (Graphical Interface)")
    print("2. CLI Mode (Command Line Interface)")
    print("------------------------------------------------")
    
    choice = input("Enter choice (1 or 2): ").strip()

    if choice == '1':
        print("\n[+] Launching GUI...")
        try:
            # Uses the current Python interpreter to run the GUI script
            subprocess.run([sys.executable, "GUI.py"])
        except Exception as e:
            print(f"Error launching GUI: {e}")
            input("Press Enter to exit...")

    elif choice == '2':
        print("\n[+] Opening Command Prompt...")
        
        # Windows Logic
        print("\n[+] Opening Command Prompt...")
        
        # Get project paths
        gui_dir = os.path.dirname(os.path.abspath(__file__))
        project_root = os.path.join(gui_dir, "..")
        project_root = os.path.abspath(project_root)
        
        if platform.system() == "Windows":
            help_text = [
                "==================================================",
                "  XML Editor CLI - Project Root Directory",
                "==================================================",
                "",
                "Available Commands:",
                "",
                "  xml_editor verify -i data/sample.xml [-f] [-o output.xml]",
                "  xml_editor format -i data/sample.xml [-o formatted.xml]",
                "  xml_editor mini -i data/sample.xml [-o minified.xml]",
                "  xml_editor json -i data/sample.xml [-o output.json]",
                "  xml_editor compress -i data/sample.xml [-o compressed.comp]",
                "  xml_editor decompress -i compressed.comp [-o decompressed.xml]",
                "",
                "  xml_editor draw -i data/sample.xml [-o graph.jpg]",
                "  xml_editor most_active -i data/sample.xml",
                "  xml_editor most_influencer -i data/sample.xml",
                "  xml_editor mutual -i data/sample.xml -ids 1,2",
                "  xml_editor suggest -i data/sample.xml -id 1",
                "  xml_editor search -w lorem -i data/sample.xml",
                "  xml_editor search -t sports -i data/sample.xml",
                "",
                "Note: All output files will be saved in the same folder as input!",
                ""
            ]
            
            # Build the echo commands
            echo_lines = " && ".join([f'echo {line}' for line in help_text])
            
            cmd_command = (
                f'start cmd /k "cd /d "{project_root}" && '
                f'{echo_lines}'
                '"'
            )
            os.system(cmd_command)
        else:
            # Unix/Linux/Mac fallback
            print("CLI auto-launch is optimized for Windows.")
            print("Please manually open a terminal and cd to the bin folder.")
        

    else:
        print("\nInvalid selection. Please run the launcher again.")
        input("Press Enter to exit...")

if __name__ == "__main__":
    main()