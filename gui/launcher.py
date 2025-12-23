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
        if platform.system() == "Windows":
            # 'start cmd /k' opens a new CMD window and keeps it open (/k)
            # We also inject a helpful echo command so the user knows what to do
            cmd_command = (
                'start cmd /k "cd /d ..\\bin && '
                'echo ============================================ && '
                'echo   CLI MODE - You are now in /bin folder      && '
                'echo ============================================ && '
                'echo Try running: xml_editor.exe verify -i ../data/sample.xml && echo."'
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