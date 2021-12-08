#ifndef ILP_SOLVER_H_ 
#define ILP_SOLVER_H_ 
#include "definitions_db.h" 

/**************************************** Interface ****************************************
 *  Interface: ILP Solver
 *  Implemantation: ILP_Solver.c
 *  Including modules:  Game.c 
 *  Usage:
 *      This function provides the interface to the ILP solving module.
 *      It is used by the Game module for validating boards.
 *******************************************************************************************/

/*  
 *  Function: solve_board_with_ILP
 *      Functionality:
 *          Function to try to solve the sudoku board: board with block size 
 *          of block_rowsXblock_cols using Gurobi. Upadates the board to the 
 *          solved one of it is solvable.
 *      Parameters:
 *          -   int **board     :   the board to be solved
 *          -   int block_rows  :   the number of rows in a block 
 *          -   int block_cols  :   the number of cols in a block 
 *      Return Value:
 *          returns enum of type RETURN_STATUS:
 *              BOARD_SOLVABLE - when board is solvable, then board gets updated
 *              BOARD_UNSOLVABLE - when board is unsolvable, then board is unchanged
 *              ALLOC_FAIL - when an allocation fails
 *              ILP_FAIL -  when an operation on the gurobi optimizer failed, we consider this to be
 *                          a fatal error because we don't know how much memory Gurobi has allocated,
 *                          which is usually a lot (~0.5GB for moderate dimension board).
 */
RETURN_STATUS solve_board_with_ILP(int **board, int block_rows, int block_cols);

#endif
