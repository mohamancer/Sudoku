#ifndef BOARD_PRINT_H_
#define BOARD_PRINT_H_
#include "definitions_db.h"
/**************************************** Interface ****************************************
 *  Interface: Board Print
 *  Implemantation: Board_Print.c
 *  Including modules: Game.c
 *  Usage:
 *      This interface provides the function to print the sudoku board as described in
 *      the project file.
 *******************************************************************************************/

/*  
 *  Function: board_print 
 *      Functionality:
 *          prints the board in the following order:
 * 	            1. Separator row
 * 	            2. Repeat block_rows times for each row of blocks:
 *		            a. Cell row
 *			            i. Repeat block_cols times for each of the row of cells
 *		            b. Separator row
 *      Parameters:
 *          -   int **board         : the board to be printed
 *          -   int **fixed_tabel   : the board representing the fixed table.
 *          -   int **err_tabel     : the baord representing erroneous values
 *          -   int block_rows      : the number of rows in a block
 *          -   int block_cols      : the number of cols in a block
 *          -   int mark_err        : the mark_errors parameter of the game. 
 *      Return Value:
 *          void
 */
void board_print(int **Board,int **fixed_tabel,int **err_tabel,int n, int m,int mark_err);

#endif
