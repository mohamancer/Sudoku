#include "File_IO.h"
#include "Game_IO_IF.h"

/****************************************** Module ******************************************
 *  Module: File IO
 *  Implements: File_IO.h
 *  Includes: Game_IO_IF.h
 *  Usage:
 *     This module implements the interface above. It uses the IO interface of the Game 
 *     module for allocations and de-allocationg 2-D arrays.
 *******************************************************************************************/

/*  
*  Function: load_game
*      Functionality:
*           Performs loading a game from the file in file_path formatted according to 
*           the provided structure (which is assumed). It loads it into the structure pointed
*           to by g_board_p, and updates the value of fixed cells only if (set_fixed==1).
*           Used by both solve and edit commands.
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
RETURN_STATUS load_game(game_board *g_board_p ,char *file_path, int set_fixed)
{
	int **board, **fixed_tabel, **err, block_rows, block_cols, N, ch, i, j, empty_cells=0;
	FILE *fp;
	fp = fopen (file_path,"r");
	if (!fp)
		return IO_FAIL;
	if(fscanf(fp,"%d",&block_rows) != 1){
        printf("File %s not formatted correctly!\n", file_path);
        return IO_FAIL;
    }
	if(fscanf(fp,"%d",&block_cols) != 1){
        printf("File %s not formatted correctly!\n", file_path);
        return IO_FAIL;
    }
	N= block_rows * block_cols;
	if(alloc_table(&board,N)==ALLOC_FAIL)
		return ALLOC_FAIL;
	if(alloc_table(&fixed_tabel,N)==ALLOC_FAIL)
		return ALLOC_FAIL;
	if(alloc_table(&err,N)==ALLOC_FAIL)
		return ALLOC_FAIL;

	for (i=0; i<N; i++)
		for(j=0; j<N; j++)
		{
			if(fscanf(fp,"%d",&board[i][j]) != 1){
                printf("File %s not formatted correctly!\n", file_path);
                return IO_FAIL;
            }
            if (board[i][j]==0)
				empty_cells++;
			if (((ch = fgetc(fp)) != EOF))
				if (set_fixed && (ch == '.'))
					fixed_tabel[i][j]=1;
		}

	fclose (fp);
	g_board_p->N= N;
	g_board_p->block_cols= block_cols;
	g_board_p->block_rows= block_rows;
	g_board_p->board= board;
	g_board_p->empty_cells_num= empty_cells;
	g_board_p->err= err;
	g_board_p->fixed= fixed_tabel;
	return IO_SUCCESS;
}

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
RETURN_STATUS save_game(game_board *g_board_p ,char *file_path)
{
	int i,j;
	FILE *fp;
	fp = fopen (file_path,"w");
	if (!fp)
		return IO_FAIL;
	fprintf(fp,"%2d %2d\n", g_board_p->block_rows, g_board_p->block_cols);
	for (i=0; i < g_board_p->N; i++){
		for(j=0; j < g_board_p->N; j++) {
			fprintf(fp, "%2d", g_board_p->board[i][j]);
			if (g_board_p->fixed[i][j])
				fprintf(fp, ".");
			else
				fprintf(fp," ");
			fprintf(fp," ");
		}
		fprintf(fp,"\n");
	}
	fclose (fp);
	return IO_SUCCESS;
}
