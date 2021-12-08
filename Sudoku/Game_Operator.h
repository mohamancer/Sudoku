#ifndef GAME_OPERATOR_H_ 
#define GAME_OPERATOR_H_
#include "definitions_db.h"

/**************************************** Interface ****************************************
 *  Interface: Game Operator
 *  Implemantation: Game_Operator.c
 *  Including modules:  Main_Aux.c 
 *  Usage:
 *      This module receives a command struct from Main_Aux, it checks if the game mode 
 *      allowes for the command to be executed, it checks the parameters of the commands
 *      for legal type and range and then performs the commands by calling the needed 
 *      functions from the Game module.
 ********************************************************************************************/

/*  
 *  Function: process_command
 *      Functionality:
 *          This function receives a command struct that was filled by the Parser, it performs
 *          game mode checking, and when needed it calls the function in Game responsible
 *          for performing the command.
 *          -   The command type is neither EXIT nor INVALID_COMMAND, these are already dealed with in
 *              Main_Aux.
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command to be processed
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          Returns PROCSS_SUCCESS when the processing of a command was successful, otherwise returns
 *          PROCESS_FAIL to indicate a fatal error that requires exiting the game happened.
 */
RETURN_STATUS process_command(command *, game_state *);

/*  
 *  Function:   initialize_game_aux_struct
 *      Functionality:
 *          This command initializes the game_state struct at the start of a new game 
 *          setting the mode to Init, mark errors to 1, and no game_board or moves_list are
 *          valid so valid is 0.
 *      Parameters:
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          void.
 */
void initialize_game_aux_struct(game_state *);

/*  
 *  Function:   free_game_aux_struct
 *      Functionality:
 *          A wrapper for free_game function in game to bridge between Game and Main_Aux. It frees the 
 *          resources allocated withing the game_board/moves_list structs in the game_state struct.
 *      Parameters:
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          void.
 */
void free_game_aux_struct(game_state *); 

/*  
 *  Function:   print_invalid_command
 *      Functionality:
 *          Prints the error message for an invalid command.
 *          Used in this module and in Main_Aux.
 *      Parameters:
 *          none
 *      Return Value:
 *          void.
 */
void print_invalid_command(void);



#endif
