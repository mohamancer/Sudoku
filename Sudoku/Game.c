#include "Game_IO_IF.h"
#include "Game_IF.h"
#include "Moves_Linked_List.h"
#include "Board_Print.h"
#include "File_IO.h"
#include "ILP_Solver.h"
#include "LP_Solver.h"
#include "Back_Tracking_Solver.h"
#include <math.h>

/****************************************** Module ******************************************
 *  Module: Game
 *  Implements: Game_IO_IF.h Game_IF.h
 *  Includes: Moves_Linked_List.h Board_Print.h File_IO.h ILP_Solver.h Back_Tracking_Solver.h
 *  Usage:
 *      This modules implements almost all the functionality of processing the commands
 *      from the user. A large portion of the functionality is already provided in the 
 *      included modules. These functions get called by the Game Operator after parameter
 *      checking and they return whether the processing succeeded or failed.
 *      This module includes static functions used internally for logical partitioning
 *      for the functions.
 *      It uses a lot of the structures defined in the data base.
 *      READ the definitions_db.h documentation first to understand the struct used.
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
RETURN_STATUS alloc_table(int ***board_p, int N)
{
    int **board, i;
    board = (int **) malloc(N * sizeof(int *));
    if(!board){
        return ALLOC_FAIL;
    }
    for (i=0; i<N; i++){
         board[i] = (int *) calloc(N, sizeof(int));
         if(!board[i]){
             return ALLOC_FAIL;
        }
    }
    *board_p = board;
    return ALLOC_SUCCESS;
}

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
void free_table(int **board, int N)
{
    int i;
    for (i=0; i<N; i++)
        free(board[i]); 
    free(board);
    return;
}

/*  
 *  Function: copy_table
 *      Functionality:
 *          Copies the 2-D board src to the 2-D board dst.
 *      Parameters:
 *          -   int **dst : board that get copied to
 *          -   int **src : board thet gets copied from
 *          -   int N : the dimension of the table
 *      Return Value:
 *          void
 */
static void copy_table(int **dst, int **src, int N)
{
    int i, j;
    for(i=0; i<N; i++)
        for(j=0; j<N; j++)
            dst[i][j] = src[i][j];
    return;
}


/*  
 *  Function: free_game
 *      Functionality:
 *          Initializes the game_state struct to default_values.
 *          Frees the game_board and moves_list structures inside the game_state structure
 *          pointed to by the function argument.
 *          It frees the 3 2-D tables within the game_board and the resources allocated within
 *          the moves_list only if the struct is valid (g_state_p->valid == 1)
 *      Parameters:
 *          -   game_state *g_state_p : pointer to the game_state struct to be freed of resources.
 *      Return Value:
 *          void. 
 */
void free_game(game_state *g_state_p)
{
    if(g_state_p->valid){
       free_table(g_state_p->g_board.board, g_state_p->g_board.N); 
       free_table(g_state_p->g_board.err, g_state_p->g_board.N); 
       free_table(g_state_p->g_board.fixed, g_state_p->g_board.N); 
       free_moves_list(&(g_state_p->m_list));
    }
    g_state_p->mode =  Init;
    g_state_p->valid = 0;
    return;
}

/*  
 *  Function: update_erroneous
 *      Functionality:
 *          The function updates the err table in the game_board structure.
 *          We don't always update the err table of the game_board structure, 
 *          only when needed we call this function.
 *          And returns if there is an erroneous cell or not.
 *          It uses the is_legal_value function in Back_Tracking_Solver.h
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure
 *      Return Value:
 *          returns 1 if there is an erroneous cell in the board, otherwise returns 0.
 */
static int update_erroneous(game_state *g_state_p)
{
    int is_erroneous, temp, i, j, N, **board, **err, **fixed;
    is_erroneous = 0;
    N = g_state_p->g_board.N;
    board = g_state_p->g_board.board;
    fixed = g_state_p->g_board.fixed;
    err = g_state_p->g_board.err;
    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            err[i][j] = 0;
            temp = board[i][j];
            if(temp){ /* cell is filled */
                board[i][j] = 0;
                if(is_legal_value(board, i, j, temp, g_state_p->g_board.block_rows, g_state_p->g_board.block_cols) == NOT_LEGAL_VALUE) {
                    is_erroneous = 1;
                    if(!fixed[i][j])
                        err[i][j] = 1;
                }
                board[i][j] = temp;
            }
        }
    }
    return is_erroneous;
}

/*  
 *  Function: solve 
 *      Functionality:
 *         Frees allocated resources that are no longer needed.
 *         Starts a puzzle in Solve mode, from the puzzle in file_path, updates the game_state
 *         struct and initializes it.
 *      Parameters:
 *          -   game_state *g_state_p : pointer to game_state struct to have game loaded into
 *          -   char *file_path :   string that contains the file path to the board to be 
 *                                  loaded into the game.
 *      Return Value:
 *          PROCESS_SUCCESS if no fatal errors happened,
 *          otherwise returns PROCESS_FAIL indicating a fatal error that requires exiting the game
 *          and prints corresponding messages.
 */
RETURN_STATUS solve(game_state *g_state_p, char *file_path)
{
    RETURN_STATUS status;
    game_board dummy_g_board;
    status = load_game(&dummy_g_board, file_path, 1);
    if(status == IO_FAIL) {
        printf("Error: File doesn't exist or cannot be opened\n");
        return PROCESS_SUCCESS;
    } else if(status == ALLOC_FAIL) {
        printf("Couldn't allocate space for boards when loading the board from file:%s\n", file_path);
        printf("Exiting game without freeing resources...\n");
        return PROCESS_FAIL;
    } else {
        free_game(g_state_p);
        g_state_p->g_board = dummy_g_board;
        init_moves_list(&(g_state_p->m_list));
        g_state_p->mode = Solve;
        g_state_p->valid = 1;
        print_board(g_state_p);
        return PROCESS_SUCCESS; 
    }
}

/*  
 *  Function: edit 
 *      Functionality:
 *         Frees allocated resources that are no longer needed.
 *         Starts a puzzle in Edit mode, from the puzzle in file_path if it exists,
 *         updates the game_state struct and initializes it.
 *      Parameters:
 *          -   game_state *g_state_p : pointer to game_state struct to have game loaded into
 *          -   char *file_path :   string that contains the file path to the board to be 
 *                                  loaded into the game, if NULL then a default board gets 
 *                                  built.
 *      Return Value:
 *          PROCESS_SUCCESS if no fatal errors happened,
 *          otherwise returns PROCESS_FAIL indicating a fatal error that requires exiting the game
 *          and prints corresponding messages.
 */
RETURN_STATUS edit(game_state *g_state_p, char *file_path)
{
    RETURN_STATUS status;
    game_board dummy_g_board;
    if(file_path) {
        /* A file path has been given in the command */
        status = load_game(&dummy_g_board, file_path, 0);
        if(status == IO_FAIL) {
            printf("Error: File cannot be opened\n");
            return PROCESS_SUCCESS;
        } else if(status == ALLOC_FAIL) {
            printf("Couldn't allocate space for boards when loading the board from file:%s\n", file_path);
            printf("Exiting game without freeing resources...\n");
            return PROCESS_FAIL;
        }
    } else {
        /*  No file path has been given in the command
         *  Filling the game_board structure with default values.
         */
        dummy_g_board.N = 9;
        dummy_g_board.block_rows = 3;
        dummy_g_board.block_cols = 3;
        dummy_g_board.empty_cells_num = 81;
        status = alloc_table(&(dummy_g_board.board), 9);
        if(status != ALLOC_SUCCESS){
            printf("Couldn't allocate space for boards when building a default size board\n");
            printf("Exiting game without freeing resources...\n");
            return PROCESS_FAIL;
        } 
        status = alloc_table(&(dummy_g_board.err), 9);
        if(status != ALLOC_SUCCESS){
            printf("Couldn't allocate space for boards when building a default size board\n");
            printf("Exiting game without freeing resources...\n");
            return PROCESS_FAIL;
        }  
        status = alloc_table(&(dummy_g_board.fixed), 9);
        if(status != ALLOC_SUCCESS){
            printf("Couldn't allocate space for boards when building a default size board\n");
            printf("Exiting game without freeing resources...\n");
            return PROCESS_FAIL;
        }  
    }
    /* updating game_state structure */
    free_game(g_state_p);
    g_state_p->g_board = dummy_g_board;
    init_moves_list(&(g_state_p->m_list));
    g_state_p->mode = Edit;
    g_state_p->valid = 1;
    print_board(g_state_p);
    return PROCESS_SUCCESS; 
}

/*  
 *  Function: print_board
 *      Functionality:
 *          Prints the current board, uses the function in Board_Print.h
 *          Uses the algorithm presented in the project document.
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure that contains the board to be printed
 *      Return Value:
 *          void
 */
void print_board(game_state *g_state_p)
{
    int eff_mark_errors;
    
    eff_mark_errors = g_state_p->mode==Solve ? g_state_p->mark_errors : 1;
    if(eff_mark_errors)
        (void) update_erroneous(g_state_p);

    board_print(g_state_p->g_board.board,
                g_state_p->g_board.fixed,
                g_state_p->g_board.err,
                g_state_p->g_board.block_rows,
                g_state_p->g_board.block_cols,
                eff_mark_errors
                );
    return;
}

/*  
 *  Function: set_cell
 *      Functionality:
 *          Sets the cell in the board to value z2 in the board at cell with row number:cell_rows
 *          and column number:cell_cols. It updates the empty_cells_num in the game_board structure
 *          accordingly. And adds the change to the redo/undo list where the needed changes are made
 *          to the list as described in the functions called from Moves_Linked_List.h
 *      Parameters:
 *          -   game_state *g_state_p : the game_state struct
 *          -   int cell_rows : the row of the cell to be changed
 *          -   int cell_cols : the column of the cell to be changed
 *          -   int z2 : the value to be put in the cell
 *      Return Value:
 *          Returns ALLOC_SUCCESS if it succeeded to add the change to the board and to the moves list.
 *          otherwise returns ALLOC_FAIL indicating a fatal error
 */
static RETURN_STATUS set_cell(game_state *g_state_p, int cell_rows, int cell_cols, int z2)
{
    int before_filled, after_filled, z1, **board;
    move_node *m_node_p;
    RETURN_STATUS status;
    board = g_state_p->g_board.board;
    z1 = board[cell_rows][cell_cols];
    before_filled = (z1 != 0);
    after_filled = (z2 != 0);
    status = alloc_move_node(&m_node_p);
    if(status == ALLOC_FAIL){
        return ALLOC_FAIL;
    }
    status = add_change_to_move(m_node_p, cell_rows, cell_cols, z1, z2);
    if(status == ALLOC_FAIL){
        return ALLOC_FAIL;
    }
    add_move_node_to_list(&(g_state_p->m_list), m_node_p);
    board[cell_rows][cell_cols] = z2;
    g_state_p->g_board.empty_cells_num -= (after_filled - before_filled);
    return ALLOC_SUCCESS;
}

/*  
 *  Function: is_valid_board
 *      Functionality:
 *          Performs the validation of the current board, copies the board to an 
 *          auxiliary one, running the ILP solver on it, then checking the return value 
 *          of the function, and freeing the auxiliary board.
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure which contains the board to be 
 *                                      validated.
 *      Return Value:
 *          -   PROCESS_FAIL : if an error in allocation or Gurobi happened.
 *          -   BOARD_SOLVABLE : if the board is solvable
 *          -   BOARD_UNSOLVABLE : if the board is unsolvable
 */
static RETURN_STATUS is_valid_board(game_state *g_state_p)
{
    int **aux_board, N;
    RETURN_STATUS status;
    N = g_state_p->g_board.N;
    status = alloc_table(&aux_board, N);
    if(status == ALLOC_FAIL)
        return PROCESS_FAIL;
    copy_table(aux_board, g_state_p->g_board.board, N);
    status = solve_board_with_ILP(aux_board, g_state_p->g_board.block_rows, g_state_p->g_board.block_cols);
    if((status == ALLOC_FAIL)||(status == ILP_FAIL)) {
        return PROCESS_FAIL;
    } /* status is either BOARD_SOLVABLE or BOARD_UNSOLVABLE */ 
    free_table(aux_board, N);
    return status;
}


/*  
 *  Function: set
 *      Functionality:
 *          Performs the set operation after the parameters have been checked to be
 *          in the legal range in the process_command in the Game_Operator.
 *          Performs the steps described in the project document. And adds the change 
 *          to the moves_list.
 *      Parameters:
 *          -   game_state *g_state_p : game_state struct to be updated
 *          -   int x : the x in the command, in legal range.
 *          -   int y : the y in the command, in legal range.
 *          -   int z : the z in the command, in legal range.
 *      Return Value:
 *          Returns PROCESS_SUCCESS if it succeeded to perform the command without fatal errors.
 *          otherwise returns PROCESS_FAIL indicating a fatal error
 */
RETURN_STATUS set(game_state *g_state_p, int x, int y, int z)
{
    int cell_rows, cell_cols;
    RETURN_STATUS status;
    cell_rows = y-1;
    cell_cols = x-1;
    /* In EDIT mode, the fixed table is cleared because all cells can be changed */
    if((g_state_p->g_board.fixed)[cell_rows][cell_cols]){
        printf("Error: cell is fixed\n"); 
        return PROCESS_SUCCESS;
    }
    status = set_cell(g_state_p, cell_rows, cell_cols, z);
    if(status == ALLOC_FAIL){
        printf("Couldn't allocate space for the change in the redo/undo list\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    print_board(g_state_p);
    if((g_state_p->mode == Solve) && (g_state_p->g_board.empty_cells_num == 0)){
        status = is_valid_board(g_state_p);
        if(status == PROCESS_FAIL) {
            printf("Fatal error while validating board after all cells were filled.\n");
            printf("Exiting program without freeing resources..\n");
            return PROCESS_FAIL; 
        } else if(status == BOARD_UNSOLVABLE) {
            printf("Puzzle solution erroneous\n");
        } else { /* status == BOARD_SOLVABLE*/
            printf("Puzzle solved successfully\n");
            free_game(g_state_p);
        }
    }
    return PROCESS_SUCCESS;
}

/*  
 *  Function: validate
 *      Functionality:
 *          Validates that the current board is solvable using ILP. It uses the function is_valid_board defined above 
 *          for running the ILP. It checks for erroneous values first, and reports if such are found.
 *      Parameters:
 *          -   game_state *g_state_p : game_state struct that contains board to be validated.
 *      Return Value:
 *          Returns PROCESS_SUCCESS if it succeeded to perform the command without fatal errors.
 *          otherwise returns PROCESS_FAIL indicating a fatal error
 */
RETURN_STATUS validate(game_state *g_state_p)
{
    int is_erroneous;
    RETURN_STATUS status;
    is_erroneous = update_erroneous(g_state_p);
    if(is_erroneous) {
        printf("Error: board contains erroneous values\n");
        return PROCESS_SUCCESS;
    }
    status = is_valid_board(g_state_p);
    if(status == PROCESS_FAIL) {
        printf("Fatal error while validating board.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL; 
    } else if(status == BOARD_UNSOLVABLE) {
        printf("Validation failed: board is unsolvable\n");
    } else { /* status == BOARD_SOLVABLE*/
        printf("Validation passed: board is solvable\n");
    }
    return PROCESS_SUCCESS;
}

/*
 *  Function: comapre
 *      Functionality: compare doubles for qsort to use
 */
static int compare (const void * a, const void * b)
{
  if (*(double*)a > *(double*)b) return 1;
  else if (*(double*)a < *(double*)b) return -1;
  else return 0;  
}

/*
 *  Function: random_value
 *      Functionality:
 *          uses the array of scores, after sorting it, to return a random cell value 
 *      Parameters:
 *          - double *arr : the solution array
 *          - int n : length of arr 
 *      Return Value:
 *          return the guess to be set on the board
 */
int random_value(double *arr,double *p_arr, int n){
	double sum,random_value,frac,integ;
	const void *a,*b;
	int i,j;
	sum=0;
	for (i=0;i<n;i++){
		sum+=arr[i];
	}
	
	qsort(arr, n, sizeof(double), compare);
	random_value = ((double)rand() / (double)RAND_MAX)*sum;
	for (i=0;i<n;i++){
		a=&random_value;
		b=&(arr[i]);
		if (compare(a,b)== -1) {
			for (j=0;j<n;j++){
				frac=modf(p_arr[j],&integ);
				a=&frac;
				b=&(arr[i]);
				if (compare(a,b)==0) return j+1; 
			}
		}
		else random_value-=arr[i];
	}
	return 0;
	
}

/*  
 *  Function: random_guess
 *      Functionality:
 *          uses the solution array and makes it an array of length N for a single cell
 *          then uses random_number to assign a random member of it to the board if it satisfies 
 *          all conditions in the project pdf 
 *      Parameters:
 *          -   int block_cols : number of columns in block;
 *          -   int block_rows : number of rows in block;
 *          -   double *sol : all cells and their scores (from LP solution)
 *          -   double x : threshold 
 *      Return Value:
 *           Returns PROCESS_SUCCESS if it succeeded to perform the command without fatal errors.
 *          otherwise returns PROCESS_FAIL indicating a fatal error
 */
static RETURN_STATUS random_guess(int **board,int block_rows,int block_cols, double *sol,double x){
	int i,j,k,N,cnt;
	double *tmp,*p_tmp;
        N=block_cols*block_rows;
	cnt=0;

	for(i=0; i<N; i++){
        for(j=0; j<N; j++){
			cnt=0;
			tmp = calloc(sizeof(double),N);
			p_tmp= calloc(sizeof(double),N);
			if(tmp == NULL || p_tmp==NULL){
				printf("allocation error in random_guess\n");
				return ALLOC_FAIL;
			}
				for(k=0; k<N; k++){
					
					if(sol[i*N*N+j*N+k] >= x && board[i][j]==0 
						&& is_legal_value(board,i,j,k+1,block_rows,block_cols)==LEGAL_VALUE){
						cnt++;
						tmp[k]=sol[i*N*N+j*N+k];
						p_tmp[k]=sol[i*N*N+j*N+k]+k;
					}
				
				}
				if (cnt==0){
					free(p_tmp);
					free(tmp);
					continue;
				}
				else if (cnt==1) {
					for (k=0;k<N;k++){
						if (tmp[k]>0) board[i][j]=k+1;
					}
				}
				else{
					board[i][j]=random_value(tmp,p_tmp, N);
				}
			free(p_tmp);
			free(tmp);
		}
	}
	return PROCESS_SUCCESS;
}
/*  
 *  Function: guess
 *      Functionality:
 *          guess a board soultion (or part of it) using LP,
 *      Parameters:
 *          -   game_state *g_state_p : game_state struct that contains board to be filled.
 *          -   double x : threshold 
 *      Return Value:
 *          Returns PROCESS_SUCCESS if it succeeded to perform the command without fatal errors.
 *          otherwise returns PROCESS_FAIL indicating a fatal error
 */
RETURN_STATUS guess(game_state *g_state_p, double x)
{
	int N, num_of_variables, cell_rows,cell_cols, block_rows, block_cols, is_erroneous, **board,**old_board;
    RETURN_STATUS status;
	move_node *m_node_p;
	double *sol;
	block_rows=g_state_p->g_board.block_rows;
	block_cols=g_state_p->g_board.block_cols;
    	N = block_rows*block_cols;
    num_of_variables = N*N*N;
	status = alloc_table(&old_board, N);
    if(status == ALLOC_FAIL){
        printf("Fatal error while performing guess.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    copy_table(old_board, g_state_p->g_board.board, g_state_p->g_board.N);
    is_erroneous = update_erroneous(g_state_p);
    if(is_erroneous) {
        printf("Error: board contains erroneous values\n");
        return PROCESS_SUCCESS;
    }
    sol = malloc(sizeof(double)*num_of_variables);
	if(sol == NULL){
        printf("allocation error in guess\n");
        return ALLOC_FAIL;
    }
	status = alloc_table(&board, g_state_p->g_board.N);
    if(status == ALLOC_FAIL){
        printf("Fatal error while in guess\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    copy_table(board, g_state_p->g_board.board, g_state_p->g_board.N);
    status = solve_board_with_LP(board, g_state_p->g_board.block_rows, g_state_p->g_board.block_cols,sol);
    if((status == ALLOC_FAIL)||(status == LP_FAIL)) {
        printf("Fatal error while in guess.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    } else {
	random_guess(g_state_p->g_board.board,block_rows,block_cols,sol,x);
		print_board(g_state_p);	
	}
	/* adding the move to the moves list as one move with multiple changes*/
        status = alloc_move_node(&m_node_p);
        if(status == ALLOC_FAIL){
            printf("Fatal error while performing generate.\n");
            printf("Exiting program without freeing resources..\n");
            return PROCESS_FAIL;
        }
	for(cell_rows=0; cell_rows<N; cell_rows++){
            for(cell_cols=0; cell_cols<N; cell_cols++){
                if(!(old_board[cell_rows][cell_cols] ) && (g_state_p->g_board.board[cell_rows][cell_cols]) ){
                    status = add_change_to_move(m_node_p, cell_rows, cell_cols, old_board[cell_rows][cell_cols], (g_state_p->g_board.board[cell_rows][cell_cols]));
                    if(status == ALLOC_FAIL){
                        printf("Fatal error while performing guess.\n");
                        printf("Exiting program without freeing resources..\n");
                        return PROCESS_FAIL;
                    }
                }
            }
        }
	add_move_node_to_list(&(g_state_p->m_list), m_node_p);
	free(sol);
    free_table(old_board, N);
	free_table(board, N);
    return PROCESS_SUCCESS;
}

/*  
 *  Function: find_all_legal_values
 *      Functionality:
 *          Function receives a board and a cell, it finds all legal values that could be put into cell,
 *          using the function is_legal_value from Back_Tracking_Solver.h
 *          it puts them in the preallocated array legal_values_list of size N at least and returns 
 *          their number.
 *      Parameters:
 *          -   int **board : board that contains the wanted cell
 *          -   int cell_rows : the row of the cell.
 *          -   int cell_cols : the column of the cell.
 *          -   int block_rows : the number of rows in a block
 *          -   int block_cols : the number of columns in a block
 *          -   int *legal_values_list : a pre allocated array of size N (block_rows*block_cols) at least
 *      Return Value:
 *          The number of legal values that can be put in the cell
 */
static int find_all_legal_values(int **board, int cell_rows, int cell_cols, int block_rows, int block_cols, int *legal_values_list)
{
    int num_legal_values, val, tmp, N;
    N = block_rows*block_cols;
    num_legal_values = 0;
    tmp = board[cell_rows][cell_cols];

    board[cell_rows][cell_cols] = 0;
    for(val=1; val<=N; val++){
        if(is_legal_value(board, cell_rows, cell_cols, val, block_rows, block_cols) == LEGAL_VALUE) {
            legal_values_list[num_legal_values] = val;
            num_legal_values++;
        }
    }
    board[cell_rows][cell_cols] = tmp;
    return num_legal_values;
}

/*  
 *  Function: randomly_extract_a_cell
 *      Functionality:
 *          Receives an array of arrays of integers of length 2, and a pointer to 
 *          an array of length 2 (at_least). It extracts a random cell from the list
 *          ,deletes it rearranges the list. It sets the pointer to the chosen cell and returns the number
 *          of empty cells after extracting. Assumes len>0
 *      Parameters:
 *          -   int (*cells_list)[2] : array of int [2] of length len
 *          -   int len : length of the cells_list - must be > 0
 *          -   int *cell : the pointer to which the extracted cell gets put into. 
 *      Return Value:
 *          returns len-1.
 */
static int randomly_extract_a_cell(int (*cells_list)[2], int len, int *cell)
{
    int chosen_indx;
    chosen_indx = rand()%len;
    cell[0] = cells_list[chosen_indx][0];
    cell[1] = cells_list[chosen_indx][1];
    if(chosen_indx<len-1){
        cells_list[chosen_indx][0] = cells_list[len-1][0];
        cells_list[chosen_indx][1] = cells_list[len-1][1];
    }
    return len-1;
}

/*  
 *  Function: randomly_fill_a_legal_board
 *      Functionality:
 *          Function receives a 2-D array -board- that shall be empty, it tries to fill it with x
 *          legal values if x>0, otherwise returns. It does so by randomly choosing an empty cell from the empty cells pool 
 *          (using the function randomly_extract_a_cell define above).
 *          It chooses a random value from the available random legal values for that cell (that it gets 
 *          by calling the function find_all_legal_values defined above) and sets it. 
 *          If it succeeds to do this for x cells it returns FILL_SUCCESS, if one of the cells
 *          turned out to have no legal values it returns FILL_FAIL.
 *          If a fatal error happened it returns PROCESS_FAIL.
 *      Parameters:
 *          -   int **board : the board to be partially filled 
 *          -   int x : number of cells to be filled 
 *          -   int block_rows : number of rows in one block
 *          -   int block_cols : number of columns in one block

 *      Return Value:
 *          Returns an enum of type RETURN_STATUS, one of the following: 
 *              - PROCESS_FAIL : if a fatal error happened - in allocation
 *              - FILL_SUCCESS : if it succeeded to fill the board, then board is updated to contain
 *                                  the filled cells.
 *              - FILL_FAIL :   if it couldn't fill the board. 
 */
static RETURN_STATUS randomly_fill_a_legal_board(int **board, int x, int block_rows, int block_cols)
{
    int N, cell[2], val, i, j, cnt;
    int *legal_values_list, num_legal_values;
    int (*legal_cells_list)[2], empty_cells_num;
    N = block_rows*block_cols;
    empty_cells_num = N*N;

    /* If x is 0, there is no need to fill anything */
    if(!x){
        return FILL_SUCCESS;
    }
    legal_values_list = (int *) calloc(N, sizeof(int));
    if(!legal_values_list)
        return PROCESS_FAIL;
    legal_cells_list = calloc(empty_cells_num, sizeof(int [2]));
    if(!legal_cells_list)
        return PROCESS_FAIL;
    /* Filling legal_cells_list which contains all empty cells in the board */
    cnt = 0;
    for(i=0; i<N; i++)
        for(j=0; j<N; j++){
            legal_cells_list[cnt][0] = i;
            legal_cells_list[cnt][1] = j;
            cnt++;
        }
    /* Filling x cells */
    while (x) {
        empty_cells_num = randomly_extract_a_cell(legal_cells_list, empty_cells_num, &(cell[0]));
        num_legal_values = find_all_legal_values(board, cell[0], cell[1], block_rows, block_cols, legal_values_list);
        if(!num_legal_values) {
            free(legal_values_list);
            free(legal_cells_list);
            return FILL_FAIL;
        }
        val = legal_values_list[rand()%num_legal_values];
        board[cell[0]][cell[1]] = val;
        x--;
    }
    free(legal_values_list);
    free(legal_cells_list);
    return FILL_SUCCESS; 
}


/*  
 *  Function: randomly_generate_board
 *      Functionality:
 *          Generates a board by randomly filling x cells with random legal values, using function
 *          randomly_fill_a_legal_board defined above. Running ILP to solve the resulting board,
 *          and returning with the board solved and completely filled if possible (see return values)
 *          remembers board state and returns to it on failure
 *      
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure which contains the board to be generated into
 *          -   int x : number of cells to be filled at start
 *      Return Value:
 *          Returns an enum of type RETURN_STATUS, one of the following: 
 *              - PROCESS_FAIL : if a fatal error happened - in ILP or allocation
 *              - GENERATE_SUCCESS : if succeeded to generate a board, then g_state_p->g_board.board
 *                                      is the filled board which is solved.
 *              - GENERATE_FAIL :   if couldn't generate a solved board by filling x cells with legal random 
 *                                  values and trying to solve it within the given maximal number of tries, which 
 *                                  is 1000. (indicating x is too high in respect to N*N)
 */
static RETURN_STATUS randomly_generate_board(game_state *g_state_p, int x)
{
    int maximal_num_of_tries, **board,**old_board;
    RETURN_STATUS status;
    /* Value from the project document */
    maximal_num_of_tries = 1000;
    board = g_state_p->g_board.board;
    status = alloc_table(&old_board, g_state_p->g_board.N);
    if(status == ALLOC_FAIL){
        printf("Fatal error while performing generate.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    copy_table(old_board, g_state_p->g_board.board, g_state_p->g_board.N);
    while(maximal_num_of_tries) {
        status = randomly_fill_a_legal_board(board, x, g_state_p->g_board.block_rows, g_state_p->g_board.block_cols);
        if(status == PROCESS_FAIL) {
			free_table(old_board, g_state_p->g_board.N);
            return status;
        } else if (status == FILL_SUCCESS) {
            status = solve_board_with_ILP(board, g_state_p->g_board.block_rows, g_state_p->g_board.block_cols);
            if((status == ALLOC_FAIL)||(status == ILP_FAIL)) {
				free_table(old_board, g_state_p->g_board.N);
                return PROCESS_FAIL;
            } else if(status == BOARD_SOLVABLE) {
                /* The board is solved and it's already filled in the game_board structure */
				free_table(old_board, g_state_p->g_board.N);
                return GENERATE_SUCCESS;
            }/* otherwise status == BOARD_UNSOLVABLE : filled a board legally but turned out unsolvable */
        } /* otherwise status == FILL_FAIL : couldn't fill board*/
        copy_table(board, old_board, g_state_p->g_board.N);
        maximal_num_of_tries--;
    } 
    free_table(old_board, g_state_p->g_board.N);
    return GENERATE_FAIL;
}
           
/*  
 *  Function: partially_clear_board
 *      Functionality:
 *          Function receives a full board, it chooses cells randomly and clears them, until y filled
 *          cells remain. If no cells need to be clear it just returns.
 *      Parameters:
 *          -   int **board : the board to be partially cleared
 *          -   int y : number of filled cells at the end
 *          -   int N : dimension of the board
 *      Return Value:
 *          Returns PROCESS_SUCCESS if it succeeded to clear the board partially without fatal errors.
 *          otherwise returns PROCESS_FAIL indicating a fatal error in allocation
 */
static RETURN_STATUS partially_clear_board(int **board, int y, int N)
{
    int cell[2], i, j, cnt, num_empty_cells;
    int (*legal_cells_list)[2], num_filled_cells;
    num_filled_cells = N*N;
    num_empty_cells = num_filled_cells-y;

    /* If y==N*N, there is no need to clear any cell */
    if(y == num_filled_cells)
        return PROCESS_SUCCESS;
            
    legal_cells_list = calloc(num_filled_cells, sizeof(int [2]));
    if(!legal_cells_list)
        return PROCESS_FAIL;
    /* Filling legal_cells_list which contains all filled cells in the board */
    cnt = 0;
    for(i=0; i<N; i++)
        for(j=0; j<N; j++){
            legal_cells_list[cnt][0] = i;
            legal_cells_list[cnt][1] = j;
            cnt++;
        }
    /* Choosing y cells to keep */
    while(y){
        num_filled_cells = randomly_extract_a_cell(legal_cells_list, num_filled_cells, &(cell[0]));
        y--;
    }
    /* Clearing all cells that hasn't been chosen */
    for(i=0; i<num_empty_cells; i++){
        board[legal_cells_list[i][0]][legal_cells_list[i][1]] = 0;
    }

    free(legal_cells_list);
    return PROCESS_SUCCESS;
}

/*  
 *  Function:  generate
 *      Functionality:
 *          Checks first if number of empty cells is less than x, if not issues an error message, otherwise it
 *          generates a puzzle by randomly filling x cells with random legal values, running ILP
 *          to solve the resulting board, all by calling the function randomly_generate_board defined above.
 *          And then clearing all but y random cells, by calling the function partially_clear_board
 *          defined above. At the end if it succeeded it prints the board. And adds the move to the redo/undo list
 *          x and y are guaranteed to be in legal range by the Game_Operator before calling this function.
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure which contains the board.
 *          -   int x : number of cells to be filled at start
 *          -   int y : number of filled cells after generating a solvable board
 *      Return Value:
 *          Returns PROCESS_SUCCESS if it succeeded to perform the command without fatal errors.
 *          otherwise returns PROCESS_FAIL indicating a fatal error
 */
RETURN_STATUS generate(game_state *g_state_p, int x, int y)
{
    int N, cell_rows, cell_cols, **old_board;
    RETURN_STATUS status;
    move_node *m_node_p;
    N = g_state_p->g_board.N;
	
	status = alloc_table(&old_board, N);
    if(status == ALLOC_FAIL){
        printf("Fatal error while performing generate.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    if(g_state_p->g_board.empty_cells_num < x) {
        printf("Error: number of empty cells too low\n");
        return PROCESS_SUCCESS;
    }
	copy_table(old_board, g_state_p->g_board.board, N);
    status = randomly_generate_board(g_state_p, x);
    if(status == PROCESS_FAIL) {
        printf("Fatal error while generating board.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL; 
    } else if(status == GENERATE_FAIL) {
        printf("Error: puzzle generator failed\n");
        return PROCESS_SUCCESS;
    } else { /* status == GENERATE_SUCCESS */
        status = partially_clear_board(g_state_p->g_board.board, y , g_state_p->g_board.N);
        if(status == PROCESS_FAIL) {
            printf("Fatal error while clearing board.\n");
            printf("Exiting program without freeing resources..\n");
            return PROCESS_FAIL; 
        } /* status == PROCESS_SUCCESS : managed to clear board partially */
        /* adding the move to the moves list as one move with multiple changes*/
        status = alloc_move_node(&m_node_p);
        if(status == ALLOC_FAIL){
            printf("Fatal error while performing generate.\n");
            printf("Exiting program without freeing resources..\n");
            return PROCESS_FAIL;
        }
		
        for(cell_rows=0; cell_rows<N; cell_rows++){
            for(cell_cols=0; cell_cols<N; cell_cols++){
                if((g_state_p->g_board.board)[cell_rows][cell_cols] || old_board[cell_rows][cell_cols]){
                    status = add_change_to_move(m_node_p, cell_rows, cell_cols, old_board[cell_rows][cell_cols], (g_state_p->g_board.board[cell_rows][cell_cols]));
                    if(status == ALLOC_FAIL){
                        printf("Fatal error while performing generate.\n");
                        printf("Exiting program without freeing resources..\n");
                        return PROCESS_FAIL;
                    }
                }
            }
        }
        if(y>0) { /* At least one change has been made */
            add_move_node_to_list(&(g_state_p->m_list), m_node_p);
        } else { /* No change has been made */
            free_move_node(m_node_p);
        }
        print_board(g_state_p);
        g_state_p->g_board.empty_cells_num = N*N-y;
		free_table(old_board, g_state_p->g_board.N);
        return PROCESS_SUCCESS;
    }
}

/*  
 *  Function: change_cell
 *      Functionality:
 *          function used by redo/undo commands to change the value of a cell.
 *          It checks that the "before" value is consistent with the current board.
 *          It is used to maintain the empty_cells_num in the game_board.
 *      Parameters:
 *          -   game_state *g_state_p : game_state structure that contains board.
 *          -   int cell_rows : the row of the cell to be changed
 *          -   int cell_cols : the column of the cell to be changed
 *          -   int from : the current value in the cell
 *          -   int to : the value getting put into the cell
 *      Return Value:
 *          void
 */
static void change_cell(game_state *g_state_p, int cell_rows, int cell_cols, int from, int to)
{
    int before_filled, after_filled;
    before_filled = (from != 0);
    after_filled = (to != 0);
    (g_state_p->g_board.board)[cell_rows][cell_cols] = to;
    g_state_p->g_board.empty_cells_num -= (after_filled - before_filled);
    return;
}

/*  
 *  Function: undo
 *      Functionality:
 *          Checks if there is a late move to undo, if not issues a message, otherwise 
 *          it undoes all the changes associated with the current move using the iterator 
 *          it gets from Moves_Linked_List.h, it reverts all changes in the move, prints the changes 
 *          and at the end prints the board.
 *      Parameters:
 *          -   game_state *g_state_p : the game_status structure to be changed
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation, otherwise returns 
 *         PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS undo(game_state *g_state_p)
{
    change_node *c_node_p;
    char *z1_string, *z2_string;
    if(!can_undo(&(g_state_p->m_list))){
        printf("Error: no moves to undo\n");
        return PROCESS_SUCCESS; 
    }
    z1_string = (char *) malloc(g_state_p->g_board.N*sizeof(char));
    if(!z1_string){
        printf("Fatal error while performing undo.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL; 
    }
    z2_string = (char *) malloc(g_state_p->g_board.N*sizeof(char));
    if(!z2_string){
        printf("Fatal error while performing undo.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL; 
    }    

    c_node_p = get_curr_pointer_changes_iter(&(g_state_p->m_list));
    while(c_node_p) {
        change_cell(g_state_p, c_node_p->cell_rows, c_node_p->cell_cols, c_node_p->z2, c_node_p->z1);
        c_node_p = c_node_p->next;
    }
    print_board(g_state_p);
    c_node_p = get_curr_pointer_changes_iter(&(g_state_p->m_list));
    while(c_node_p) {
        sprintf(z1_string, "%d", c_node_p->z1);
        sprintf(z2_string, "%d", c_node_p->z2);
        printf("Undo %d,%d: from %s to %s\n",   c_node_p->cell_cols+1,
                                                c_node_p->cell_rows+1,
                                                (c_node_p->z2)!=0 ? z2_string : "0",
                                                (c_node_p->z1)!=0 ? z1_string : "0"
                                                );
        c_node_p = c_node_p->next;
    }
    dec_curr_pointer(&(g_state_p->m_list));
    free(z1_string);
    free(z2_string);
    return PROCESS_SUCCESS;
}

/*  
 *  Function: redo
 *      Functionality:
 *          Checks if there is a late move to redo, if not issues a message, otherwise 
 *          it does all the changes associated with the move to be redone using the iterator 
 *          it gets from Moves_Linked_List.h, it redoes all changes in the move, prints the changes 
 *          and at the end prints the board.
 *      Parameters:
 *          -   game_state *g_state_p : the game_status structure to be changed
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation, otherwise returns 
 *         PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS redo(game_state *g_state_p)
{
    change_node *c_node_p;
    char *z1_string, *z2_string;
    if(!can_redo(&(g_state_p->m_list))){
        printf("Error: no moves to redo\n");
        return PROCESS_SUCCESS; 
    }
    z1_string = (char *) malloc(g_state_p->g_board.N*sizeof(char));
    if(!z1_string){
        printf("Fatal error while performing redo.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL; 
    }
    z2_string = (char *) malloc(g_state_p->g_board.N*sizeof(char));
    if(!z2_string){
        printf("Fatal error while performing redo.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL; 
    }    
    inc_curr_pointer(&(g_state_p->m_list));
    c_node_p = get_curr_pointer_changes_iter(&(g_state_p->m_list));
    while(c_node_p) {
        change_cell(g_state_p, c_node_p->cell_rows, c_node_p->cell_cols, c_node_p->z1, c_node_p->z2);
        c_node_p = c_node_p->next;
    }
    c_node_p = get_curr_pointer_changes_iter(&(g_state_p->m_list));
    print_board(g_state_p);
    while(c_node_p) {
        sprintf(z1_string, "%d", c_node_p->z1);
        sprintf(z2_string, "%d", c_node_p->z2);
        printf("Redo %d,%d: from %s to %s\n",   c_node_p->cell_cols+1,
                                                c_node_p->cell_rows+1,
                                                (c_node_p->z1)!=0 ? z1_string : "0",
                                                (c_node_p->z2)!=0 ? z2_string : "0"
                                                );
        c_node_p = c_node_p->next;
    }
    free(z1_string);
    free(z2_string);
    return PROCESS_SUCCESS;
}

/*  
 *  Function:  save
 *      Functionality:
 *          Saves the current board to the file in file_path. If in Edit mode, the puzzle is
 *          tested for erroneous values and validated, if it isn't solvable an error is issued
 *          and no saving happens.It uses the save_game function in File_IO.h to save the game.
 *      Parameters:
 *          -   game_state *g_state_p :     the game_state structure of the current state.
 *          -   char *file_path :   the file path of the file to have the game saved to.
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation in validation, otherwise returns 
 *         PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS save(game_state *g_state_p, char *file_path)
{
    int **tmp_fixed, is_erroneous;
    RETURN_STATUS status;
    tmp_fixed = g_state_p->g_board.fixed;
    if(g_state_p->mode == Edit) {
        is_erroneous = update_erroneous(g_state_p);
        if(is_erroneous) {
            printf("Error: board contains erroneous values\n");
            return PROCESS_SUCCESS;
        }
        status = is_valid_board(g_state_p);
        if(status == PROCESS_FAIL) {
            printf("Fatal error while validating board.\n");
            printf("Exiting program without freeing resources..\n");
            return PROCESS_FAIL; 
        } else if(status == BOARD_UNSOLVABLE) {
            printf("Error: board validation failed\n");
            return PROCESS_SUCCESS;
        } else { /* status == BOARD_SOLVABLE*/
            /*
             * In edit mode we don't maintain a fixed table. Before we save game we make the 
             * fixed cells table point to the board (actual cell values), making it look like
             * all filled cells are fixed -as required-.
             * At the end this change gets reverted.
             */
            g_state_p->g_board.fixed = g_state_p->g_board.board;
        }
    }
    status = save_game(&(g_state_p->g_board), file_path);
    if(status == IO_FAIL){
        printf("Error: File cannot be created or modified\n");
    } else { /* status == IO_SUCCESS */ 
        printf("Saved in file: %s\n", file_path);
    }
    g_state_p->g_board.fixed = tmp_fixed;
    return PROCESS_SUCCESS;
}

/*  
 *  Function:  hint
 *      Functionality:
 *          Returns a hint to the specified cell if possible. First checks that the board isn't 
 *          erroneous, then that the cell isn't fixed, then that the cell isn't filled, issuing 
 *          an error if one of them didn't hold. Then it solves the board with ILP using the 
 *          function in ILP_Solver.h and giving the value in the solution if it turned out 
 *          solvable.
 *          x and y are guaranteed to be in legal range by Game_Operator
 *      Parameters:
 *          -   game_state *g_state_p : current game state structure
 *          -   int x : x coordinate of the cell
 *          -   int y : y coordinate of the cell
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation in solving the board,
 *         otherwise returns PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS hint(game_state *g_state_p, int x, int y) 
{ 
    int cell_rows, cell_cols, is_erroneous, **board;
    RETURN_STATUS status;
    cell_rows = y-1;
    cell_cols = x-1;

    is_erroneous = update_erroneous(g_state_p);
    if(is_erroneous) {
        printf("Error: board contains erroneous values\n");
        return PROCESS_SUCCESS;
    }
    if((g_state_p->g_board.fixed)[cell_rows][cell_cols]) { 
        printf("Error: cell is fixed\n");
        return PROCESS_SUCCESS;
    }
    if((g_state_p->g_board.board)[cell_rows][cell_cols]){ 
        printf("Error: cell already contains a value\n");
        return PROCESS_SUCCESS;
    }
    /* Solving the board */
    status = alloc_table(&board, g_state_p->g_board.N);
    if(status == ALLOC_FAIL){
        printf("Fatal error while getting a hint.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    copy_table(board, g_state_p->g_board.board, g_state_p->g_board.N);
    status = solve_board_with_ILP(board, g_state_p->g_board.block_rows, g_state_p->g_board.block_cols);
    if((status == ALLOC_FAIL)||(status == ILP_FAIL)) {
        printf("Fatal error while getting a hint.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    } else if(status == BOARD_SOLVABLE) {
        printf("Hint: set cell to %d\n", board[cell_rows][cell_cols]);
    } else {/* otherwise status == BOARD_UNSOLVABLE */ 
        printf("Error: board is unsolvable\n");
    }
    free_table(board, g_state_p->g_board.N);
    return PROCESS_SUCCESS;
}
/*  
 *  Function:  guess_hint
 *      Functionality:
 *          Returns all values with score greater than zero to the specified cell if possible. First checks that the board isn't 
 *          erroneous, then that the cell isn't fixed, then that the cell isn't filled, issuing 
 *          an error if one of them didn't hold. Then it solves the board with LP using the 
 *          function in LP_Solver.h and giving all scores greater than zero
 *          x and y are guaranteed to be in legal range by Game_Operator
 *      Parameters:
 *          -   game_state *g_state_p : current game state structure
 *          -   int x : x coordinate of the cell
 *          -   int y : y coordinate of the cell
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation in solving the board,
 *         otherwise returns PROCESS_FAIL indicating an erroneous exit.
 */
 
RETURN_STATUS guess_hint(game_state *g_state_p, int x, int y) 
{ 
    int N, num_of_variables, cell_rows, cell_cols, block_rows, block_cols, is_erroneous, k, **board;
    RETURN_STATUS status;
	double *sol;
	block_rows=g_state_p->g_board.block_rows;
	block_cols=g_state_p->g_board.block_cols;
    cell_rows = y-1;
    cell_cols = x-1;
	N = block_rows*block_cols;
    num_of_variables = N*N*N;
    sol = malloc(sizeof(double)*num_of_variables);
	if(sol == NULL){
        printf("allocation error in set_board_solution\n");
        return ALLOC_FAIL;
    }
    is_erroneous = update_erroneous(g_state_p);
    if(is_erroneous) {
        printf("Error: board contains erroneous values\n");
        return PROCESS_SUCCESS;
    }
    if((g_state_p->g_board.fixed)[cell_rows][cell_cols]) { 
        printf("Error: cell is fixed\n");
        return PROCESS_SUCCESS;
    }
    if((g_state_p->g_board.board)[cell_rows][cell_cols]){ 
        printf("Error: cell already contains a value\n");
        return PROCESS_SUCCESS;
    }
    /* Solving the board */
    status = alloc_table(&board, g_state_p->g_board.N);
    if(status == ALLOC_FAIL){
        printf("Fatal error while guessing hint.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    copy_table(board, g_state_p->g_board.board, g_state_p->g_board.N);
    status = solve_board_with_LP(board, g_state_p->g_board.block_rows, g_state_p->g_board.block_cols,sol);
    if((status == ALLOC_FAIL)||(status == LP_FAIL)) {
        printf("Fatal error while guessing hint.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    } else if(status == BOARD_SOLVABLE) {
		printf("The legal values (and their scores) for %d %d:\n",cell_cols+1,cell_rows+1);
		for(k=0; k<N; k++){
			if (sol[cell_rows*N*N+cell_cols*N+k]>0)
				printf("value: %d score: %f\n",k+1 , sol[cell_rows*N*N+cell_cols*N+k]);
			}
	} else {/* otherwise status == BOARD_UNSOLVABLE */ 
        printf("Error: board is unsolvable\n");
    }
	free(sol);
    free_table(board, g_state_p->g_board.N);
    return PROCESS_SUCCESS;}

/*  
 *  Function: num_solutions
 *      Functionality:
 *          Checks if the puzzle is erroneous, if yes issues an error and returns.
 *          If not it runs the back tracking algorithm in Back_Tracking_Solver.h
 *          to count the number of solutions of the puzzle. And prints the number with
 *          other messages regarding the number of solutions.
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation in counting the number of
 *         solutions, otherwise returns PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS num_solutions(game_state *g_state_p) 
{
    int is_erroneous, **board, num_of_solutions;
    RETURN_STATUS status;

    /* Check if the board is erroneous */
    is_erroneous = update_erroneous(g_state_p);
    if(is_erroneous) {
        printf("Error: board contains erroneous values\n");
        return PROCESS_SUCCESS;
    }
    /* Finding number of solutions */
    status = alloc_table(&board, g_state_p->g_board.N);
    if(status == ALLOC_FAIL){
        printf("Fatal error while getting a number_of_solutions.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    copy_table(board, g_state_p->g_board.board, g_state_p->g_board.N);
    status = backTrack(&num_of_solutions, board, g_state_p->g_board.block_rows,  g_state_p->g_board.block_cols);
    if(status == ALLOC_FAIL) {
        printf("Fatal error while getting a number_of_solutions.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    printf("Number of solutions: %d\n", num_of_solutions);
    free_table(board, g_state_p->g_board.N);
    return PROCESS_SUCCESS;
}

/*  
 *  Function: reset
 *      Functionality:
 *          Reverts all the moves that have been applied to the board since it was loaded.
 *           At the end it moves the pointer to the head of moves_list of the current game_state structure.
 *      Parameters:
 *          -   game_state *g_state_p : the current game_state structure
 *      Return Value:
 *          Function guaranteed to return PROCESS_SUCCESS, currently does this to allow
 *          future changes
 */
RETURN_STATUS reset(game_state *g_state_p)
{
    change_node *c_node_p;
    /* Iterating over all move_nodes currently in list */
    while(can_undo(&(g_state_p->m_list))) {
        c_node_p = get_curr_pointer_changes_iter(&(g_state_p->m_list));
        /* reverting all the changes associated with the move */
        while(c_node_p) {
            change_cell(g_state_p, c_node_p->cell_rows, c_node_p->cell_cols, c_node_p->z2, c_node_p->z1);
            c_node_p = c_node_p->next;
        }
        dec_curr_pointer(&(g_state_p->m_list));
    }
    print_board(g_state_p);	
    return PROCESS_SUCCESS; 
}

/*  
 *  Function: autofill
 *      Functionality:
 *          Function check for erroneous values, if none exist it checks the number of 
 *          legal values in an empty cell using the function find_all_legal_values 
 *          defined above, if the number is 1 it performs a change of this cell and updates
 *          the move_node of this command to contain this change. At the end, if the move_node 
 *          contains changes it is added to the moves_list otherwise discarded.
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure of the current move
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation, otherwise 
 *         returns PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS autofill(game_state *g_state_p)
{ 
    int **old_board, cell_rows, cell_cols;
    int *legal_values_list, num_legal_values;
    int is_erroneous;
    move_node *m_node_p;
    RETURN_STATUS status;

    /* Check if the board is erroneous */
    is_erroneous = update_erroneous(g_state_p);
    if(is_erroneous) {
        printf("Error: board contains erroneous values\n");
        return PROCESS_SUCCESS;
    }
    /* allocate a copy of the current board  */
    status = alloc_table(&old_board, g_state_p->g_board.N);
    if(status == ALLOC_FAIL){
        printf("Fatal error while performing autofill.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    copy_table(old_board, g_state_p->g_board.board, g_state_p->g_board.N);
    /* allocating the array to have the legal values of a cell */
    legal_values_list = (int *) calloc(g_state_p->g_board.N, sizeof(int));
    if(!legal_values_list){
        printf("Fatal error while performing autofill.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    status = alloc_move_node(&m_node_p);
    if(status == ALLOC_FAIL){
        printf("Fatal error while performing autofill.\n");
        printf("Exiting program without freeing resources..\n");
        return PROCESS_FAIL;
    }
    for(cell_rows=0; cell_rows<g_state_p->g_board.N; cell_rows++){
        for(cell_cols=0; cell_cols<g_state_p->g_board.N; cell_cols++){
            if(!old_board[cell_rows][cell_cols]){
                num_legal_values = find_all_legal_values(   old_board, 
                                                            cell_rows, cell_cols,
                                                            g_state_p->g_board.block_rows, g_state_p->g_board.block_cols, 
                                                            legal_values_list);
                if(num_legal_values == 1) {
                    /* Found a cell with 1 legal value only */
                    status = add_change_to_move(m_node_p, cell_rows, cell_cols, 0, legal_values_list[0]);
                    if(status == ALLOC_FAIL){
                        printf("Fatal error while performing autofill.\n");
                        printf("Exiting program without freeing resources..\n");
                        return PROCESS_FAIL;
                    }
                    change_cell(g_state_p, cell_rows, cell_cols, 0, legal_values_list[0]);
                    printf("Cell <%d,%d> set to %d\n", cell_cols+1, cell_rows+1, legal_values_list[0]);
                }
            }
        }
    }
    if(m_node_p->changes_list_head) { /* At least one change has been made */
        add_move_node_to_list(&(g_state_p->m_list), m_node_p);
    } else { /* No change has been made */
        free_move_node(m_node_p);
    }
    free(legal_values_list);
    free_table(old_board, g_state_p->g_board.N);
    print_board(g_state_p);
    /* Checking if board is completely filled */
    if(g_state_p->g_board.empty_cells_num == 0){
        status = is_valid_board(g_state_p);
        if(status == PROCESS_FAIL) {
            printf("Fatal error while validating board after all cells were filled.\n");
            printf("Exiting program without freeing resources..\n");
            return PROCESS_FAIL; 
        } else if(status == BOARD_UNSOLVABLE) {
            printf("Puzzle solution erroneous\n");
        } else { /* status == BOARD_SOLVABLE*/
            printf("Puzzle solved successfully\n");
            free_game(g_state_p);
        }
    }
    return PROCESS_SUCCESS;
}
