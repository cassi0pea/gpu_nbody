"""
Barnes-Hut N-Body Simulator GUI
Author: Cassidy Ureda

This program provides a graphical interface for running the Barnes-Hut gravitational
simulator. It allows users to modify simulation parameters, recompiles the C++ code
with those parameters, and runs the simulation while showing progress.

Requirements:
    - tkinter (usually comes with Python)
    - PIL/Pillow (pip install pillow)
    - The C++ source code in ../src/ relative to this script
    - A working C++ compiler (g++)
    - make or ability to compile manually
"""

import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
import subprocess
import threading
import os
import re
import glob

class SimulatorGUI:
    def __init__(self, root):
        """
        Initialize the GUI window and all its components.

        Args:
            root: The tkinter root window
        """
        self.root = root
        self.root.title("Barnes-Hut Simulator")
        self.root.geometry("1800x900")
        self.root.resizable(True, True)

        # Path to the constants header file
        # Adjust this if your directory structure is different
        self.constants_path = "../src/Constants.h"

        # Store original values from Constants.h
        self.original_values = self.read_constants()

        # Build the GUI
        self.create_widgets()

    def read_constants(self):
        """
        Read the current values from Constants.h.
        This parses the #define statements to get default values.

        Returns:
            dict: A dictionary mapping constant names to their values
        """
        values = {}
        try:
            with open(self.constants_path, 'r') as f:
                content = f.read()

                # Use regex to find #define statements
                # Pattern matches: #define NAME value
                pattern = r'#define\s+(\w+)\s+([^\s/]+)'
                matches = re.findall(pattern, content)

                for name, value in matches:
                    # Clean up the value (remove parentheses, e, etc)
                    clean_value = value.strip('()')
                    values[name] = clean_value

        except FileNotFoundError:
            print(f"Warning: Could not find {self.constants_path}")
            print("Using default values")

        return values

    def create_widgets(self):
        """
        Create all the GUI elements: title, labels, sliders, and buttons.
        """
        # ===== HEADER SECTION =====
        # Title and author name at the top
        header_frame = tk.Frame(self.root, bg="#2c3e50", height=140)
        header_frame.pack(fill=tk.X, pady=(0, 20))
        header_frame.pack_propagate(False)

        title_label = tk.Label(
            header_frame,
            text="Barnes-Hut Gravitational Simulator",
            font=("Courier New", 18, "bold"),
            bg="#2c3e50",
            fg="white"
        )
        title_label.pack(pady=(20, 5))

        author_label = tk.Label(
            header_frame,
            text="Cassidy Ureda",
            font=("Courier New", 12),
            bg="#2c3e50",
            fg="#bdc3c7"
        )
        author_label.pack()

        # ===== MAIN CONTENT AREA =====
        # Split into left (controls) and right (preview)
        main_frame = tk.Frame(self.root, padx=40)
        main_frame.pack(fill=tk.BOTH, expand=True)

        # Left side - controls
        controls_frame = tk.Frame(main_frame)
        controls_frame.pack(side=tk.LEFT, fill=tk.Y)

        # --- Number of Bodies Control ---
        self.create_entry_control(
            controls_frame,
            "Number of Bodies",
            default=100000,
            row=0
        )

        # --- Number of Frames Control ---
        self.create_entry_control(
            controls_frame,
            "Number of Frames",
            default=100,
            row=1
        )

        # --- Image Width Control ---
        self.create_entry_control(
            controls_frame,
            "Image Width (pixels)",
            default=2048,
            row=2
        )

        # --- Image Height Control ---
        self.create_entry_control(
            controls_frame,
            "Image Height (pixels)",
            default=2048,
            row=3
        )

        # ===== PROGRESS SECTION =====
        # Progress bar (hidden initially)
        self.progress_frame = tk.Frame(controls_frame)
        self.progress_frame.grid(row=4, column=0, columnspan=2, pady=20, sticky="ew")

        self.progress_label = tk.Label(
            self.progress_frame,
            text="",
            font=("Helvetica", 10)
        )
        self.progress_label.pack()

        self.progress_bar = ttk.Progressbar(
            self.progress_frame,
            length=400,
            mode='determinate'
        )
        self.progress_bar.pack(pady=5)

        # Hide progress elements initially
        self.progress_frame.grid_remove()

        # ===== RUN BUTTON AND PREDICTION =====
        button_frame = tk.Frame(controls_frame)
        button_frame.grid(row=5, column=0, columnspan=2, pady=20, sticky="ew")

        self.run_button = tk.Button(
            button_frame,
            text="Run Simulation",
            command=self.run_simulation,
            bg="#27ae60",
            fg="white",
            font=("Helvetica", 14, "bold"),
            relief=tk.FLAT,
            cursor="hand2",
            height=2,
            width=20
        )
        self.run_button.pack(side=tk.LEFT)

        # Runtime prediction label
        self.prediction_label = tk.Label(
            button_frame,
            text="",
            font=("Helvetica", 11),
            fg="#7f8c8d"
        )
        self.prediction_label.pack(side=tk.LEFT, padx=(30, 0))

        # ===== STOP BUTTON =====
        self.stop_button = tk.Button(
            controls_frame,
            text="Stop Simulation",
            command=self.stop_simulation,
            bg="#e74c3c",
            fg="white",
            font=("Helvetica", 14, "bold"),
            relief=tk.FLAT,
            cursor="hand2",
            height=2,
            state=tk.DISABLED
        )
        self.stop_button.grid(row=6, column=0, columnspan=2, pady=(0, 20), sticky="ew")

        # Track the running process
        self.running_process = None

        # Update prediction when entries change
        for entry in self.sliders.values():
            entry.bind('<KeyRelease>', lambda e: self.update_prediction())

        # Show initial message
        self.prediction_label.config(text="Begin simulation for estimate")

        # ===== RIGHT SIDE - FRAME PREVIEW =====
        preview_frame = tk.Frame(main_frame, bg="#34495e", relief=tk.SUNKEN, borderwidth=2)
        preview_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=(40, 0))

        # Label to show the image
        self.preview_label = tk.Label(
            preview_frame,
            text="Latest Frame\n(starts when simulation runs)",
            font=("Helvetica", 12),
            bg="#34495e",
            fg="#ecf0f1"
        )
        self.preview_label.pack(expand=True)

    def create_entry_control(self, parent, label_text, default, row):
        """
        Helper function to create a labeled text entry control.

        Args:
            parent: The parent frame to put the control in
            label_text: Text to display above the entry box
            default: Starting value for the entry box
            row: Which row in the grid to place this control
        """
        # Label showing what this entry controls
        label = tk.Label(
            parent,
            text=label_text,
            font=("Helvetica", 11)
        )
        label.grid(row=row, column=0, sticky="w", pady=(10, 5))

        # The text entry box
        entry = tk.Entry(
            parent,
            font=("Helvetica", 11),
            justify='right',
            width=8
        )
        entry.insert(0, str(int(default)))
        entry.grid(row=row, sticky="w", column=0, columnspan=2, padx=(400,0))

        # Store references to the entry for later access
        # The key is a simplified version of the label text
        key = label_text.lower().replace(" ", "_").replace("(", "").replace(")", "")
        if not hasattr(self, 'sliders'):
            self.sliders = {}
        self.sliders[key] = entry

    def create_slider_control(self, parent, label_text, from_, to, default, row):
        """
        Helper function to create a labeled slider control.

        Args:
            parent: The parent frame to put the control in
            label_text: Text to display above the slider
            from_: Minimum value for the slider
            to: Maximum value for the slider
            default: Starting value for the slider
            row: Which row in the grid to place this control
        """
        # Label showing what this slider controls
        label = tk.Label(
            parent,
            text=label_text,
            font=("Helvetica", 11)
        )
        label.grid(row=row, column=0, sticky="w", pady=(10, 5))

        # Label showing the current value
        value_label = tk.Label(
            parent,
            text=str(int(default)),
            font=("Helvetica", 11, "bold"),
            width=10
        )
        value_label.grid(row=row, column=1, sticky="e", pady=(10, 5))

        # The actual slider
        slider = tk.Scale(
            parent,
            from_=from_,
            to=to,
            orient=tk.HORIZONTAL,
            showvalue=0,  # Don't show value on the slider itself
            length=400,
            command=lambda v: value_label.config(text=str(int(float(v))))
        )
        slider.set(default)
        slider.grid(row=row+1, column=0, columnspan=2, sticky="ew", pady=(0, 10))

        # Store references to the slider for later access
        # The key is a simplified version of the label text
        key = label_text.lower().replace(" ", "_").replace("(", "").replace(")", "")
        if not hasattr(self, 'sliders'):
            self.sliders = {}
        self.sliders[key] = slider

    def write_constants(self, num_bodies, width, height):
        """
        Rewrite the Constants.h file with new values.
        This reads the original file and replaces specific #define values.

        Args:
            num_bodies: New value for NUM_BODIES
            width: New value for WIDTH
            height: New value for HEIGHT
        """
        try:
            # Read the original file
            with open(self.constants_path, 'r') as f:
                content = f.read()

            # Replace the specific values we care about
            # Using regex to find and replace #define statements
            content = re.sub(
                r'#define\s+NUM_BODIES\s+\([^)]+\)',
                f'#define NUM_BODIES ({num_bodies})',
                content
            )
            content = re.sub(
                r'#define\s+WIDTH\s+[^\s/]+',
                f'#define WIDTH\t{width}',
                content
            )
            content = re.sub(
                r'#define\s+HEIGHT\s+[^\s/]+',
                f'#define HEIGHT\t{height}',
                content
            )

            # Write the modified content back
            with open(self.constants_path, 'w') as f:
                f.write(content)

            return True

        except Exception as e:
            print(f"Error writing constants: {e}")
            return False

    def run_simulation(self):
        """
        Main function that runs when the "Run" button is clicked.
        This handles the entire workflow: rewriting constants, compiling, and running.
        """
        # Get values from the sliders
        num_bodies = int(self.sliders['number_of_bodies'].get())
        num_frames = int(self.sliders['number_of_frames'].get())
        width = int(self.sliders['image_width_pixels'].get())
        height = int(self.sliders['image_height_pixels'].get())

        # Disable the run button, enable the stop button
        self.run_button.config(state=tk.DISABLED, bg="#95a5a6")
        self.stop_button.config(state=tk.NORMAL, bg="#e74c3c")

        # Show the progress section
        self.progress_frame.grid()
        self.progress_label.config(text="Writing configuration...")
        self.progress_bar['value'] = 0

        # Run the compilation and simulation in a separate thread
        # so the GUI doesn't freeze
        thread = threading.Thread(
            target=self.run_simulation_thread,
            args=(num_bodies, num_frames, width, height)
        )
        thread.daemon = True  # Thread dies when main program exits
        thread.start()

    def run_simulation_thread(self, num_bodies, num_frames, width, height):
        """
        This function runs in a separate thread to avoid freezing the GUI.
        It performs the compile and run steps.

        Args:
            num_bodies: Number of bodies to simulate
            num_frames: Number of frames to generate
            width: Image width in pixels
            height: Image height in pixels
        """
        import time

        try:
            # Step 1: Write new constants
            if not self.write_constants(num_bodies, width, height):
                self.update_progress("Error writing configuration!", 0)
                return

            # Step 2: Compile the C++ code using cmake
            self.update_progress("Compiling...", 10)

            # Change to the build directory
            # CMake projects compile from the build directory
            compile_result = subprocess.run(
                ['cmake', '--build', '.'],
                cwd='../build',
                capture_output=True,
                text=True
            )

            if compile_result.returncode != 0:
                self.update_progress("Compilation failed!", 0)
                print("Compilation error:", compile_result.stderr)
                return

            # Step 3: Run the simulation
            self.update_progress("Running simulation...", 30)

            # Run the compiled binary with the frame count argument
            # The binary should be in ../build after compilation
            process = subprocess.Popen(
                ['./gpu_nbody', str(num_frames)],
                cwd='../build',
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )

            # Store the process so it can be stopped
            self.running_process = process

            # Track timing for prediction
            step_times = []  # Store time taken for each step
            last_step_time = time.time()

            # Monitor the output to track progress
            # Your C++ code prints "Step N" for each frame
            for line in process.stdout:
                print(line.strip())  # Also print to console

                # Look for "Step X" in the output
                if "Step" in line:
                    try:
                        # Extract the step number
                        step = int(line.split()[1])

                        # Calculate time for this step
                        current_time = time.time()
                        step_duration = current_time - last_step_time
                        step_times.append(step_duration)
                        last_step_time = current_time

                        # Calculate progress percentage (30-90% range)
                        progress = 30 + (step / num_frames) * 60
                        self.update_progress(
                            f"Generating frame {step}/{num_frames}",
                            progress
                        )

                        # Calculate and display time estimate
                        if len(step_times) > 0:
                            avg_step_time = sum(step_times) / len(step_times)
                            remaining_steps = num_frames - step
                            estimated_remaining = avg_step_time * remaining_steps

                            if estimated_remaining < 60:
                                time_str = f"{int(estimated_remaining)}s remaining"
                            elif estimated_remaining < 3600:
                                minutes = int(estimated_remaining / 60)
                                seconds = int(estimated_remaining % 60)
                                time_str = f"{minutes}m {seconds}s remaining"
                            else:
                                hours = int(estimated_remaining / 3600)
                                minutes = int((estimated_remaining % 3600) / 60)
                                time_str = f"{hours}h {minutes}m remaining"

                            self.update_prediction_text(time_str)

                        # Update the preview image
                        self.update_preview_image(step - 1)
                    except:
                        pass

            # Wait for the process to complete
            process.wait()

            if process.returncode == 0:
                self.update_progress("Complete! Video saved to images/", 100)
                self.update_prediction_text("Begin simulation for estimate")
            else:
                # Check if it was terminated by stop button
                if process.returncode == -15 or process.returncode == -9:  # SIGTERM or SIGKILL
                    self.update_progress("Simulation stopped by user", 0)
                else:
                    self.update_progress("Simulation failed!", 0)
                self.update_prediction_text("Begin simulation for estimate")

        except Exception as e:
            self.update_progress(f"Error: {str(e)}", 0)
            print(f"Exception in simulation thread: {e}")
            self.update_prediction_text("Begin simulation for estimate")

        finally:
            # Clear the running process reference
            self.running_process = None

            # Re-enable the run button, disable stop button
            self.root.after(0, lambda: self.run_button.config(
                state=tk.NORMAL,
                bg="#27ae60"
            ))
            self.root.after(0, lambda: self.stop_button.config(
                state=tk.DISABLED,
                bg="#95a5a6"
            ))

    def update_progress(self, message, progress):
        """
        Update the progress bar and message.
        This must be called from the GUI thread using root.after().

        Args:
            message: Text to display above the progress bar
            progress: Progress value (0-100)
        """
        self.root.after(0, lambda: self.progress_label.config(text=message))
        self.root.after(0, lambda: self.progress_bar.config(value=progress))

    def update_prediction_text(self, text):
        """
        Update the prediction label text.
        This must be called from the GUI thread using root.after().

        Args:
            text: Text to display in the prediction label
        """
        self.root.after(0, lambda: self.prediction_label.config(text=text))

    def update_prediction(self):
        """
        This method is no longer used but kept for compatibility.
        Prediction is now done dynamically during simulation.
        """
        pass

    def update_preview_image(self, step):
        """
        Load and display the latest frame from the images directory.

        Args:
            step: The step number of the frame to display
        """
        try:
            # Construct the filename based on the step number
            image_path = f"../build/images/Step{step:05d}.ppm"

            if os.path.exists(image_path):
                # Load the PPM image
                img = Image.open(image_path)

                # Resize to fit in the preview area (max 800x800, maintain aspect ratio)
                img.thumbnail((800, 800), Image.Resampling.LANCZOS)

                # Convert to PhotoImage for tkinter
                photo = ImageTk.PhotoImage(img)

                # Update the label (must be done in GUI thread)
                def update_ui():
                    self.preview_label.config(image=photo, text="")
                    self.preview_label.image = photo  # Keep a reference to prevent garbage collection

                self.root.after(0, update_ui)
        except Exception as e:
            print(f"Error loading preview image: {e}")

    def stop_simulation(self):
        """
        Stop the currently running simulation by terminating the process.
        """
        if self.running_process is not None:
            try:
                self.running_process.terminate()  # Send SIGTERM
                self.update_progress("Stopping simulation...", 0)
            except Exception as e:
                print(f"Error stopping simulation: {e}")

# ===== MAIN ENTRY POINT =====
if __name__ == "__main__":
    # Create the main window
    root = tk.Tk()

    # Create the GUI
    app = SimulatorGUI(root)

    # Start the GUI event loop
    root.mainloop()
