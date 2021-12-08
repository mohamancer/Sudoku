#include "Back_Tracking_Solver.h"

/****************************************** Module ******************************************
 *  Module: Back Tracking Solver
 *  Implements: Back_Tracking_Solver.h
 *  Usage:
 *     This module implements the interface above. It implements a stack data structure 
 *     and functions to work on the stack, as well as other functions used in the 
 *     implementation of the back tracking algorithm.
 *******************************************************************************************/

/*
 *  Structure: node
 *  Usage:
 *      Used by the backtracking algorithm to implement the recursive algorithm 
 *      using a stack implementation. The node represents a recursive call.
 *  Members:
 *      (row, col)  :   the coordinates of a cell in the board.
 *      val         :   the value to be tried in the cell next by the algorithms.  
 *      next        :   pointer to the next node in the stack.
 */
typedef struct node
{
    int row;
    int col;
    int value;
    struct node *next;
} node;

/*
 *  Structure: stack
 *  Usage:
 *      Used by the backtracking algorithm to implement the recursive algorithm 
 *      using a stack implementation. The stack represents the queue used by the 
 *      algorithm, it is a wrapper of the stack head to abstract the stack 
 *      implementation
 *  Members:
 *      head        :   pointer to the head node of the stack
 */
typedef struct stack
{
    node *head ;
} stack;

/*  
 *  Function: push
 *      Functionality:
 *          Inserts triplet (row, col, val) to the head of the stack pointed to by stack_p. 
 *          Allocates a new node and inserts it to the head of the stack.
 *      Parameters:
 *          -   stack *stack_p :    pointer to the stack.
 *          -   int row        :    the row of the new node
 *          -   int col        :    the col of the new node
 *          -   int value      :    the val of the new node
 *      Return Value:
 *          ALLOC_SUCCESS if the allocation of the new node was successful, otherwise returns ALLOC_FAIL.
 */
static RETURN_STATUS push(stack *stack_p, int row, int col, int value)
{
    node *new_node;
    new_node = (node *)malloc(sizeof(node));
    if (!new_node){
        printf("Error in malloc push");
        return ALLOC_FAIL;
    }
    new_node->col=col;
    new_node->row=row;
    new_node->value=value;
    new_node->next=stack_p->head;
    stack_p->head=new_node;
    return ALLOC_SUCCESS;
}

/*  
 *  Function:  is_empty
 *      Functionality:
 *          Used to know it stack is empty 
 *      Parameters:
 *          -   stack *stack_p :    pointer to the stack.
 *      Return Value:
 *          Returns 1 if stack is empty, otherwise returns 0.          
 */
static int is_empty(stack *stack_p)
{
    return (stack_p->head == NULL);
}

/*  
 *  Function: pop
 *      Functionality:
 *          Deletes the head of the stack and frees the space allocated for it.
 *          Assumes (!is_empty(stack_p)).
 *      Parameters:
 *          -   stack *stack_p :    pointer to the stack.
 *      Return Value:
 *          void 
 */
static void pop(stack *stack_p)
{
    node *temp;
    temp = stack_p->head;
    stack_p->head = (stack_p->head)->next;
    free(temp);
}

/*  
 *  Function: peek
 *      Functionality:
 *          Peeks into the stack head and updates the given pointers to have the 
 *          values of the head node.
 *          Assumes (!is_empty(stack_p)).
 *      Parameters:
 *          -   stack *stack_p :    pointer to the stack.
 *          -   int* row    : pointer to an int to have the value of the row member of the stack head
 *          -   int* col    : pointer to an int to have the value of the col member of the stack head
 *          -   int* value  : pointer to an int to have the value of the value member of the stack head 
 *      Return Value:
 *          void          
 */
static void peek(int* row, int* col, int* value, stack* stack_p)
{
    *col=stack_p->head->col;
    *row=stack_p->head->row;
    *value=stack_p->head->value;
}

/*  
 *  Function: update_stack_head
 *      Functionality:
 *          Updates the head of the stack to the given values.
 *          Used to change the head of the stack without deallocating the space for 
 *          the head and then allocating a new node, thus saving memory allocation
 *          operations. Assumes (!is_empty(stack_p)).
 *      Parameters:
 *          -   stack *stack_p :    pointer to the stack.
 *          -   int row        :    the row of the updated head.
 *          -   int col        :    the col of the updated head.
 *          -   int value      :    the val of the updated head.
 *      Return Value:
 *          void 
 */
static void update_stack_head(stack *stack_p, int row, int col, int value)
{
    stack_p->head->col=col;
    stack_p->head->row=row;
    stack_p->head->value=value;
}

/*  
 *  Function:  get_box_start
 *      Functionality:
 *          Function to find the upper left corner of the block (*i,*j) that contains the
 *          cell (row_x, col_y) in a board with blocks of dimensions (block_rows, block_cols)
 *          It updates the 2 pointers given to it.
 *      Parameters:
 *          -   int* i          :   pointer to an int to contain the x coordinate of the block
 *          -   int* j          :   pointer to an int to contain the y coordinate of the block
 *          -   int row_x       :   the x coordinate of the cell    
 *          -   int col_y       :   the y coordinate of the cell    
 *          -   int block_rows  :   number of rows in a block 
 *          -   int block_cols  :   number of cols in a block 
 *      Return Value:
 *          void 
 */
static void get_box_start(int* i,int* j, int row_x, int col_y, int block_rows, int block_cols)
{
    *i = row_x/block_rows;
    *j = col_y/block_cols;
}

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
RETURN_STATUS is_legal_value(int ** Board, int row_x, int col_y, int value, int block_rows, int block_cols)
{
    int i,j,N,l,m;
    N = block_cols*block_rows;
    for(i=0; i<N; i++){
        if(Board[i][col_y]==value)
            return NOT_LEGAL_VALUE;
        if(Board[row_x][i]==value)
            return NOT_LEGAL_VALUE;
    }
    get_box_start(&i,&j,row_x,col_y,block_rows,block_cols);
    for(l=0;l<block_rows;l++){
        for(m=0;m<block_cols;m++){
            if(Board[i*block_rows+l][j*block_cols+m]==value)
                return NOT_LEGAL_VALUE;
        }
    }
    return LEGAL_VALUE;
}

/*  
 *  Function: get_next_cell
 *      Functionality:
 *          Finds the next empty cell in the board (Board) with dimension N, starting at the cell
 *          in (*x ,*y) and updates the pointers to the next empty cell. Goes through the row before 
 *          moving to the next row. When no empty cells are present after cell (*x, *y) it updates
 *          the pointers to (-1, -1).
 *      Parameters:
 *          -   int **Board : the board
 *          -   int N       : the dimension of the board
 *          -   int *x      : pointer to x coordinate of the current cell, and gets updated to the new value.
 *          -   int *y      : pointer to y coordinate of the current cell, and gets updated to the new value.
 *      Return Value:
 *          void
 */
static void get_next_cell(int **Board, int N, int *x, int *y)
{
    int i,j;
    i = *x;
    j = *y;
    while((!((j==0) && (i==N))) && Board[i][j])
    {
        if(j<N-1){
            j++;
        }
        else{
            j = 0;
            i++;
        }
    }
    if((j==0) && (i==N)){
        *x = -1;
        *y = -1;
        return;
    }
    *x = i;
    *y = j;
}

/*  
 *  Function:  backTrack
 *      Functionality:
 *          Performs the back tracking algorithm to find the number of solutions
 *          of the board (board) with block dimensions of (block_rows, block_cols)
 *          and updates the pointer given (num_of_solutions) to the number of solutions. It uses the stack
 *          structure defined above as a FIFO queue to implement the recursive calls 
 *          of the original recursive algorithm.
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
RETURN_STATUS backTrack(int* num_of_solutions,int** board, int block_rows, int block_cols)
{
    int i=0,j=0,num,N,sum=0;
    stack *board_updates;
    RETURN_STATUS status;

    N = block_rows*block_cols;
    board_updates= (stack *) calloc(1, sizeof(stack));
    if (!board_updates)
        return ALLOC_FAIL;
    if(board[i][j])
        get_next_cell(board, N, &i, &j);

    status = push(board_updates, i, j, 1);
    if(status == ALLOC_FAIL)
        return ALLOC_FAIL;
    while(!is_empty(board_updates)){
        /* gets the cell values that are on the stack head */
        peek(&i, &j ,&num, board_updates);		
        if(i==-1) {
            /* 
             * if the board is fully solved then we should go one step back
             * by deleting the stack head and reading the head again 
             */
            sum++;
            pop(board_updates);
            continue;
        }
        board[i][j] = 0;
        if(num == N+1){
            /* 
             * if all values in the current cell are illegal values
             * then we delete the stack head and read the head again 
             */
            pop(board_updates);
            continue;
        }
        if(is_legal_value(board, i, j, num, block_rows, block_cols)==LEGAL_VALUE){
            /* 
             * if the value is legal we update the board with the value
             * we also update the head value to the num+1 so when we get
             * back to this cell we continue checking from num+1
             * we push to the stack the next cell to check with the value of 1 as first check
             */
            board[i][j] = num;
            update_stack_head(board_updates, i, j, num+1);
            get_next_cell(board, N, &i, &j);
            status = push(board_updates, i,j,1);
            if(status == ALLOC_FAIL)
                return ALLOC_FAIL;
        }
        else /* else we update the stack head to the next value num+1 to recheck the same cell */
            update_stack_head(board_updates, i, j, num+1);
    }
    *num_of_solutions = sum;
    free(board_updates);
    if (sum==0)
        return BOARD_UNSOLVABLE;
    return BOARD_SOLVABLE;
}
