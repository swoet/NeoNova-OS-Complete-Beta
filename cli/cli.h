#ifndef CLI_H
#define CLI_H

#define MAX_COMMAND_LENGTH 256
#define MAX_COMMAND_ARGS 10
#define MAX_OUTPUT_LENGTH 1024 // For a single output line/block

// Structure for a registered command
typedef struct {
    const char* name;
    const char* description;
    // Function pointer for the command handler
    // Takes argc (argument count, including command name)
    // and argv (argument values, argv[0] is command name)
    // Returns 0 on success, non-zero on error or for specific statuses
    int (*handler)(int argc, char* argv[]);
} CLI_Command;

// Initialize the CLI system (e.g., register built-in commands)
void CLI_Initialize();

// Process a single command line input string
// This function will parse the input, find the command, and execute its handler.
// It will manage printing output (e.g., to a console window or buffer).
void CLI_ProcessInput(const char* input_string);

// For direct output from CLI commands or the system
void CLI_DisplayOutput(const char* format, ...); // Variadic for printf-like behavior
void CLI_DisplayError(const char* format, ...);

// (Conceptual) Functions for getting input if CLI runs in its own interactive loop
// char* CLI_GetLine(); // Gets a line of input from the user

// Register a new command with the CLI system
// Returns 0 on success, -1 if command table is full or name is duplicate
int CLI_RegisterCommand(CLI_Command command);


#endif // CLI_H
