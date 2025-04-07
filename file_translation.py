import tkinter as tk
from tkinter import filedialog, messagebox
import subprocess
import os

# Function to open file dialog and select a file
def select_file():
# Get current working directory
    current_directory = os.getcwd()
    # Set the cwd to the current working directory
    cwd = os.path.expanduser(current_directory)

    # Open a file dialog to select the file
    file_path = filedialog.askopenfilename(initialdir=cwd, title="Select your wtmp file", filetypes=[("All Files", "*.*")])
    
    if file_path:
        # Set the file path in the label or text entry field
        file_label.config(text=file_path)
        return file_path
    return None

# Function to execute the C program on the selected file
def execute_c_program():
    file_path = file_label.cget("text")
    
    if not file_path or not os.path.isfile(file_path):
        messagebox.showerror("Error", "Please select a valid file.")
        return
    
    # Path to your compiled C program (you should replace this with the correct path)
    c_program_path = "~./wtmp_read_to_file.c"  # Replace with the path to your C executable
    
    try:
        # Run the C program with the selected file as an argument
        result = subprocess.run([c_program_path, file_path], capture_output=True, text=True)
        
        # Check if the program ran successfully
        if result.returncode == 0:
            # Show the output in a messagebox or in the GUI
            output_text = result.stdout
            messagebox.showinfo("Success", "Translation completed successfully!\n\n" + output_text)
        else:
            # Show error message if the program failed
            messagebox.showerror("Error", f"Error in executing C program: {result.stderr}")
    
    except Exception as e:
        messagebox.showerror("Execution Error", f"An error occurred: {e}")

# Create the main window
root = tk.Tk()
root.title("Binary File Translator")

# Create and place a label to display the selected file path
file_label = tk.Label(root, text="No file selected", width=50, anchor="w")
file_label.pack(padx=10, pady=10)

# Create and place a button to select the file
select_button = tk.Button(root, text="Select Your WTMP File: ", command=select_file)
select_button.pack(padx=10, pady=10)

# Create and place a button to execute the C program
execute_button = tk.Button(root, text="Translate To Readable Text: ", command=execute_c_program)
execute_button.pack(padx=10, pady=10)

# Start the GUI event loop
root.mainloop()
