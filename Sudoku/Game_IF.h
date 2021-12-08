#ifndef GAME_IF_H_ 
#define GAME_IF_H_

/**************************************** Interface ****************************************
 *  Interface: Game IF
 *  Implemantation: Game.c
 *  Including modules:  Game_Operator.c 
 *  Usage:
 *      This interface provides all the functionality on the game state struct 
 *      to the Game Operator. The Operator calls these functions to process the 
 *      user input commands after performing parameter checking.
 *      It relies heavily of the struct definitions in the data base.
 *      READ the definitions_db.h documentation first to understand the struct used.
 *******************************************************************************************/

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
void free_game(game_state *g_state_p);

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
RETURN_STATUS solve(game_state *g_state_p, char *file_path);

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
RETURN_STATUS edit(game_state *g_state_p, char *file_path);

/*  
 *  Function: print_board
 *      Functionality:
 *          Prints the current board, Uses the algorithm presented in the project document.
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure that contains the board to be printed
 *      Return Value:
 *          void
 */
void print_board(game_state *g_state_p);

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
RETURN_STATUS set(game_state *g_state_p, int x, int y, int z);

/*  
 *  Function: validate
 *      Functionality:
 *          Validates that the current board is solvable using ILP. It checks for erroneous
 *          values first, and reports if such are found.
 *      Parameters:
 *          -   game_state *g_state_p : game_state struct that contains board to be validated.
 *      Return Value:
 *          Returns PROCESS_SUCCESS if it succeeded to perform the command without fatal errors.
 *          otherwise returns PROCESS_FAIL indicating a fatal error
 */
RETURN_STATUS validate(game_state *g_state_p);
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
RETURN_STATUS guess(game_state *g_state_p, double x);

/*  
 *  Function:  generate
 *      Functionality:
 *          Checks first that the board is empty, if not issues an error message, otherwise it
 *          generates a puzzle by randomly filling x cells with random legal values, running ILP
 *          to solve the resulting board, And then clearing all but y random cells.
 *          At the end if it succeeded it prints the board. And adds the move to the redo/undo list
 *          x and y are guaranteed to be in legal range by the Game_Operator before calling this function.
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure which contains the board.
 *          -   int x : number of cells to be filled at start
 *          -   int y : number of filled cells after generating a solvable board
 *      Return Value:
 *          Returns PROCESS_SUCCESS if it succeeded to perform the command without fatal errors.
 *          otherwise returns PROCESS_FAIL indicating a fatal error
 */
RETURN_STATUS generate(game_state *g_state_p, int x, int y);

/*  
 *  Function: undo
 *      Functionality:
 *          Checks if there is a late move to undo, if not issues a message, otherwise 
 *          it undoes all the changes associated with the current move, it reverts all changes
 *          in the move, prints the changes and the board.
 *      Parameters:
 *          -   game_state *g_state_p : the game_status structure to be changed
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation, otherwise returns 
 *         PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS undo(game_state *g_state_p);

/*  
 *  Function: redo
 *      Functionality:
 *          Checks if there is a late move to redo, if not issues a message, otherwise 
 *          it does all the changes associated with the move to be redone, it redoes all changes
 *          in the move, prints the changes and the board.
 *      Parameters:
 *          -   game_state *g_state_p : the game_status structure to be changed
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation, otherwise returns 
 *         PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS redo(game_state *g_state_p);

/*  
 *  Function:  save
 *      Functionality:
 *          Saves the current board to the file in file_path. If in Edit mode, the puzzle is
 *          tested for erroneous values and validated, if it isn't solvable an error is issued
 *          and no saving happens.
 *      Parameters:
 *          -   game_state *g_state_p :     the game_state structure of the current state.
 *          -   char *file_path :   the file path of the file to have the game saved to.
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation in validation, otherwise returns 
 *         PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS save(game_state *g_state_p, char *file_path);

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
RETURN_STATUS hint(game_state *g_state_p, int x, int y);

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
RETURN_STATUS guess_hint(game_state *g_state_p, int x, int y);

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
RETURN_STATUS num_solutions(game_state *g_state_p);

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
RETURN_STATUS reset(game_state *g_state_p);

/*  
 *  Function: autofill
 *      Functionality:
 *          Function check for erroneous values, if none exist it checks the number of 
 *          legal values in an empty cell, if the number is 1 it performs a change of this cell 
 *          and updates the  moves linked list at the end.
 *      Parameters:
 *          -   game_state *g_state_p : the game_state structure of the current move
 *      Return Value:
 *         PROCESS_SUCCESS if no fatal fail happened at memory allocation, otherwise 
 *         returns PROCESS_FAIL indicating an erroneous exit.
 */
RETURN_STATUS autofill(game_state *g_state_p);

#endif
