
#define MAX_LINE_LENGTH 256
#define ALWAYS 1
#include "Main_Aux.h"
#include "Parser.h"
#include "Game_Operator.h"

/****************************************** Module ******************************************
 *  Module: Main Aux
 *  Implements: Main_Aux.h
 *  Includes: Parser.h Game_Operator.h
 *  Usage:
 *      This modules implements the main logic of the game. It is called by the main
 *      function to start a new game. 
 *      The program flow:
 *          1.  A line is taken from user.
 *          2.  The line gets preprocessed in this module.
 *          3.  The line gets passed to parser that fills a command struct with the user 
 *              command type and parameters.
 *          4.  The command struct gets passed to the Game Operator along with the 
 *              game_state for processing and updating the game_state.
 *     This module includes some internally used functions for performing the main functinality
 *     of the module.
 *******************************************************************************************/
/* Internally used functions (partial list - others also exist) */
static void initialize_aux_struct(command *);
static RETURN_STATUS get_new_line(char *);
static RETURN_STATUS enter_game_loop(command *,game_state *);

/*  
 *  Function: start_new_game
 *      Functionality:
 *          This function is called by main once, it allocates (statically) memory for the
 *          command and game_state (which contains a game_board and moves_list) structs. 
 *          Memory for structs of these types is allocated only once for the entire game operation,
 *          and it gets statically de-allocated once this function returns.
 *          It initializes the command and game_state structs to default values and calls the 
 *          enter_game_loop function. If it returns SAFE_EXIT indicating exit by the user command
 *          or an EOF has been read, it frees memory allocated within game_state. Otherwise a 
 *          fatal error has happened and the game exits without freeing these resources.
 *      Parameters:
 *          none
 *      Return Value:
 *          void
 */
void start_new_game()
{
    RETURN_STATUS game_loop_status;
    command cmd;
    game_state g_state;

    initialize_aux_struct(&cmd);
    initialize_game_aux_struct(&g_state);

    game_loop_status = enter_game_loop(&cmd, &g_state);
    if(game_loop_status == SAFE_EXIT) {
        free_game_aux_struct(&g_state); 
    } 
    return;
}

/*  
 *  Function: enter_game_loop
 *      Functionality:
 *          This is the main loop of the game. It gets called from start_new_game after a game_state and command 
 *          structs have been allocated. It performs the following:
 *              1. allocates an array to hold user input (statically - length is known)
 *              2. performs forever (or until function returns):
 *                   a. Prints the enter command line.
 *                   b. Gets an input line from user:
 *                          i.  if it was blank, it returns to loop start.
 *                         ii.  if it was illegal length it notifies user and returns to start of loop.
 *                        iii.  if it reached EOF, it returns to caller with SAFE_EXIT.
 *                         iv.  if it was legal it continues to c.
 *                   c. It passes the line read to parsing at the Parser, which fills the command struct
 *                      with the command info (look at definitions_db.h on struct def).
 *                   d. It checks the command type in the struct:
 *                          i.  EXIT, it returns a SAFE_EXIT to caller.
 *                         ii.  INVALID_COMMAND, it prints error and returns to start of loop.
 *                        iii.  All other, it passes pointers to the command and game_state structs to 
 *                              the Game_Operator to process the commands:
 *                                  a. PROCESS_SUCCESS, returns to start of loop.
 *                                  b. PROCESS_FAIL, a fatal error happened (e.g. memory allocation),
 *                                     it returns ERRONEOUS_EXIT to caller.
 *      Parameters:
 *          -   command *cmd_p  :   pointer to a command struct already allocated.
 *          -   game_state *g_state_p   :   pointer to a game_state struct already allocated.
 *      Return Value:
 *          Returns either SAFE_EXIT indicating it should exit the game because of a command exit from
 *          user or reached EOF, or ERRONEOUS_EXIT indicating a fatal error (e.g. memory_allocation or Gurobi)
 *          happened, and caller should exit game without freeing resources.
 */
static RETURN_STATUS enter_game_loop(command *cmd_p, game_state *g_state_p)
{
    char allocated_line[MAX_LINE_LENGTH + 2]; /* The array is sized up to detect longer than acceptable lines */
    char *line;
    RETURN_STATUS get_line_return_status, process_status; 

    line = allocated_line;
    while(ALWAYS)   /* A forever loop */
    {
        printf("Enter a command:\n");
        get_line_return_status = get_new_line(line);
        switch(get_line_return_status) {
            case ILLEGAL_LENGTH_LINE :
                printf("ERROR: Too many charcters in line");
                break;
            case LEGAL_LENGTH_LINE :
                parse_command(line, cmd_p);
                if( cmd_p->cmd_type == EXIT) {
                    return SAFE_EXIT;
                } else if (cmd_p->cmd_type == INVALID_COMMAND) {
                    print_invalid_command();
                } else if (cmd_p->cmd_type==INVALID_PARA_NUM){

					continue;
				}
				else {
                    process_status = process_command(cmd_p, g_state_p);  
                    if(process_status == PROCESS_FAIL)
                        return ERRONEOUS_EXIT;
                    /* here: process_status == PROCESS_SUCCESS
                     * should just return to start of loop
                     */
                }
                break;
            case BLANK_LINE :
                break;
            case REACHED_EOF :
                return SAFE_EXIT; 
            default:
                /* Should never be here */
                printf("error in the return value of get_new_line()");
                return ERRONEOUS_EXIT;
        }
    }
}

/*  
 *  Function: is_blank_line
 *      Functionality:
 *          Tests whether the null terminated line in the input contains white spaces 
 *          only or not.
 *      Parameters:
 *          -   char *line : the line to be tested.
 *      Return Value:
 *          1 if the line contains white spaces only, otherwise 0.
 */
static int is_blank_line(char *line)
{
    while(*line != '\0'){
        if(!isspace(*line))
            return 0;
        line++;
    }
    return 1;
}

/*  
 *  Function: discard_rest_of_line
 *      Functionality:
 *          Reads user input char by char until a new line is reached.
 *          Used when the user typed a line longer than the max length to ignore the rest
 *          of the line.
 *      Parameters:
 *          none
 *      Return Value:
 *          void
 */
static void discard_rest_of_line()
{
    char c;    
    do {
        c = getc(stdin);
    } while (c!=EOF && c!='\n');
}

/*  
 *  Function: get_new_line
 *      Functionality:
 *          This function receives a new line from the user and fills it to the already allocated
 *          array pointed to by the function parameter.
 *          4 cases can be encountered:
 *          -   reached EOF, then it is returned to caller to initiate en exit with freeing resources.
 *          -   read a blank line, the line read has white spaces only, it is indicated to caller to ignore it.
 *          -   read a line of legal length, it is filled to the array given and indicated to caller.
 *          -   read a line longer than the maximum length, the rest of the line gets discarded and the caller is
 *              nodified
 *      Parameters:
 *          -   char *line : the array to be filled with the line read from user.
 *      Return Value:
 *          Returns one of: REACHED_EOF, BLANK_LINE, LEGAL_LENGTH_LINE, ILLEGAL_LENGTH_LINE. As described
 *          in functionality.
 */
static RETURN_STATUS get_new_line(char *line)
{
    int line_read_len;
    if (!fgets(line, MAX_LINE_LENGTH + 2, stdin)){
        return REACHED_EOF;
    } else {
        line_read_len = strlen(line);
        if(line_read_len > MAX_LINE_LENGTH) {
            /*  The line read is longer than the limit (exactly MAX_LINE_LENGTH+1), 
             *  will be treated as an invalid command, 
             *  if end-of-line hasn't been reached, we will
             *  discard the rest of the line - (read until '\n' or EOF)
             */
            if(line[MAX_LINE_LENGTH] != '\n')
                discard_rest_of_line();
            return ILLEGAL_LENGTH_LINE;
        } else {
            if(is_blank_line(line))
                return BLANK_LINE;
            return LEGAL_LENGTH_LINE;
        }
    }
}

/*  
 *  Function: initialize_aux_struct
 *      Functionality:
 *          This function initializes the command struct allocated in start_new_game to
 *          default values.
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct to be initialized.
 *      Return Value:
 *          void
 */
static void initialize_aux_struct(command *cmd_p)
{
    int i;
    cmd_p->cmd_type = DEFAULT_COMMAND;
    for(i=0; i<NUM_OF_PARAMETERS; i++)
        (cmd_p->parameters)[i] = DEFAULT_PARA;
	cmd_p->para =0; 
    cmd_p->file_path = NULL;
}
