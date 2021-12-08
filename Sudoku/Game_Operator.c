#include "Game_Operator.h"
#include "Game_IF.h"

/****************************************** Module ******************************************
 *  Module: Game Operator
 *  Implements: Game_Operator.h
 *  Includes: Game_IF.h
 *  Usage:
 *     This module implements the game operation. It receives a command struct from Main Aux,
 *     it performs parameters and game_state checking and calls the relevant function from
 *     the Game module to perform the command.
 *     Read the definitions_db.h documentation for more information about the command struct.
 *     This module includes some internally used functions for performing the main functinality
 *     of the module.
 *******************************************************************************************/
/* Internally used functions (partial list - others also exist) */
static RETURN_STATUS process_command_init(command *, game_state *);
static RETURN_STATUS process_mark_errors(command *cmd_p, game_state *g_state_p);
static RETURN_STATUS process_hint(command *cmd_p, game_state *g_state_p);
static RETURN_STATUS process_guess_hint(command *cmd_p, game_state *g_state_p);
static RETURN_STATUS process_generate(command *cmd_p, game_state *g_state_p);
static RETURN_STATUS process_set(command *cmd_p, game_state *g_state_p);
static RETURN_STATUS process_guess(command *cmd_p, game_state *g_state_p);
static void print_available_modes(int i);
static void print_invalid_mode(command *cmd_p);
/*  
 *  Function: process_command
 *      Functionality:
 *          This function receives a command struct that was filled by the Parser, it performs
 *          game mode checking, and when needed it calls the function in Game responsible
 *          for performing the command, or another function in this module to perform further 
 *          parameter checking.
 *          -   The command type is neither EXIT nor INVALID_COMMAND, these are already dealed with in
 *              Main_Aux.
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command to be processed
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          Returns PROCSS_SUCCESS when the processing of a command was successful, otherwise returns
 *          PROCESS_FAIL to indicate a fatal error that requires exiting the game happened.
 */
RETURN_STATUS process_command(command *cmd_p, game_state *g_state_p)
{
    if(g_state_p->mode == Init) {
        return process_command_init(cmd_p, g_state_p);
    }
    /* Game mode is either Solve or Edit */
    switch (cmd_p->cmd_type) {
        case SOLVE:	
            return solve(g_state_p, cmd_p->file_path);
        case EDIT:	
            return edit(g_state_p, cmd_p->file_path);
        case PRINT_BOARD:
            print_board(g_state_p);
            return PROCESS_SUCCESS;
        case VALIDATE:	
            return validate(g_state_p);
	case GUESS_HINT:
		if(g_state_p->mode != Solve){
	                print_invalid_mode(cmd_p);
        	        return PROCESS_SUCCESS;
            }
            return process_guess_hint(cmd_p, g_state_p);
        case UNDO:
            return undo(g_state_p);
        case REDO:
            return redo(g_state_p);
        case NUM_SOLUTIONS:
            return num_solutions(g_state_p);
        case RESET:
            return reset(g_state_p);
        case AUTOFILL:
            if(g_state_p->mode != Solve){
                print_invalid_mode(cmd_p);
                return PROCESS_SUCCESS;
            }
            return autofill(g_state_p);
        case SAVE:	
            return save(g_state_p, cmd_p->file_path); 
        case HINT:	
            if(g_state_p->mode != Solve){
                print_invalid_mode(cmd_p);
                return PROCESS_SUCCESS;
            }
            return process_hint(cmd_p, g_state_p);
        case GENERATE:
            if(g_state_p->mode != Edit){
                print_invalid_mode(cmd_p);
                return PROCESS_SUCCESS;
            }
            return process_generate(cmd_p, g_state_p);
		case GUESS:
			if(g_state_p->mode != Solve){
                print_invalid_mode(cmd_p);
                return PROCESS_SUCCESS;
            }
			return process_guess(cmd_p,g_state_p);
        case MARK_ERRORS:	
            if(g_state_p->mode != Solve){
                print_invalid_mode(cmd_p);
                return PROCESS_SUCCESS;
            }
            return process_mark_errors(cmd_p, g_state_p);
        case SET:	
            return process_set(cmd_p, g_state_p);
        default: 
            print_invalid_command();
            return PROCESS_FAIL;
        }
}

/*  
 *  Function: process_command_Init
 *      Functionality:
 *          This command performs the process_command's job when the game mode is Init, it is used because
 *          only a small part of the commands are available in Init mode.
 *          -   The command type is neither EXIT nor INVALID_COMMAND, these are already dealed with in
 *              Main_Aux.
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command to be processed
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          Returns PROCSS_SUCCESS when the processing of a command was successful, otherwise returns
 *          PROCESS_FAIL to indicate a fatal error that requires exiting the game happened.
 */
static RETURN_STATUS process_command_init(command *cmd_p, game_state *g_state_p)
{
    if(cmd_p->cmd_type == SOLVE){
        return solve(g_state_p, cmd_p->file_path);
    } else if (cmd_p->cmd_type == EDIT ) {
        return edit(g_state_p, cmd_p->file_path);
    } else {
        print_invalid_mode(cmd_p);
        return PROCESS_SUCCESS;
    }
}

/*  
 *  Function:   process_set
 *      Functionality:
 *          This command further processes the command when it's of type SET. It performs additional 
 *          parameter checking, issuing a message if any were find to be inconsistent with the current
 *          game_state. Then, it calls the set function in Game. 
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command to be processed
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          Returns PROCSS_SUCCESS when the processing of a command was successful, otherwise returns
 *          PROCESS_FAIL to indicate a fatal error that requires exiting the game happened.
 */
static RETURN_STATUS process_set(command *cmd_p, game_state *g_state_p)
{
    int is_valid_set, N, i;
    N = g_state_p->g_board.N;

    is_valid_set = 1;
    for(i=0; i<3; i++){
        if((cmd_p->parameters)[i] < (1-i/2))
            is_valid_set = 0;
        if((cmd_p->parameters)[i] > N)
            is_valid_set = 0;
    }
    if(is_valid_set){
        return set(g_state_p, (cmd_p->parameters)[0], (cmd_p->parameters)[1], (cmd_p->parameters)[2]);
    }
    printf("Error: value not in range 0-%d\n", N);
    return PROCESS_SUCCESS;
}

/*  
 *  Function:   process_generate
 *      Functionality:
 *          This command further processes the command when it's of type GENERATE. It performs additional 
 *          parameter checking, issuing a message if any were find to be inconsistent with the current
 *          game_state. Then, it calls the generate function in Game. 
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command to be processed
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          Returns PROCSS_SUCCESS when the processing of a command was successful, otherwise returns
 *          PROCESS_FAIL to indicate a fatal error that requires exiting the game happened.
 */
static RETURN_STATUS process_generate(command *cmd_p, game_state *g_state_p)
{
    int is_valid_generate, E, i;
    E = (g_state_p->g_board.N)*(g_state_p->g_board.N);
    
    is_valid_generate = 1;
    for(i=0; i<2; i++){
        if((cmd_p->parameters)[i] < 0)
            is_valid_generate = 0;
        if((cmd_p->parameters)[i] > E)
            is_valid_generate = 0;
    }
    if(is_valid_generate){
        return generate(g_state_p, (cmd_p->parameters)[0], (cmd_p->parameters)[1]);
    }
    printf("Error: value not in range 0-%d\n", E);
    return PROCESS_SUCCESS;
}
    
/*  
 *  Function:   process_hint
 *      Functionality:
 *          This command further processes the command when it's of type HINT. It performs additional 
 *          parameter checking, issuing a message if any were find to be inconsistent with the current
 *          game_state. Then, it calls the hint function in Game. 
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command to be processed
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          Returns PROCSS_SUCCESS when the processing of a command was successful, otherwise returns
 *          PROCESS_FAIL to indicate a fatal error that requires exiting the game happened.
 */
static RETURN_STATUS process_hint(command *cmd_p, game_state *g_state_p)
{
    int is_valid_hint, N, i;
    N = g_state_p->g_board.N;
    
    is_valid_hint = 1;
    for(i=0; i<2; i++){
        if((cmd_p->parameters)[i] < 1)
            is_valid_hint = 0;
        if((cmd_p->parameters)[i] > N)
            is_valid_hint = 0;
    }
    if(is_valid_hint){
        return hint(g_state_p, (cmd_p->parameters)[0], (cmd_p->parameters)[1]);
    }
    printf("Error: value not in range 1-%d\n", N);
    return PROCESS_SUCCESS;
}

/*  
 *  Function:   process_guess_hint
 *      Functionality:
 *          This command further processes the command when it's of type GUESS_HINT. It performs additional 
 *          parameter checking, issuing a message if any were find to be inconsistent with the current
 *          game_state. Then, it calls the GUESS_HINT function in Game. 
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command to be processed
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          Returns PROCSS_SUCCESS when the processing of a command was successful, otherwise returns
 *          PROCESS_FAIL to indicate a fatal error that requires exiting the game happened.
 */
static RETURN_STATUS process_guess_hint(command *cmd_p, game_state *g_state_p)
{
    int is_valid_guess_hint, N, i;
    N = g_state_p->g_board.N;
    
    is_valid_guess_hint = 1;
    for(i=0; i<2; i++){
        if((cmd_p->parameters)[i] < 1)
            is_valid_guess_hint = 0;
        if((cmd_p->parameters)[i] > N)
            is_valid_guess_hint = 0;
    }
    if(is_valid_guess_hint){
        return guess_hint(g_state_p, (cmd_p->parameters)[0], (cmd_p->parameters)[1]);
    }
    printf("Error: value not in range 1-%d\n", N);
    return PROCESS_SUCCESS;
}

/*  
 *  Function:   process_mark_errors
 *      Functionality:
 *          This command further processes the command when it's of type MARK_ERRORS. It performs additional 
 *          parameter checking, issuing a message if any were find to be inconsistent with the current
 *          game_state. Then, it updates the mark_errors parameter.
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command to be processed
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          Returns PROCSS_SUCCESS always, it's so for consistensy with other functions.
 */
static RETURN_STATUS process_mark_errors(command *cmd_p, game_state *g_state_p)
{
    int is_valid_mark_errors;

    is_valid_mark_errors = ((cmd_p->parameters)[0] == 0)||((cmd_p->parameters)[0] == 1);
    if(is_valid_mark_errors) {
        g_state_p->mark_errors = (cmd_p->parameters)[0];
    } else {
        printf("Error: the value should be 0 or 1\n");
    }
    return PROCESS_SUCCESS;
}
/*  
 *  Function:   process_guess
 *      Functionality:
 *          This command further processes the command when it's of type GUESS. It performs additional 
 *          parameter checking, issuing a message if any were find to be inconsistent with the current
 *          game_state.
 *      arameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command to be processed
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          Returns PROCSS_SUCCESS when the processing of a command was successful, otherwise returns
 *          PROCESS_FAIL to indicate a fatal error that requires exiting the game happened.
 */
static RETURN_STATUS process_guess(command *cmd_p, game_state *g_state_p){
	int is_valid_guess;
    
    
    is_valid_guess = 1;
	
    if((cmd_p->para) > 1.0 || (cmd_p->para)<0) is_valid_guess = 0;
	if (is_valid_guess) return guess(g_state_p, cmd_p->para);
	else printf("Error: the value should be from 0 to 1\n");
	return PROCESS_SUCCESS;
}

/*  
 *  Function:   initialize_game_aux_struct
 *      Functionality:
 *          this command initializes the game_state struct at the start of a new game 
 *          setting the mode ti Init, mark errors to 1, and no game_board or moves_list are
 *          valid so valid is 0.
 *      Parameters:
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          void.
 */
void initialize_game_aux_struct(game_state *g_state_p)
{
    g_state_p->mode = Init;
    g_state_p->mark_errors = 1;
    g_state_p->valid = 0;
    return;
}

/*  
 *  Function:   free_game_aux_struct
 *      Functionality:
 *          A wrapper for free_game function in game to bridge between Game and Main_Aux. It frees the 
 *          resources allocated withing the game_board/moves_list structs in the game_state struct.
 *      Parameters:
 *          -   game_state *g_state_p   :   pointer to current game_state struct
 *      Return Value:
 *          void.
 */
void free_game_aux_struct(game_state *g_state_p)
{
    free_game(g_state_p);
    return;
}

/*  
 *  Function:   print_invalid_command
 *      Functionality:
 *          Prints the error message for an invalid command.
 *          Used in this module and in Main_Aux.
 *      Parameters:
 *          none
 *      Return Value:
 *          void.
 */
void print_invalid_command()
{
    printf("ERROR: invalid command\n");
}
/*  
 *  Function:   print_available_mode
 *      Functionality:
 *          Prints the error message for which modes are appropriate
 *      Parameters:
 *          -   int i  :   number representing mode, 0 for both, 1 for Solve, 2 for Edit
 *      Return Value:
 *          void.
 */
 
static void print_available_modes(int i)
{
	switch (i){
		case 0:
			printf("available in Solve and Edit modes\n");
			break;
		case 1:
			printf("available in Solve mode only\n");
			break;
		case 2:
			printf("available in Edit mode only\n");
			break;
	}
}
/*  
 *  Function:   print_invalid_mode
 *      Functionality:
 *          uses switch to decide which modes are avaiable and calls print_available_modes to print them
 *      Parameters:
 *          -   command *cmd_p  :   pointer to command struct that contains command 
 *      Return Value:
 *          void.
 */
 
static void print_invalid_mode(command *cmd_p)
{
    printf("ERROR: command unavailable in current mode, it is ");
	switch(cmd_p->cmd_type){
        case PRINT_BOARD:
            print_available_modes(0);
            break;
        case VALIDATE:	
            print_available_modes(0);
            break;
        case GUESS_HINT:
            print_available_modes(1);
            break;
        case UNDO:
            print_available_modes(0);
            break;
        case REDO:
            print_available_modes(0);
            break;
        case NUM_SOLUTIONS:
            print_available_modes(0);
            break;
        case RESET:
            print_available_modes(0);
            break;
        case AUTOFILL:
            print_available_modes(1);
            break;
        case SAVE:	
            print_available_modes(0);
            break;
        case HINT:	
            print_available_modes(1);
            break;
        case GENERATE:
            print_available_modes(2);
            break;
        case MARK_ERRORS:	
            print_available_modes(1);
            break;
      	case GUESS:
            print_available_modes(1);
	    break;
        case SET:	
            print_available_modes(0);
            break;
	default: 
	    printf("not supposed to be here.\n");
			
	}
	
}
