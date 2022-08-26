#pragma once

#include "coffeeOS.h"

#ifdef USE_CLI

#define LINE_BUF_SIZE 128 // maximum input string length
#define ARG_BUF_SIZE 64   // maximum argument string length
#define MAX_NUM_ARGS 8    // maximum number of arguments

// variables
extern bool error_flag;
extern const uint8_t num_commands;
extern char line[LINE_BUF_SIZE];
extern char args[MAX_NUM_ARGS][ARG_BUF_SIZE];

// functions
void cli_init();
void cli_check(WaterIO*);
void read_line();
void parse_line();
void execute(WaterIO*);

// commands
void cmd_help(WaterIO*);
void cmd_calibrate(WaterIO*);
void cmd_pump(WaterIO*);
void cmd_valve(WaterIO*);
void cmd_zone(WaterIO*);
void cmd_sensor(WaterIO*);

// command help dialogs
void help();
void help_calibrate();
void help_pump();
void help_valve();
void help_zone();
void help_sensor();

// command lookups
extern void (*commands_func[])(WaterIO*);
extern void (*commands_help[])();
extern const char *commands_str[];
extern char *command_args[num_commands][MAX_NUM_ARGS];

#endif
