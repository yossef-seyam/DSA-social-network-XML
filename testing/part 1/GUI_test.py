import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext
import subprocess
import os

# Function to run the C++ executable
def run_cpp_command(action):
    # 1. Determine Input
    manual_text = text_input.get("1.0", tk.END).strip()
    input_filename = "temp_input.xml" 
    
    if manual_text:
        with open(input_filename, "w") as f:
            f.write(manual_text)
    elif entry_file_path.get():
        input_filename = entry_file_path.get()
    else:
        messagebox.showerror("Error", "Please select a file OR paste XML text.")
        return

    # 2. Determine Output Filename
    output_filename = "temp_output.txt" # Default if user leaves it empty
    if entry_output_path.get():
        output_filename = entry_output_path.get()

    # 3. Build Command
    # We add "-f" automatically if the action is verify, so we always get an output file
    cmd = ["xml_editor.exe", action, "-i", input_filename, "-o", output_filename]
    if action == "verify":
        cmd.append("-f")

    try:
        # Run C++ program
        # capture_output=True allows us to grab the console text (errors)
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        # 4. Display Results
        text_output.config(state=tk.NORMAL) 
        text_output.delete("1.0", tk.END)   
        
        # FIRST: Show the Console Output (The Error List)
        text_output.insert(tk.END, "--- CONSOLE LOG (ERRORS) ---\n")
        text_output.insert(tk.END, result.stdout + "\n\n")

        # SECOND: Show the File Output (The Result)
        if os.path.exists(output_filename):
            text_output.insert(tk.END, f"--- FILE OUTPUT ({output_filename}) ---\n")
            with open(output_filename, "r") as f:
                file_content = f.read()
                text_output.insert(tk.END, file_content)
        else:
             text_output.insert(tk.END, "--- NO FILE OUTPUT CREATED ---")

        text_output.config(state=tk.DISABLED) 
        
        if result.returncode != 0:
             messagebox.showwarning("Warning", "The program finished with errors (see log).")

    except Exception as e:
        messagebox.showerror("Error", f"Failed to run executable.\n{e}")

# --- GUI SETUP ---
root = tk.Tk()
root.title("XML Social Network Editor")
root.geometry("800x700")

# Section 1: Input File Selection
frame_file = tk.Frame(root)
frame_file.pack(pady=10)
tk.Label(frame_file, text="Input File Path:").pack(side=tk.LEFT)
entry_file_path = tk.Entry(frame_file, width=50)
entry_file_path.pack(side=tk.LEFT, padx=5)
def browse_file():
    path = filedialog.askopenfilename(filetypes=[("XML files", "*.xml")])
    entry_file_path.delete(0, tk.END)
    entry_file_path.insert(0, path)
tk.Button(frame_file, text="Browse", command=browse_file).pack(side=tk.LEFT)

# Section 2: Manual Input Text Area
tk.Label(root, text="OR Enter XML Manually:").pack()
text_input = scrolledtext.ScrolledText(root, height=10, width=90)
text_input.pack(pady=5)

# Section 3: Output File Selection
frame_out = tk.Frame(root)
frame_out.pack(pady=10)
tk.Label(frame_out, text="Save Output To (Optional):").pack(side=tk.LEFT)
entry_output_path = tk.Entry(frame_out, width=50)
entry_output_path.pack(side=tk.LEFT, padx=5)
def browse_out():
    path = filedialog.asksaveasfilename(defaultextension=".xml")
    entry_output_path.delete(0, tk.END)
    entry_output_path.insert(0, path)
tk.Button(frame_out, text="Browse", command=browse_out).pack(side=tk.LEFT)

# Section 4: Action Buttons
frame_btns = tk.Frame(root)
frame_btns.pack(pady=10)
tk.Button(frame_btns, text="Verify / Fix", width=15, bg="#ffcccb", command=lambda: run_cpp_command("verify")).grid(row=0, column=0, padx=5)
tk.Button(frame_btns, text="Format", width=15, bg="#90ee90", command=lambda: run_cpp_command("format")).grid(row=0, column=1, padx=5)
tk.Button(frame_btns, text="Minify", width=15, bg="#add8e6", command=lambda: run_cpp_command("mini")).grid(row=0, column=2, padx=5)
tk.Button(frame_btns, text="To JSON", width=15, bg="#ffffe0", command=lambda: run_cpp_command("json")).grid(row=0, column=3, padx=5)

# Section 5: Read-Only Output Area
tk.Label(root, text="Operation Output:").pack()
text_output = scrolledtext.ScrolledText(root, height=10, width=90, state=tk.DISABLED)
text_output.pack(pady=5)

root.mainloop()