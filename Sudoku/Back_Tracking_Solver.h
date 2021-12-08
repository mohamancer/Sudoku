#ifndef BACK_TRACKING_SOLVER_H_
#define BACK_TRACKING_SOLVER_H_
#include "definitions_db.h"

/**************************************** Interface ****************************************
 *  Interface: Back Tracking Solver
 *  Implemantation: Back_Tracking_Solver.h
 *  Including modules: Game.c
 *  Usage:
 *     This interface offers 2 methods. The first performs the back tracking algorithm
 *     to find the number of solutions of a given board. The second checks if a value can
 *     be inserted into a board at a specific cell, It is an internally used function in 
 *     the implementation and also provided to the Game module for other use cases than
 *     the back tracking algorithm.
 *******************************************************************************************/

/*  
 *  Function:  backTrack
 *      Functionality:
 *          Performs the back tracking algorithm to find the number of solutions
 *          of the board (board) with block dimensions of (block_rows, block_cols)
 *          and updates the pointer given (num_of_solutions) to the number of solutions.
 *          It uses a stack implementatoin of recursive calls.
 *      Parameters:
 *          -   int* num_of_solutions : pointer to the int to contain the number of solutions
 *          -   int ** board    :   pointer to 2-D array containing board
 *          -   int block_rows  :   number of rows in a block 
 *          -   int block_cols  :   number of cols in a block 
 *      Return Value:
 *          returns:
 *              -   ALLOC_FAIL :    if a memory allocation call failed
 *              -   BOARD_SOLVABLE :    if the board is solvable
 *              -   BOARD_UNSOLVABLE :    if the board is unsolvable
 */
RETURN_STATUS backTrack(int* num_of_solutions,int** board, int block_rows, int block_cols);

/*  
 *  Function: is_legal_value
 *      Functionality:
 *          Checks if (value) is a legal value to be inserted to the board at the
 *          cell (row_x, row_y) in board (Board) with block dimensions of 
 *          (block_rows, block_cols) considering the currently filled cells in the
 *          board only. Assumes (Board[row_x][col_y]==0)
 *      Parameters:
 *          -   int ** Board    :   pointer to 2-D array containing board
 *          -   int row_x       :   the x coordinate of the cell    
 *          -   int col_y       :   the y coordinate of the cell    
 *          -   int value       :   the value to be tested in the cell    
 *          -   int block_rows  :   number of rows in a block 
 *          -   int block_cols  :   number of cols in a block 
 *      Return Value:
 *          LEGAL_VALUE if the value can be inserted, otherwise returns NOT_LEGAL_VALUE.
 */
RETURN_STATUS is_legal_value(int ** Board, int row_x, int col_y, int value, int block_rows, int block_cols);

#endif 
