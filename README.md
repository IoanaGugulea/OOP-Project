## Flow Management System

This C++ program is a Flow Management System designed to create, manage, and execute flows consisting of various steps. Each step can perform specific actions, such as displaying text, taking user input, reading files, performing calculations, and generating output.

### Features:

- **Create Flow**: Allows the user to create a new flow by specifying a name and adding a series of steps to it.
- **Delete Flow**: Enables the user to delete an existing flow by providing its name.
- **Run Flow**: Executes a selected flow, prompting the user to interact with each step as required.
- **Print Available Flows**: Displays the names of all available flows in the system.
- **Step-wise Analytics**: Tracks and displays analytics information for each step within a flow, including the number of times each step was started, completed, skipped, and encountered errors.
- **Average Errors per Flow**: Calculates and prints the average number of errors per flow completed across all flows.

### Steps:

The program supports various types of steps, including:

- **TitleStep**: Displays a title and subtitle.
- **TextStep**: Displays a title and textual content.
- **TextInputStep**: Takes text input from the user.
- **CSVInputStep**: Reads and processes data from a CSV file.
- **FileInputStep**: Reads and displays content from a text file.
- **TextFileInputStep**: Specialized step for reading and displaying content from a text file.
- **DisplayStep**: Displays the output of the previous step.
- **NumberInputStep**: Takes numerical input from the user.
- **CalculusStep**: Performs calculations on input values based on specified operations.
- **OutputStep**: Generates output to a text file based on provided content.

### System:

- **Flow**: Represents a sequence of steps to be executed.
- **Step**: Base class for all types of steps.
- **System**: Manages the creation, deletion, and execution of flows.

### How to Use:

1. **Compile**: Compile the code using a C++ compiler (e.g., g++).
2. **Run**: Execute the compiled program.
3. **Choose an Option**: Select an option from the menu:
   - Create Flow
   - Delete Flow
   - Run Flow
   - Print Available Flows
   - Exit

4. **Follow Instructions**: Follow the on-screen instructions to create, delete, or run flows.

### Requirements:

- C++ compiler supporting C++11 or later.
- Standard C++ libraries (iostream, fstream, sstream, string, vector, algorithm, exception, map).

### Note:

- Ensure that input files (e.g., CSV, text) are available in the specified locations for relevant steps.
- Handle errors and invalid inputs as prompted by the program.

### Disclaimer:

This program is provided as-is without any warranties. Use it at your own risk.

---

**Author**: Ioana Gugulea

**Date**: 14th January 2024
