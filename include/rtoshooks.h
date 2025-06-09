#pragma once

#include <FreeRTOS.h>
#include <FreeRTOS_CLI.h>
#include <cstring>
#include <semphr.h>
#include <stdio.h>
#include <task.h>

void register_cli_commands();

void vCommandConsoleTask(void *params);

// COMMAND DEFINITIONS

BaseType_t cat_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command
);
static const CLI_Command_Definition_t cat_definition = {
	"cat",
	"cat <players|games>:\r\n Shows contents of volatile memory.\r\n\r\n",
	cat_command,
	1
};

BaseType_t top_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command_string
);
static const CLI_Command_Definition_t top_definition = {
	"top",
	"top:\r\n Returns list of running tasks and their \%cpu.\r\n\r\n",
	top_command,
	0
};

BaseType_t hello_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command_string
);
static const CLI_Command_Definition_t hello_definition =
	{"hello", "hello:\r\n Prints hello message.\r\n\r\n", hello_command, 0};

BaseType_t rm_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command_string
);
static const CLI_Command_Definition_t rm_definition = {
	"rm",
	"rm <players|games|*>:\r\n Removes players or games from volatile "
	"memory.\r\n\r\n",
	rm_command,
	1
};

BaseType_t clear_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command
);

static const CLI_Command_Definition_t clear_definition = {
	"clear",
	"clear:\r\n Clears the console screen.\r\n\r\n",
	clear_command,
	0
};

static const CLI_Command_Definition_t cls_definition =
	{"cls", "cls:\r\n Alias for clear.\r\n\r\n", clear_command, 0};

BaseType_t load_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command
);
static const CLI_Command_Definition_t load_definition =
	{"load", "load:\r\n Load data from flash.\r\n\r\n", load_command, 0};

BaseType_t save_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command
);
static const CLI_Command_Definition_t save_definition =
	{"save", "save:\r\n Save data to flash.\r\n\r\n", save_command, 0};

BaseType_t rename_command(
	char *write_buffer,
	size_t write_buffer_len,
	const char *command
);
static const CLI_Command_Definition_t rename_definition = {
	"rename",
	"rename 0x<uid> <name>: rename or create a player\n",
	rename_command,
	2
};