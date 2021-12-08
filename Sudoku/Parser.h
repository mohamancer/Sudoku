#ifndef PARSER_H_
#define PARSER_H_
#include "definitions_db.h"

/**************************************** Interface ****************************************
 *  Interface: Parser
 *  Implemantation: Parser.c
 *  Including modules:  Main_Aux.c 
 *  Usage:
 *      This interface is used in the main game loop. It receives a non-blank line
 *      of legal length. It parses it according to the predefined user commands and
 *      fills the provided command struct for processing.
 *******************************************************************************************/

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
void parse_command(char *line, command *cmd_p);

#endif 
