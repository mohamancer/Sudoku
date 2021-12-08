#ifndef MAIX_AUX_H_ 
#define MAIX_AUX_H_ 
#include "definitions_db.h"

/**************************************** Interface ****************************************
 *  Interface: Main Aux
 *  Implemantation: Main_Aux.c
 *  Including modules:  Main.c 
 *  Usage:
 *      This interface is used by the main function to start a new game.
 *******************************************************************************************/

/*  
 *  Function: start_new_game
 *      Functionality:
 *          This function is called by main once, it allocates (statically) memory for the
 *          command and game_state (which contains a game_board and moves_list) structs. 
 *          Memory for structs of these types is allocated only once for the entire game operation,
 *          and it gets statically de-allocated once this function returns.
 *          It initializes the command and game_state structs to default values and calls the 
 *          enter_game_loop function. If it returns SAFE_EXIT indicating exit by the user command
 *          or an EOF has been read, it frees memory allocated within game_state. Otherwise a 
 *          fatal error has happened and the game exits without freeing these resources.
 *      Parameters:
 *          none
 *      Return Value:
 *          void
 */
void start_new_game(void);

#endif
