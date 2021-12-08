#ifndef GAME_IO_IF_H_ 
#define GAME_IO_IF_H_

#include "definitions_db.h"

/**************************************** Interface ****************************************
 *  Interface: Game IO IF
 *  Implemantation: Game.c
 *  Including modules:  File_IO.c 
 *  Usage:
 *      This interface provides the functions to allocate and de-allocate 2-D 
 *      arrays.
 *******************************************************************************************/

/*  
 *  Function: alloc_table
 *      Functionality:
 *          Allocates a 2-D array of dimensions N x N dynamically, sets the pointer given
 *          to it to the array. Allocates an array of N pointers, then allocates an array 
 *          of N integers for each one of the pointers.
 *      Parameters:
 *          -   int ***board_p : pointer to the 2-D array to be allocated
 *          -   int N : the dimension of the array
 *      Return Value:
 *          Returns enums of type RETURN_STATUS:
 *              ALLOC_SUCCESS if the allocation succeeded, otherwise ALLOC_FAIL
 */
RETURN_STATUS alloc_table(int ***board_p, int N);

/*  
 *  Function: free_table
 *      Functionality:
 *          Frees a 2-D array of dimensions N x N dynamically, the array must have been previously 
 *          allocated by a call to alloc_table(.,.).
 *          It frees each one of the N arrays of N integers, then the array of pointers.
 *      Parameters:
 *          -   int **board : 2-D array to be allocated
 *          -   int N : the dimension of the array
 *      Return Value:
 *          void. Assumes that a call to alloc_table(.,.) was made to allocate the array, so a
 *          free is guaranteed to succeed.
 */
void free_table(int **board, int N);

#endif
