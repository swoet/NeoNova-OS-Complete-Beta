#ifndef DEV_TOOLS_H
#define DEV_TOOLS_H
#include <stdbool.h>
#define MAX_CLI_COMMANDS 32
#define MAX_SCRIPTS 16

typedef int (*cli_command_fn)(int argc, char** argv);

typedef struct cli_command {
    char name[32];
    cli_command_fn fn;
} cli_command_t;

typedef struct script_entry {
    char name[32];
    char* code;
} script_entry_t;

typedef struct dev_tools_manager {
    cli_command_t commands[MAX_CLI_COMMANDS];
    int command_count;
    script_entry_t scripts[MAX_SCRIPTS];
    int script_count;
} dev_tools_manager_t;

void dev_tools_init(dev_tools_manager_t* dt);
int dev_tools_register_command(dev_tools_manager_t* dt, const char* name, cli_command_fn fn);
int dev_tools_execute_command(dev_tools_manager_t* dt, const char* name, int argc, char** argv);
int dev_tools_register_script(dev_tools_manager_t* dt, const char* name, const char* code);
int dev_tools_run_script(dev_tools_manager_t* dt, const char* name);
void dev_tools_tick(dev_tools_manager_t* dt);
void dev_tools_list(dev_tools_manager_t* dt);

#endif // DEV_TOOLS_H 