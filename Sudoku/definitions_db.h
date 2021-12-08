#ifndef DEFINITIONS_DB_H_
#define DEFINITIONS_DB_H_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

/**************************************** Data Base ****************************************
 *  This module provides a data base of enum/struct/constants definitions. It is included
 *  in all the modules of the project and provides:
 *      0.  3 defined constants used as default values.
 *      1.  RETURN_STATUS - return values for all functions of the module.
 *      2.  COMMAND_TYPE -  enum for the type of a command within a command struct
 *      3.  command - a struct used to contain a user command
 *      4.  moves_list, move_node, change_node - structs used to implement the moves 
 *                                               doubly linked list.
 *      5.  game_board - a struct to contain the game board and it's attributes
 *      6.  GAME_MODE - an enum for the game mode.
 *      7.  gane_state - a struct that contains all information about the game, command 
 *                       processing is based on this struct, and it gets modified to 
 *                       reflect each command processed when needed.
 *******************************************************************************************/

#define INVALID_PARA -1        /* Used in the command struct for an integer parameter to indicate it's not an integer */    
#define DEFAULT_PARA -2        /* The default value of an integer parmaeter in the command struct */
#define NUM_OF_PARAMETERS 3    /* The maximal number of integer parameters a command can have */

/*  Enum:    RETURN_STATUS
 *  Usage:
 *      This enumeration type is used as the return value of almost all the functions
 *      in all modules. Not all functions return all values, the return values for each
 *      function are described in it's documentation.
 */ 
typedef enum 
{
/* Used in functions that dynamically allocate memory to indicate if it was successful*/
    ALLOC_SUCCESS,
    ALLOC_FAIL,
/* Used in functions that fill some kind of array */
    FILL_SUCCESS,
    FILL_FAIL,
/* Used in functions that perform a generation (randomly) */
    GENERATE_SUCCESS,
    GENERATE_FAIL,
/* Used in the File_IO module to indicate whether IO operation succeeded or failed */
    IO_SUCCESS,
    IO_FAIL,
/* Used in the ILP_Solver module to indicate whether operation on Gurobi succeeded or failed */
    ILP_FAIL,
    ILP_SUCCESS,
/* Used in the ILP_Solver module to indicate whether operation on Gurobi succeeded or failed */
    LP_FAIL,
    LP_SUCCESS,
/* Used in the ILP_Solver/Game/Back_Tracking module to indicate whether a puzzle is solvable or not */
    BOARD_SOLVABLE,
    BOARD_UNSOLVABLE,
/* Used in functions that try to place a value somewhere to indicate whether that placement is legal or not */
    LEGAL_VALUE,
    NOT_LEGAL_VALUE,
/* Used in the game loop to return if the game exited safely -then it would free resources as needed- or not */
    SAFE_EXIT,
    ERRONEOUS_EXIT,
/* Used by all root functions that process commands in Game_Operator and Game to indicate whether the
 * processing of a command was completed successfully, then the game can resume with getting more 
 * commands from the user or it encountered a fatal error (e.g in memory allocation) then the game 
 * should just perform an exit because a fatal error happened
 */
    PROCESS_SUCCESS,
    PROCESS_FAIL,
/* Used by the function that reads a line from the user to indicate the type of the line it read*/
    LEGAL_LENGTH_LINE,
    ILLEGAL_LENGTH_LINE,
    BLANK_LINE,
    REACHED_EOF
} RETURN_STATUS;   

/*  Enum:    COMMAND_TYPE
 *  Usage:
 *      This enum represents the different kinds of commands 
 *      that the user can enter to the command line interface of the 
 *      game. It is used in the command struct to represent the kind
 *      of the commnan within the struct.
 */ 
typedef enum
{
    SOLVE,
    EDIT,
    MARK_ERRORS,
    PRINT_BOARD,
    SET,
    VALIDATE,
    GENERATE,
    UNDO,
    REDO,
    SAVE,
    HINT,
	GUESS_HINT,
	GUESS,
    NUM_SOLUTIONS,
    AUTOFILL,
    RESET,
    EXIT,
    INVALID_COMMAND,
	INVALID_PARA_NUM,
    DEFAULT_COMMAND
} COMMAND_TYPE;

/*
 *  Structure: command
 *  Usage:  
 *      This structure represents a command that was taken from the user and parsed 
 *      by the parser. Only one structure of this type gets allocated (statically) in 
 *      Main_Aux.c, the usage of this struct:
 *           Repeat:
 *              1.  Main_aux passes the struct to the parser along with a command line from the user
 *              2.  Parser fills the command struct with the information from the line.
 *              3   Main_Aux passes the struct to Game_Operator to process the command.
 *  Members:
 *      COMMAND_TYPE cmd_type   :   enum that represents the command type 
 *      int parameters[NUM_OF_PARAMETERS] : array of NUM_OF_PARMETERS ints that were partially given 
 *                                          by the user (differs between kinds of comands - not all commands
 *                                          need NUM_OF_PARAMETERS int parameters).
 *      float para :        only used for GUESS command
 *      char *file_path :   a string to contain the file path given by several command that involve IO,
 *                          no memory is allocated to contain the file_path, it just points to the file_path 
 *                          start in the char array given to the parser by Main_Aux.
 */
typedef struct
{
    COMMAND_TYPE cmd_type;
    int parameters[NUM_OF_PARAMETERS];
	double para;
    char *file_path;
} command;

/*
 *  Structure: change_node
 *  Usage:  
 *      This structure represents a change (in a specific cell) that happened
 *      during performing a move (like set/autofill). The changes are saved 
 *      in a singly linked list for all moves to make the implementaions uniform. 
 *  Members:
 *      int cell_rows : the row of the cell associated with the change
 *      int cell_cols : the column of the cell associated with the change 
 *      int z1  :   the value of the cell before the move
 *      int z2  :   the value of the cell after the move
 *      change_node *next   :   pointer to the next change of the changes list
 */
typedef struct Change_Node {
    int cell_rows;
    int cell_cols;
    int z1;
    int z2;
    struct Change_Node *next;
} change_node;

/*
 *  Structure: move_node
 *  Usage:  
 *      This strucure represents a move in the moves doubly linked list. It represents 
 *      a single command that made changes to the board (like set/autofill) and 
 *      this move has a liked list of change_node's representing the single changes
 *      of the move (per cell).
 *  Members:
 *      move_node *next : the next move_node in the moves linked list
 *      move_node *prev: the next move_node in the moves linked list
 *      change_node *changes_list_head : the head of the changes linked list of the move
 *      change_node *changes_list_tail : the tail of the changes linked list of the move
 */
typedef struct Move_Node
{
    struct Move_Node *next;
    struct Move_Node *prev;
    change_node *changes_list_head;
    change_node *changes_list_tail;
} move_node;

/*
 *  Structure: moves_list
 *  Usage:  
 *      -   This strucure represents the moves linked list. It is a circular doubly linked list with
 *          a sentinel node (eases implementation). Each move_node in the list represents a move 
 *          that possibly made changes to the board (like set/autofill/generate), and each move_node
 *          contains a linked list of change_node's that represent the specific changes per cell.
 *      -   It contains a pointer to the sentinel and a pointer to the current move that reflects 
 *          the current state of the board considering all the redo/undo operations. When this pointer
 *          points to the sentinel, it means that the board is at the same state as when it was loaded.
 *      -   Only one structure of this type is declared in the whole program and it is allocated statically
 *          once only in Maic_Aux as part of the game_state structure. The move_node structs get dynamically
 *          allocated when needed and freed when there isn't a need for them
 *  Members:
 *      move_node *sentinel : the sentinel of the moves linked list
 *      move_node *curr_pointer: the pointer to the current move in the linked list.
 */
typedef struct
{
    move_node sentinel;
    move_node *curr_pointer;
} moves_list;

/*
 *  Structure: game_board
 *  Usage:  
 *      -   This strucure represents the game board. It contains 3 2-D arrays, one for the cell values,
 *          and two binary ones for indicating whether a cell is fixed or erroneous or neither. It also
 *          contains the dimensions of the block, and the board. In addition, it has the number of empty
 *          cells currently in the board for efficient checking after set/autofill or before generate.
 *      -   The 3 2-D arrays within the struct get dynamically allocated when needed, and they get freed
 *          at exit or at the start of a new game.
 *      -   Only one structure of this type is declared in the whole program and it is allocated statically
 *          once only in Main_Aux as part of the game_state struct.
 *  Members:
 *      int **board :   the actual puzzle with filled values, a value of 0 indicates a cel is empty.
 *                      board[i][j] represents the cell (j+1,i+1) in the way the user interacts with
 *                      the game and described in the project file
 *      int **err   :   logical table to indicate whether a cell is erroneous or not
 *                      -   We don't maintain this table locally per cell, but we update it when it's needed
 *                          to know if a board contains erroneous values or when we print a board when mark 
 *                          errors is ON. This saves time when a function like autofill/generate and their 
 *                          respective redo/undo perform a lot of changes to the board.
 *      int **fixed :   logical table to indicate whether a cell is fixed or not, it is used only in Solve 
 *                      mode. In Edit mode it is completely cleared beacuse all cells are considered fixed.
 *      int block_cols  :   the number of columns in a block    
 *      int block_rows  :   the number of rows in a block
 *      int N   :   equals block_rows*block_cols, saves redundant recomputation of this value over and over.
 *      int empty_cells_num :   the number of empty cells in the board. It is maintained with each change to 
 *                              the board
 *
 */
typedef struct
{
   int **board;
   int **err;
   int **fixed;
   int block_cols;
   int block_rows;
   int N;
   int empty_cells_num;
}  game_board;
 
/*  Enum:   GAME_MODE 
 *  Usage:
 *      This enumeration type is used to store the game mode in the game_state struct.
 */ 
typedef enum
{
    Init,
    Solve,
    Edit
} GAME_MODE;

/*
 *  Structure: game_state
 *  Usage:  
 *      -   This strucure represents the game's state. It is the struct that contains everything that reflects 
 *          the state of the game. A pointer to this struct gets tossed around between Game_Aux-Game_Operator-Game,
 *          these modules have access to the relevant struct (moves_list/game_board) through this one, and they change
 *          it to reflect changes in the game state.
 *      -   Only one structure of this type is declared in the whole program and it is allocated statically
 *          once only in Maic_Aux.
 *  Members:
 *      GAME_MODE mode  :   the game mode declared above
 *      int mark_errors :   the mark_errors parameter of the game, it defaults to 1 at the start of the game
 *                          and maintained throughout.
 *      moves_list m_list   :   the moves_list struct descibed above
 *      game_board g_board  :   the game_list struct described above
 *      int valid   :   indicates whether the game_board and moves_list struct are valid or not, they should be always 
 *                      valid instead in Init mode. It used used to know if memory de-allocation is needed for these 
 *                      structs at exit or not.
 */
typedef struct
{
    GAME_MODE mode;
    int mark_errors;
    moves_list m_list;
    game_board g_board;
    int valid;
} game_state;

#endif
