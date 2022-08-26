#include "cli.h"

#ifdef USE_CLI

// list of function pointers corresponding to each command
void (*commands_func[])(WaterIO *){
    &cmd_help,
    &cmd_calibrate,
    &cmd_pump,
    &cmd_valve,
    &cmd_zone,
    &cmd_sensor};

// available commands for the command-line-interface (CLI)
const char *commands_str[] = {
    "help",
    "calibrate",
    "pump",
    "valve",
    "zone",
    "sensor"
};

const uint8_t num_commands = sizeof(*commands_str) / sizeof(char *);

// list of function pointers corresponding to each command help dialog
void (*commands_help[])()
{
    &help,
    &help_calibrate,
    &help_pump,
    &help_valve,
    &help_zone,
    &help_sensor
};

// arguments for commands
const char *command_args[][MAX_NUM_ARGS] = {
    {NULL},
    {NULL},
    {NULL},
    {NULL},
    {NULL},
    {NULL}
};

void cmd_help()
{
    for (int i = 0; i < num_commands; i++)
    {
        if (args[1] == NULL)
        {
            help();
        }
        else if (strcmp(args[1], commands_str[i]) == 0)
        {
            return (*commands_help[i])();
        }
        else
        {
            help();
        }
    }
}

void cmd_calibrate(WaterIO *instance)
{
    int sensor = atoi(args[1]);

    if (instance->sensors[sensor - 1] == NULL)
    {
        Serial.println("Sensor not found.");
    }
    else
    {
        instance->sensors[sensor - 1]->calibration();
    }
}

void cmd_pump(WaterIO *instance) {}
void cmd_valve(WaterIO *instance) {}
void cmd_zone(WaterIO *instance) {}
void cmd_sensor(WaterIO *instance) {}

// command help dialogs
void help()
{
    Serial.println("The following commands are available:");

    for (uint8_t i = 0; i < num_commands; i++)
    {
        Serial.print("  ");
        Serial.println(commands_str[i]);
    }
    Serial.println("");
    Serial.println("You can for instance type \"help led\" for more info on the LED command.");
}

void help_calibrate() { help(); }
void help_pump() { help(); }
void help_valve() { help(); }
void help_zone() { help(); }
void help_sensor() { help(); }

#endif
