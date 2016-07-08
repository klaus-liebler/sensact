/*
 * This file is part of the libemb project.
 *
 * Copyright (C) 2011 Stefan Wendler <sw@kaltpost.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "shell.h"
#include "console.h"
#include "cModel.h"
#include "cMaster.h"
#include <cmath>
#include "date.h"

extern uint64_t epochtimer;

using namespace date;
using namespace std::chrono;

namespace sensact{

static eShellError cmdHelp(shell_cmd_args *args);
static eShellError cmdArgt(shell_cmd_args *args);
static eShellError cmdCs(shell_cmd_args *args);
static eShellError cmdSettime(shell_cmd_args *args);

static const shell_cmd  myCmds[] = {
		{
				"h",
				"list available commands",
				cmdHelp,
		},
		{
				"argt",
				"print back given arguments",
				cmdArgt,
		},
		{
				"cs",
				"can send - send message over can bus",
				cmdCs,
		},
		{
				"setepochtime",
				"sets the time as second from epoch",
				cmdSettime,
		},
};

static const shell_cmds ShellCmds1 = { 4, myCmds, };

//The example below shows the definition of the two shell commands "h" and "argt":

static eShellError cmdHelp(shell_cmd_args *args) {
	(void) args;
	int i;
	for (i = 0; i < ShellCmds1.count; i++) {
		sensact::Console::Writeln("%s, %s", ShellCmds1.cmds[i].cmd,
				ShellCmds1.cmds[i].desc);
	}

	sensact::Console::Writeln((const char *) "OK");

	return eShellError::PROCESS_OK;
}

static eShellError cmdArgt(shell_cmd_args *args) {
	int i;

	sensact::Console::Writeln((char *) "OK\n\rargs given:");

	for (i = 0; i < args->count; i++) {
		sensact::Console::Writeln(" - %s", args->args[i].val);
	}

	return eShellError::PROCESS_OK;
}

static eShellError cmdCs(shell_cmd_args *args) {
	if(args->count!=2)
	{
		return eShellError::PROCESS_ERR_ARGS_LEN;
	}
	int cmdNameLength = cShell::strlen(args->args[0].val);
	uint16_t i;
	for (i = 1; i < (uint16_t) eApplicationID::CNT; i++) {
		int testCmdNameLength = cShell::strlen(MODEL::ApplicationNames[i]);
		if(cShell::strcmp(args->args[0].val, MODEL::ApplicationNames[i], cmdNameLength, testCmdNameLength)==0)
		{
			eApplicationID appId = (eApplicationID)i;
			uint8_t command = cShell::parseInt(args->args[1].val);
			cMaster::SendCommandToMessageBus(BSP::GetTime(), appId, (eCommandType)command, NULL, 0);
		}
	}
	return eShellError::PROCESS_OK;
}

static eShellError cmdSettime(shell_cmd_args *args) {
	if(args->count!=1)
	{
		return eShellError::PROCESS_ERR_ARGS_LEN;
	}
	int64_t time = cShell::parseULong(args->args[0].val);
	if(std::abs(int64_t(time - epochtimer))>3) //only set the clock, if error is significant
	{
		epochtimer = (uint64_t)time;
		sensact::Console::Writeln("Set the clock to a new value");
	}
	return eShellError::PROCESS_OK;
}



int cShell::strlen(const char *str) {
	int i = 0;

	while (str[i++] != 0)
		;

	return i - 1;
}

int cShell::strcmp(const char *str1, const char *str2, int len1, int len2) {
	int i;

	if (len1 > len2)
		return 1;

	for (i = 0; i < len1; i++) {
		if (str1[i] != str2[i])
			return 2;
	}

	// make sure we matched a whole command, and not only
	// a containing substring ...
	if (len2 > len1 && str2[i] != ' ') {
		return 2;
	}
	return 0;
}


uint32_t cShell::parseInt(const char *str) {

	int val = 0;
	int i = 0;
	char c;
	while ((c = str[i++]) != 0) {
		if (c < '0' || c > '9')
			break;
		val = val * 10 + (c - '0');
	}
	return val;
}

uint64_t cShell::parseULong(const char *str) {

	int val = 0;
	int i = 0;
	char c;
	while ((c = str[i++]) != 0) {
		if (c < '0' || c > '9')
			break;
		val = val * 10 + (c - '0');
	}
	return val;
}


uint32_t cShell::parseHex(const char *str) {

	int val = 0;
	if(str[0]!='0') return 0;
	if(str[1]!='x') return 0;
	int i = 2;
	char c;
	while ((c = str[i++]) != 0) {
		if (c >= '0' && c <= '9')
		{
			val = val * 16 + (c - '0');
		}
		else if(c >= 'A' && c <= 'F')
		{
			val = val * 16 + (c - 55);
		}
		if(c >= 'a' && c <= 'f')
		{
			val = val * 16 + (c - 87);
		}
		else
		{
			return 0;
		}
	}
	return val;
}

static int arg_parser(const char *cmd_line, int len, shell_cmd_args *args) {
	int i;
	int j;
	int spos = 0;
	int argc = 0;

	for (i = 0; i < len; i++) {
		// to many arguments
		if (argc > MAX_ARGS)
			return 1;

		if (cmd_line[i] == ' ' || i == len - 1) {
			// catch last argument ...
			if (i == len - 1)
				i++;

			// ignore first since it is the cmd itself
			if (spos == 0) {
				spos = i;
			} else {
				// argument value to long
				if (i - spos > MAX_ARG_LEN)
					return 2;

				for (j = 0; j < i - spos - 1; j++) {
					args->args[argc].val[j] = cmd_line[spos + 1 + j];
				}
				args->args[argc++].val[j] = 0;
				spos = i;
			}
		}
	}

	args->count = argc;

	return 0;
}

eShellError cShell::processCmds(const char *cmd_line) {
	int i;
	int ret;
	int cmd_len;
	int cmd_line_len;

	const shell_cmds *const cmds = &ShellCmds1;
	shell_cmd_args args;

	for (i = 0; i < cmds->count; i++) {
        cmd_line_len 	= strlen(cmd_line);
        cmd_len 		= strlen((char *)(cmds->cmds[i].cmd));

		if (strcmp((const char *) (cmds->cmds[i].cmd), cmd_line, cmd_len,
				cmd_line_len) == 0) {
			ret = arg_parser(cmd_line, cmd_line_len, &args);

			if (ret == 1)
				return eShellError::PROCESS_ERR_ARGS_MAX;
			if (ret == 2)
				return eShellError::PROCESS_ERR_ARGS_LEN;

			return (cmds->cmds[i].func)(&args);
		}
	}

	return eShellError::PROCESS_ERR_CMD_UNKN;
}
}
