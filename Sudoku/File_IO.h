#ifndef FILE_IO_H_
#define FILE_IO_H_
#include "definitions_db.h"

/**************************************** Interface ****************************************
 *  Interface: File IO
 *  Implemantation: File_IO.c
 *  Including modules: Game.c
 *  Usage:
 *      This interface provides the functions to load/store game boards from/to files in
 *      the OS. It uses the game_board struct in definition_db to abstract the game 
 *      board's attributes.
 *******************************************************************************************/
/*  
*  Function: load_game
*      Functionality:
*           Performs loading a game from the file in file_path formatted according to 
*           the provided structure (which is assumed). It loads it into the structure pointed
*           to by g_board_p, and updates the value of fixed cells only if (set_fixed==1).
*           Used by both solve and edit commands.
*           Uses alloc_table from GAME_IO_IF.h to allocate a 2D array .
*      Parameters:
*          -    game_board *g_board_p   : pointer to game_board structure to be filled.
*          -    char *file_path         : file path of a file that contains the puzzle
*          -    int set_fixed           : logical value set to 1 if we want g_board_p->fixed to be filled.
*      Return Value:
*          Returns enum of kind RETURN_STATUS:
*               -   ALLOC_FAIL  :   if a memory allocation failed
*               -   IO_FAIL     :   if the load failed - couldn't open file. 
*               -   IO_SUCCESS  :   if the load was completed successfully 
*/   
RETURN_STATUS load_game(game_board *g_board_p ,char *file_path, int set_fixed);

/*  
*  Function: save_game
*      Functionality:
*           Performs saving a game to a file in file_path formatted according to 
*           the provided structure in the profject document. It saves it from the structure pointed
*           to by g_board_p
*           The function creates a file with the file path and prints to it in the following
*               first line is :    block cols    block rows
*               the other lines are a print of the board rows
*           The function assumes that the game board struct is correct
*      Parameters:
*          -    game_board *g_board_p   : pointer to game_board structure that containd the game.
*          -    char *file_path         : file path of a file to have game saved to
*      Return Value:
*          Returns enum of kind RETURN_STATUS:
*               -   IO_FAIL     :   if the save failed - couldn't open file. 
*               -   IO_SUCCESS  :   if the save was completed successfully 
*/
RETURN_STATUS save_game(game_board *g_board_p ,char *file_path);

#endif
