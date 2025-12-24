import os
import platform
import sys

def show_menu():
    print("\n" + "="*50)
    print("        XML Editor - Main Menu")
    print("="*50)
    print("1. GUI Mode (Graphical Interface)")
    print("2. CLI Mode (Command Line Interface)")
    print("3. Exit")
    print("="*50)
    
    while True:
        choice = input("\nSelect mode (1-3): ").strip()
        if choice in ['1', '2', '3']:
            return choice
        print("Invalid choice. Please enter 1, 2, or 3.")

def run_gui_mode():
    print("\n[+] Starting GUI Mode...")
    gui_path = os.path.join(os.path.dirname(__file__), "GUI.py")
    os.system(f'python "{gui_path}"')

def run_cli_mode():
    print("\n[+] Opening CLI Mode...")
    
    # Get absolute paths
    current_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.join(current_dir, "..")
    project_root = os.path.abspath(project_root)
    
    if platform.system() == "Windows":
        help_text = [
            "==========================================================",
            "  XML Editor CLI - Enhanced Mode",
            "==========================================================",
            "",
            "How it works:",
            "  1. You type: xml_editor format -i sample.xml -o output.xml",
            "  2. System runs: bin\\xml_editor.exe -i data\\sample.xml -o data\\output.xml",
            "",
            "Examples (type exactly as shown):",
            "  xml_editor verify -i sample.xml -f -o fixed.xml",
            "  xml_editor format -i sample.xml -o formatted.xml",
            "  xml_editor json -i sample.xml -o output.json",
            "  xml_editor draw -i sample.xml -o graph.jpg",
            "  xml_editor compress -i sample.xml -o compressed.comp",
            "  xml_editor decompress -i compressed.comp -o decompressed.xml",
            "",
            "  xml_editor most_active -i sample.xml",
            "  xml_editor most_influencer -i sample.xml",
            "  xml_editor mutual -i sample.xml -ids 1,2",
            "  xml_editor suggest -i sample.xml -id 1",
            "  xml_editor search -w lorem -i sample.xml",
            "  xml_editor search -t sports -i sample.xml",
            "",
            "Note: All files are automatically prefixed with data/",
            ""
        ]
        
        # Create a Python wrapper script
        wrapper_content = '''import sys
import os
import subprocess

def process_args(args):
    """Add data/ prefix to file arguments"""
    processed = []
    i = 0
    while i < len(args):
        arg = args[i]
        processed.append(arg)
        
        # Check if next argument should be prefixed with data/
        if arg in ['-i', '-o'] and i + 1 < len(args):
            next_arg = args[i + 1]
            # Don't add data/ if already present or is a special flag
            if not next_arg.startswith('data/') and not next_arg.startswith('-'):
                processed.append('data/' + next_arg)
                i += 1  # Skip the next arg since we've processed it
        i += 1
    
    return processed

def main():
    # Get paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    exe_path = os.path.join(script_dir, "bin", "xml_editor.exe")
    
    if not os.path.exists(exe_path):
        print(f"Error: Executable not found at {exe_path}")
        sys.exit(1)
    
    # Process command line arguments
    if len(sys.argv) < 2:
        print("Usage: xml_editor <command> [options]")
        print("Example: xml_editor verify -i sample.xml -o output.xml")
        sys.exit(1)
    
    # Process arguments to add data/ prefix
    processed_args = process_args(sys.argv[1:])
    
    # Debug: Show what's being executed
    print(f"[Executing]: {exe_path} {' '.join(processed_args)}")
    print("-" * 60)
    
    # Run the command
    cmd = [exe_path] + processed_args
    result = subprocess.run(cmd, cwd=script_dir)
    sys.exit(result.returncode)

if __name__ == "__main__":
    main()
'''
        
        # Save Python wrapper to project root
        wrapper_file = os.path.join(project_root, "xml_editor.py")
        with open(wrapper_file, 'w') as f:
            f.write(wrapper_content)
        
        # Create batch file to call Python wrapper
        batch_content = f'''@echo off
REM Call the Python wrapper
python "{wrapper_file}" %*
'''
        
        batch_file = os.path.join(project_root, "xml_editor.bat")
        with open(batch_file, 'w') as f:
            f.write(batch_content)
        
        # Display help and open command prompt
        echo_lines = " && ".join([f'echo {line}' for line in help_text])
        cmd_command = (
            f'start cmd /k "cd /d "{project_root}" && '
            f'{echo_lines} && '
            f'echo. && '
            f'echo Type commands below: && echo."'
        )
        os.system(cmd_command)

def main():
    print("Welcome to XML Editor Project")
    
    while True:
        choice = show_menu()
        
        if choice == '1':
            run_gui_mode()
        elif choice == '2':
            run_cli_mode()
        elif choice == '3':
            print("\nGoodbye!")
            break

if __name__ == "__main__":
    main()