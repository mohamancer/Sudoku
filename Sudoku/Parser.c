#include "Parser.h"

/****************************************** Module ******************************************
 *  Module: Parser
 *  Implements: Parser.h
 *  Includes: None
 *  Usage:
 *     This module implements the functions to parse commands from user input.
 *     It receives commands in string form and parses it into the provided command struct.
 *     It is used by the Main_Aux module to abstract the command before providing it for
 *     processing at the Game_Operator.
 *     Read the definitions_db.h documentation for more information about the command struct.
 *     This module includes some internally used functions for performing the main functinality
 *     of the module.
 *******************************************************************************************/
/* Internally used functions (partial list - others also exist) */
static void set_command(COMMAND_TYPE *command_type,char *str);
static void set_path(command *cmd_p);
static void set_para(command *cmd_p, int num_of_para);
static int is_number(char *s);
static int is_float(char *s);
static void print_invalid_para_num(int num_of_para, int p);

/*  
 *  Function: parse_command
 *      Functionality:
 *          Receives a non blank line of user input. It parses the command and updates 
 *          the command struct pointed to by cmd_p to contain the values needed for the
 *          command to be processed
 *      Parameters:
 *          -   char *line  :   the line to be parsed
 *          -   command *cmd_p  :   the command structure to be filled
 *      Return Value:
 *          void
 */
void parse_command(char *line, command *cmd_p)
{
	char* str;
	COMMAND_TYPE command_type;

	str = strtok (line," \t\r\n");
	set_command(&command_type,str);
	cmd_p->cmd_type = command_type;
	switch (command_type)
	{
		case INVALID_COMMAND:	return;
		case SOLVE:	set_path(cmd_p); return;
		case EDIT:	set_path(cmd_p); return;
		case MARK_ERRORS:	set_para(cmd_p,1); return;
		case PRINT_BOARD:	return;
		case SET:	set_para(cmd_p,3); return;
        case VALIDATE:	return;
		case GENERATE:	set_para(cmd_p,2); return;
		case GUESS: set_para(cmd_p,1); return;
		case UNDO:	return;
		case REDO:	return;
		case SAVE:	set_path(cmd_p); return;
		case HINT:	set_para(cmd_p,2); return;
		case GUESS_HINT: set_para(cmd_p,2); return;
		case NUM_SOLUTIONS:	return;
		case AUTOFILL:	return;
		case RESET:	return;
		case EXIT:	return;
        default: printf("Shouldn't be here!\n"); return;
	}
}

/*  
 *  Function: set_command
 *      Functionality:
 *          Receives a string that should represent the command name given by the 
 *          user. It checks if it matches one of the commands already defined and updates the 
 *          enum pointed to by command_type. If no match is found the command_type is set
 *          to INVALID_COMMAND.
 *      Parameters:
 *          -   COMMAND_TYPE *command_type  :   pointer to enum to be updated
 *          -   char *str : string that represents the command name.
 *      Return Value:
 *          void
 */
static void set_command(COMMAND_TYPE *command_type,char *str)
{
	if (!strcmp(str, "solve"))
		*command_type = SOLVE;
	else if(!strcmp(str, "edit"))
		*command_type = EDIT;
	else if(!strcmp(str, "mark_errors"))
		*command_type = MARK_ERRORS;
	else if(!strcmp(str, "print_board"))
		*command_type = PRINT_BOARD;
	else if(!strcmp(str, "set"))
		*command_type = SET;
	else if(!strcmp(str, "validate"))
		*command_type = VALIDATE; 
	else if(!strcmp(str, "generate"))
		*command_type = GENERATE;
	else if (!strcmp(str, "guess"))
		*command_type = GUESS;
	else if(!strcmp(str, "undo"))
		*command_type = UNDO;
	else if(!strcmp(str, "redo"))
		*command_type = REDO;
	else if(!strcmp(str, "save"))
		*command_type = SAVE;
	else if(!strcmp(str, "hint"))
		*command_type = HINT;
	else if (!strcmp(str, "guess_hint"))
		*command_type = GUESS_HINT;
	else if(!strcmp(str, "num_solutions"))
		*command_type = NUM_SOLUTIONS;
	else if(!strcmp(str, "autofill"))
		*command_type = AUTOFILL;
	else if(!strcmp(str, "reset"))
		*command_type = RESET;
	else if(!strcmp(str, "exit"))
		*command_type = EXIT;
	else *command_type = INVALID_COMMAND;
}

/*  
 *  Function: set_path
 *      Functionality:
 *          Receives a pointer to a command struct that represents a command that has
 *          a string parameter potentially, and updates the file_path of the command
 *          struct to point to the file_path within the line received by parse_command,
 *          this gets performed using the strtok function which also sets the end of the
 *          file_path to a null charachter.
 *          If no file_path is present in the line when needed, the command in invalidated.
 *      Parameters:
 *          -   command *cmd_p  : pointer to command struct to be updated.
 *      Return Value:
 *          void
 */
static void set_path(command *cmd_p)
{
	cmd_p->file_path = strtok (NULL," \t\r\n");
	if (cmd_p->file_path==NULL && cmd_p->cmd_type != EDIT)
	{
		cmd_p->cmd_type=INVALID_COMMAND;
	}
}

/*  
 *  Function: set_para
 *      Functionality:
 *          Receives a pointer to a command struct that represents a command that has
 *          a num_of_para integers parameters or one double para and updates these parameters in the command 
 *          struct from the line received by parse_command, this gets performed using the strtok function
 *          to read the given number of parameters, if there isn't enough parameters the commnad is
 *          invalidated and returned, otherwise they are converted to integers if possible otherwise
 *          the constant INVALID_PARA (defined in definitions_db.h) is put into the struct and the command is 
 *          then considered from the given type but with invalid parameters, and then it gets given to Game_Operator
 *          for printing the needed error message which contains current board specific values .
 *      Parameters:
 *          -   command *cmd_p  : pointer to command struct to be updated.
 *          -   int num_of_para :   number of parameters to be read.
 *      Return Value:
 *          void
 */
static void set_para(command *cmd_p, int num_of_para)
{
	int i;
	char *temp, temp_arr[30] = {'\0'};
	if (cmd_p->cmd_type==GUESS) {
		temp= strtok (NULL," \t\r\n");
		if (!temp){
			print_invalid_para_num(num_of_para,0);
			cmd_p->cmd_type=INVALID_PARA_NUM; 
		}else if(is_float(temp)){
			cmd_p->para=atof(temp);
                    }else {
			cmd_p->para = INVALID_PARA;
        }
		temp= strtok (NULL," \t\r\n");
		if (temp){
			print_invalid_para_num(num_of_para,1);
			cmd_p->cmd_type=INVALID_PARA_NUM;
		}
	}else{
		for (i = 0; i < num_of_para; i++)
		{
			temp= strtok (NULL," \t\r\n");
			if (!temp) {
				print_invalid_para_num(num_of_para,0);
				cmd_p->cmd_type=INVALID_PARA_NUM; 
				break;
			} else if(is_number(temp)){
				cmd_p->parameters[i] = atoi(temp);
				sprintf(temp_arr, "%d", cmd_p->parameters[i]);
				if(strcmp(temp_arr, temp))
					cmd_p->parameters[i] = INVALID_PARA;
			} else {
				cmd_p->parameters[i] = INVALID_PARA;
			}
		}
		temp= strtok (NULL," \t\r\n");
		if (temp){
			print_invalid_para_num(num_of_para,1);
			cmd_p->cmd_type=INVALID_PARA_NUM;
		}
	}
	
}

/*  
 *  Function: is_number
 *      Functionality:
 *          Checks if a given string is the decimal representation of an integer.
 *      Parameters:
 *          char *s :   string to be checked.
 *      Return Value:
 *          1 if the string represents a number, otherwise 0.
 */
static int is_number(char *s)
{
	int len, i;
	len = strlen(s);
    for (i = 0; i < len; i++)
        if (! isdigit(s[i]))
            return 0;
    return 1;
}
/*  
 *  Function: is_float
 *      Functionality:
 *          Checks if a given string is the decimal representation of a float.
 *      Parameters:
 *          char *s :   string to be checked.
 *      Return Value:
 *          1 if the string represents a float, otherwise 0.
 */
static int is_float(char *s)
{
	int len, i;
	len = strlen(s);
	if (len==1 && (s[0]==0 || s[0]==1)) return 1;
	else {/* len>1 */
		if (s[0]=='0' && s[1]=='.'){
			for (i = 2; i < len; i++)
				if (! isdigit(s[i]))
					return 0;
			return 1;
		}
		if (s[0]=='1' && s[1]=='.'){
			for (i = 2; i < len; i++)
				if (s[i]!=0)
					return 0;
		}
	}
	return 1;
}/*  
 *  Function: print_invalid_para_num
 *      Functionality:
 *          prints the error relating to required parameters 
 *      Parameters:
 *          num_of_para: number of parameters 
 *      Return Value:
 *          void
 */
static void print_invalid_para_num(int num_of_para, int p){
	if(p) printf("ERROR: Too many parameters, number of parameters should be %d\n",num_of_para); 
	else printf("ERROR: Too few parameters, number of parameters should be %d\n",num_of_para);
}

