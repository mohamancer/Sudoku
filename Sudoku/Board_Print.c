#include "Board_Print.h"

/****************************************** Module ******************************************
 *  Module: Board Print
 *  Implements: Board_Print.h
 *  Usage:
 *     This module implements the interface above. It uses two internal functions to print 
 *     seperator rows and cells.
 *******************************************************************************************/

/*  
 *  Function: seperator_row
 *      Functionality:
 *          Prints a row ------- with a length of 4*N+block_rows+1
 *      Parameters:
 *          -   int N : N 
 *          -   int block_rows : block_rows
 *      Return Value:
 *          void
 */
static void seprator_row(int N, int block_rows)
{
	int i;
	for (i=0;i<(4*N+block_rows+1);i++)
		printf("-");
	printf("\n");
}

/*  
 *  Function: cell_row
 *      Functionality:
 *          This function is responsible of printing a single cell. If the board 
 *          in the cell [row][col] is filled we print it in two digits and if a cell is 
 *          fixed we print (.), else if a cell is erroneous and not fixed we print (*).
 *      Parameters:
 *          -   int **board         : the board to be printed
 *          -   int **fixed_tabel   : the board representing the fixed table.
 *          -   int **err_tabel     : the baord representing erroneous values
 *          -   int row             : the x coordinate of the cell to be printed
 *          -   int col             : the y coordinate of the cell to be printed
 *          -   int mark_err        : the mark_errors parameter of the game. 
 *      Return Value:
 *          void
 */
static void cell_row(int **board, int **fixed_tabel, int **err_tabel, int row, int col,int mark_err)
{
	printf(" ");
	if(board[row][col])
		printf("%2d",board[row][col]);
	else printf("  ");
	if (fixed_tabel[row][col] && board[row][col])
		printf(".");
	else if (err_tabel[row][col] && !fixed_tabel[row][col] && mark_err && board[row][col])
		printf("*");
	else printf(" ");
}

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
void board_print(int **board,int **fixed_tabel,int **err_tabel,int block_rows, int block_cols ,int mark_err)
{
	int N,row,col,i;
	N = block_cols*block_rows;
	seprator_row(N, block_rows);
	for(row=0; row<N; row++){
		for(i=0; i<block_rows; i++)
		{
			printf("|");
			for (col=0; col<block_cols; col++)
				cell_row(board,fixed_tabel,err_tabel,row,block_cols*i+col,mark_err);
		}
		printf("|");
		printf("\n");
		if((row+1)%block_rows==0)
			seprator_row(N,block_rows);
	}
}

