#pragma once

#include <inttypes.h>


	//max number of character for a command line passed to the shell
#define MAX_CMD_LINE 90
	//max number of arguments passed with one command to the shell
#define MAX_ARGS 5
	//max number of character for a single argument form a command line passed to the shell
#define MAX_ARG_LEN 15


namespace sensact{
enum struct eShellError:uint16_t
{
	//return code given when processing of a command line was OK
	PROCESS_OK=0,
	//ERROR maximum number of arguments was reached
	PROCESS_ERR_ARGS_MAX=0xfff0,
	//ERROR maximum number of chars for an argument was reached
	PROCESS_ERR_ARGS_LEN=0xfff1,
	//ERROR unknown command
	PROCESS_ERR_CMD_UNKN=0xfff2,

};
/**
 * Single command argument
 */
typedef struct {
     /**
      * Value representing the argument
      */
     char 			val[MAX_ARG_LEN];
} shell_cmd_arg;

/**
 * All arguments from a single command line
 */
typedef struct {
     /**
      * Number of arguments
      */
     uint8_t	count;

     /**
      * The arguments
      */
     shell_cmd_arg	args[MAX_ARGS];
} shell_cmd_args;

/**
 * Definition of a single shell command
 */
typedef struct {
     /**
      * Name of the command
      */
     const char 	*cmd;

     /**
      * Description of the command
      */
     const char		*desc;

     /**
      * Functino called when executing the commmand
      */
     eShellError (*func)(shell_cmd_args *args);
} shell_cmd;

/**
 * All shell commands knwon by the shell
 */
typedef struct {
     /**
      * Number of commands
      */
     const uint8_t		count;

     /**
      * The commands
      */
     const shell_cmd *const cmds;
} shell_cmds;



class cShell
{
public:


	static shell_cmds ShellCmds;
	/**
	 * Return the length of a given string.
	 *
	 * @param[in]	str	string for which to calculate the length
	 * @return		length of str
	 */
	static int strlen(const char *str);

	/**
	 * Comapre str1 with given length len1 to str2 with given length len2.
	 *
	 * @param[in] 	str1	first string
	 * @param[in] 	str2	second string
	 * @param[in] 	len1	length of first string
	 * @param[in] 	len2	length of second string
	 * @return	0 if str1 euqals str2, 1 if len1 != len2, 2 if str1 != str2
	 */
	static int strcmp(const char *str1, const char *str2, int len1, int len2);

	/**
	 * Parse the integer value from str and return it.
	 *
	 * @param[in]	str from which to parse the integer value.
	 * @return	the integer value of str
	 */
	static uint32_t parseInt(const char *str);
	static uint64_t parseULong(const char *str);
	static uint32_t parseHex(const char *str);

	/**
	 * Process a command line string given in cmd_line against the
	 * commands given by cmds. If the command form cmd_line matches
	 * against a command string defined in cmds, the function callback
	 * for that command is executet.
	 * <br/>
	 * Note: the arguments form the command line are passed to the command
	 * function, but the command function is responsible for checkeing the arguemts.
	 *
	 * @param[in]	*cmds	pointer to shell commands structure
	 * @param[in]	*cmd_line	pointer to command line string
	 * @return 	SHELL_PROCESS_OK if command and arguments where understood,
	 * 			SHELL_PROCESS_ERR_ARGS_MAX if to many arguments are given,
	 * 			SHELL_PROCESS_ERR_ARGS_LEN if an argument string was too long,
	 * 			SHELL_PROCESS_ERR_CMD_UNK if the command was unknown
	 */
	static eShellError processCmds(uint8_t * cmd_line, const uint16_t size);
	static eShellError processBinaryCmd(uint8_t * cmdBuffer, const uint16_t size);
	static eShellError processTextCmd(const char * cmd_line, const uint16_t size);
};
}




