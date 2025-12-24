import sys
import os
import subprocess
from pathlib import Path

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
    script_dir = Path(__file__).parent.absolute()
    exe_path = script_dir / "bin" / "xml_editor.exe"
    
    # Change to data directory
    data_dir = script_dir / "data"
    
    # Process arguments WITHOUT adding data/ prefix
    args = sys.argv[1:]
    processed_args = []
    
    for i, arg in enumerate(args):
        processed_args.append(arg)
        if arg in ['-i', '-o'] and i + 1 < len(args):
            next_arg = args[i + 1]
            processed_args.append(next_arg)
            i += 1
    
    print(f"[Executing from {data_dir}]: {exe_path} {' '.join(processed_args)}")
    
    # Run from data directory
    result = subprocess.run([str(exe_path)] + processed_args, cwd=data_dir)
    sys.exit(result.returncode)

if __name__ == "__main__":
    main()
