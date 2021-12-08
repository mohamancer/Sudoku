#include "ILP_Solver.h"
#include "gurobi_c.h" 

/****************************************** Module ******************************************
 *  Module: ILP Solver
 *  Implements: ILP_Solver.h
 *  Includes: gurobi_c.h
 *  Usage:
 *      This modules implements the ILP algorithm that solves the board using Gurobi.
 *      It include functions used internally to communicate with the gurobi package 
 *      and to aid in performing the solve_board_with_ILP function.
 *      Other functions are also defined at the end to add constraints more effictively.
 *******************************************************************************************/

/* Internally used functions (partial list - others also exist) */
static RETURN_STATUS create_environment(GRBenv **, char *);
static RETURN_STATUS create_model(GRBenv *, GRBmodel **, char *);
static RETURN_STATUS set_LP_problem(GRBmodel *, int **, int , int);
static RETURN_STATUS add_constraints(GRBenv *,GRBmodel *, int **, int, int);
static RETURN_STATUS optimize_model(GRBmodel *);
static RETURN_STATUS get_solution_status(GRBmodel *, int *);
static RETURN_STATUS set_board_solution(GRBmodel *, int **, int , int );
static RETURN_STATUS free_model(GRBmodel *);

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
RETURN_STATUS solve_board_with_ILP(int **board, int block_rows, int block_cols)
{
    GRBenv *env = NULL;
    GRBmodel *model = NULL;
    int game_status, error;
    RETURN_STATUS status, returned_status;
    status = create_environment(&env, NULL);
    if(status != ILP_SUCCESS){
        return status;
    }
    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if(error){
        printf("Couldn't set the log to console parameter to 0\n");
        return ILP_FAIL;
    }
    status = create_model(env, &model, "board_solving_model");
    if(status != ILP_SUCCESS){
        return status;
    }
    status = set_LP_problem(model, board, block_rows, block_cols);
    if(status != ILP_SUCCESS){
        return status;
    }
    status = optimize_model(model); 
    if(status != ILP_SUCCESS){
        return status;
    }
    status = get_solution_status(model, &game_status); 
    if(status != ILP_SUCCESS){
        return status;
    }
    if(game_status != GRB_OPTIMAL){
        returned_status = BOARD_UNSOLVABLE;
    } else {
        returned_status = BOARD_SOLVABLE;
        status = set_board_solution(model, board, block_rows, block_cols);
        if(status != ILP_SUCCESS){
            return status;
        }
    }
    /*free model and environment*/
    status = free_model(model);
    if(status != ILP_SUCCESS){
        return status;
    }
    GRBfreeenv(env);
    return returned_status;
}

/*
 *  Function to update board with the solution of it 
 *  provided that the optimizer managed to find a solution to 
 *  the problem.
 */ 
static RETURN_STATUS set_board_solution(GRBmodel *model, int **board, int block_rows, int block_cols)
{
    int error, num_of_variables, N, i, j, k;
    double *sol;
    N = block_rows*block_cols;
    num_of_variables = N*N*N;
    sol = malloc(sizeof(double)*num_of_variables);
    if(sol == NULL){
        printf("allocation error in set_board_solution\n");
        return ALLOC_FAIL;
    }
    error = GRBgetdblattrarray( model, 
            GRB_DBL_ATTR_X,
            0,
            num_of_variables,
            sol); 
    if(error){
        printf("Error: could not get solution\n");
        printf("ERROR: %s\n", GRBgeterrormsg(GRBgetenv(model)));
        return ILP_FAIL;
    }
    for(i=0; i<N; i++)
        for(j=0; j<N; j++)
            for(k=0; k<N; k++){
                if(sol[i*N*N+j*N+k] == 1.0){
                    board[i][j] = k+1;
                }
            }
    free(sol);
    return ILP_SUCCESS;
}

/*
 *  Function to get the solution status of the ILP
 *  problem after trying to solve it.
 *  -   the solution gets put to *status_p.
 *  -   Uses the integer attribute getter of the 
 *      Gurobi optimizer.
 */ 
static RETURN_STATUS get_solution_status(GRBmodel *model, int *status_p)
{
    int error;
    GRBenv *env = GRBgetenv(model);
    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, status_p);
    if(error){
        printf("Error: could not get solution status\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    return ILP_SUCCESS;
}

/*
 *  Function to optimize the model, meaning trying to find
 *  a feasible solution to the constraints provided in previous calls
 *  to other functions in set_LP_problem.
 */ 
static RETURN_STATUS optimize_model(GRBmodel *model)
{
    int error = 0;
    GRBenv *env = GRBgetenv(model);
    error = GRBoptimize(model);
    if(error){
        printf("Error: could not optimize model\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    return ILP_SUCCESS;
}

/* 
 * Create environment for the Gurobi model,
 * log gets printed to log_file_name.
 */
static RETURN_STATUS create_environment(GRBenv **env_p, char *log_file_name)
{
    int error = 0;
    error = GRBloadenv(env_p, log_file_name);
    if (error || (*env_p == NULL)) {
        printf("%s\n", GRBgeterrormsg(*env_p));
        printf("Error: could not create environment\n");
        return ILP_FAIL;
    }
    return ILP_SUCCESS;
}

/*
 * Create model for the Gurobi optimizer.
 */
static RETURN_STATUS create_model(GRBenv *env, GRBmodel **model_p, char *model_name)
{
    int error = 0;
    error = GRBnewmodel(env, model_p, model_name, 0, NULL, NULL, NULL, NULL, NULL);
    if(error || (*model_p == NULL)){
        printf("Error: could not create model\n");
        printf("ERROR: %s\n", GRBgeterrormsg(GRBgetenv(*model_p)));
        return ILP_FAIL;
    }
    return ILP_SUCCESS;
}

/*
 *  Function to free the model used by the optimizer.
 */
static RETURN_STATUS free_model(GRBmodel *model)
{
    int error;
    GRBenv *env = GRBgetenv(model);
    error = GRBfreemodel(model);
    if(error){
        printf("Error: could not free model\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    return ILP_SUCCESS;
}

/*
 *  Function adds variables to the model and sets the objective function.
 *  -   model has N*N*N BINARY variables - X i,j,k
 *  -   X i,j,k gets value 1 in the optimized solution iff
 *      in the solution to the sudoku the cell at row i and
 *      column j gets the value of (k+1).
 *      i,j,k in the range of 1...N where N is block_rows X block_cols
 *  -   the objective function is (0.0). - no variables, so it's always bounded
 *      just need to find a feasible solution.
 */ 
static RETURN_STATUS add_variables(GRBenv *env, GRBmodel *model, int N)
{
    int error, num_of_variables, i;
    char *vtype;

    num_of_variables = N*N*N;
    vtype = (char *) calloc(num_of_variables, sizeof(char));
    if(vtype == NULL){
        printf("Can't allocate memory for vtype in add_variables\n");
        return ALLOC_FAIL;
    }
    /*Setting variable types to binary*/
    for(i=0; i<num_of_variables; i++)
        vtype[i] = GRB_BINARY;
    error = GRBaddvars( model,
            num_of_variables,
            0,                      /* no non zero coefficient in objective function. */
            NULL,                   /* default values set objective function to zero, */
            NULL,                   /* with variables of binary type with lower limit */
            NULL,                   /* of zero and no upper limit - non needed. */
            NULL,                   /* no names given to variables. */
            NULL,
            NULL,
            vtype,
            NULL
            );
    if(error){
        printf("Error: could not add variables\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        free(vtype);
        return ILP_FAIL;
    }
    free(vtype);
    return ILP_SUCCESS;
}            

/*
 * Function to set the environment for the LP model.
 * -    first adds variables, updates the MODEL_SENSE attribute of the model,
 *      updates the model to the variables addition then adds constraints.
 */

static RETURN_STATUS set_LP_problem(GRBmodel *model, int **board, int block_rows, int block_cols)
{
    int N, error;
    RETURN_STATUS status;
    GRBenv *env = GRBgetenv(model);
    N = block_rows * block_cols;

    /*adding variables */
    status = add_variables(env, model, N);
    if(status != ILP_SUCCESS)
        return status;
    /* setting the problem to maximization of the objective */
    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if(error){
        printf("Error: could not set model sense attribute\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    /*updating model with the new objective and variables */
    error = GRBupdatemodel(model);
    if(error){
        printf("Error: could not integrate variables\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    /*adding the constraints to the model */
    status = add_constraints(env, model, board, block_rows, block_cols);
    return status;
}

/*
 *  Structure: constraints_packet
 *  Usage:
 *      Used by the functions that add constraints to save space in 
 *      function arguments.
 */
typedef struct { 
    int *cbeg; 
    int *cind;
    double *cval;
    double *rhs;
    char *sense;
    int num_of_consts;
    int num_of_variables;
} constraints_packet;

/*
 *  Aux. function used by the 4 regular constraint adders to call a GRB constraint adding 
 *  function.
 *  Implemented separately to avoid code duplication
 *
 */
static int call_GRB_add_constraints(GRBmodel *model, constraints_packet *c_pkt_p)
{
    int error;
    error = GRBaddconstrs(  model,
                            c_pkt_p->num_of_consts,
                            c_pkt_p->num_of_variables,
                            c_pkt_p->cbeg,
                            c_pkt_p->cind,
                            c_pkt_p->cval,
                            c_pkt_p->sense,
                            c_pkt_p->rhs,
                            NULL
                         );
    return error;
}

/* 
 *  Function to add sanity constraints.
 *  adds constraints for each cell to have exactly one value.
 *  meaning that for each i,j in 1...N, only one k exists s.t.
 *  X i,j,k is 1 in the solution.
 */ 
static RETURN_STATUS add_sanity_constraints(GRBenv *env, GRBmodel *model, constraints_packet *c_pkt_p)
{
    int error, i;

    for(i=0; i<(c_pkt_p->num_of_variables); i++)
        (c_pkt_p->cind)[i] = i;
    error = call_GRB_add_constraints(model, c_pkt_p);
    if(error){
        printf("Error: could not add sanity constraints\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    return ILP_SUCCESS;
}

/* 
 *  Function to add rows constraints to the model.
 *  for each row on the board, and for each value k,
 *  a constraints of having exactly one cell in the row with
 *  value k gets added.
 */ 
static RETURN_STATUS add_rows_constraints(GRBenv *env, GRBmodel *model, int N, constraints_packet *c_pkt_p)
{
    int error, i, j ,counter, k;

    counter = 0;
    for(i=0; i<N; i++)
        for(j=0; j<N; j++)
            for(k=0; k<N; k++){
                (c_pkt_p->cind)[counter] = i*(c_pkt_p->num_of_consts) + k*N + j;
                counter ++;
            }
    error = call_GRB_add_constraints(model, c_pkt_p);
    if(error){
        printf("Error: could not add rows constraints\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    return ILP_SUCCESS;
}

/* 
 *  Function to add column constraints to the model.
 *  for each column on the board, and for each value k,
 *  a constraints of having exactly one cell in the column with
 *  value k gets added.
 */ 
static RETURN_STATUS add_cols_constraints(GRBenv *env, GRBmodel *model, int N, constraints_packet *c_pkt_p)
{
    int error, i, j ,counter, k;

    counter = 0;
    for(i=0; i<N; i++)
        for(j=0; j<N; j++)
            for(k=0; k<N; k++){
                (c_pkt_p->cind)[counter] = k*(c_pkt_p->num_of_consts) + i*N + j;
                counter ++;
            }
    error = call_GRB_add_constraints(model, c_pkt_p);
    if(error){
        printf("Error: could not add cols constraints\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    return ILP_SUCCESS;
}

/*
 *  function to add block constraints. 
 *  for each value k in 1...N and for each block in the board,
 *  a constraint of having exactly one cell of value k in the block gets
 *  added.
 */ 
static RETURN_STATUS add_blocks_constraints(GRBenv *env, GRBmodel *model, int N, int block_rows, int block_cols, constraints_packet *c_pkt_p)
{
    int error, i, j ,counter, k;
    int block_row_num, block_col_num;

    counter = 0;
    for(k=0; k<N; k++)
        for(block_row_num=0; block_row_num<block_cols; block_row_num++)
            for(block_col_num=0; block_col_num<block_rows; block_col_num++)
                for(i=0; i<block_cols; i++)
                    for(j=0; j<block_rows; j++){
                        (c_pkt_p->cind)[counter] = (block_row_num*block_rows + j)*N*N + 
                                                    (block_col_num*block_cols + i)*N + k;
                        counter ++;
                    }
    error = call_GRB_add_constraints(model, c_pkt_p);
    if(error){
        printf("Error: could not add block constraints\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    return ILP_SUCCESS;
}

/*
 * Function to free the pointers allocated within a constraint packet.
 */
static void free_packet(constraints_packet *c_pkt_p)
{
    free(c_pkt_p->cbeg);
    free(c_pkt_p->cind);
    free(c_pkt_p->cval);
    free(c_pkt_p->sense);
    free(c_pkt_p->rhs);
}

/*
 *  function to add regular constraints. 
 *  These are the constraints that are needed for all sudoku boards regardless of the currently filled
 *  cells in the board. There are 4 types. The main components of the constraint addition are 
 *  implemented in this function, the element specific to each constraint type is implemented in
 *  a separate function for each of the four constraint type, where they are explained.
 *  -   A struct of type constraints_packet (in header) gets declared and filled and passed to
 *      each of the constraints functions.
 */ 
static RETURN_STATUS add_regular_constraints(GRBenv *env, GRBmodel *model, int N, int block_rows, int block_cols)
{
    int i;
    RETURN_STATUS status;
    constraints_packet c_pkt;


    c_pkt.num_of_consts = N*N;
    c_pkt.num_of_variables = N*N*N;

    c_pkt.cind = (int *) malloc(sizeof(int)*(c_pkt.num_of_variables));
    if(c_pkt.cind == NULL){
        printf("allocation error in add_regular_constraints\n");
        return ALLOC_FAIL;
    }

    c_pkt.cbeg = (int *) malloc(sizeof(int)*(c_pkt.num_of_consts));
    if(c_pkt.cbeg == NULL){
        printf("allocation error in add_regular_constraints\n");
        return ALLOC_FAIL;
    }
    for(i=0; i<(c_pkt.num_of_consts); i++)
        (c_pkt.cbeg)[i] = i*N;

    c_pkt.cval = (double *) malloc(sizeof(double)*(c_pkt.num_of_variables));
    if(c_pkt.cval == NULL){
        printf("allocation error in add_regular_constraints\n");
        return ALLOC_FAIL;
    }
    for(i=0; i<(c_pkt.num_of_variables); i++)
        (c_pkt.cval)[i] = 1.0;

    c_pkt.sense = (char *) malloc(sizeof(char)*(c_pkt.num_of_consts));
    if(c_pkt.sense == NULL){
        printf("allocation error in add_regular_constraints\n");
        return ALLOC_FAIL;
    }
    for(i=0; i<(c_pkt.num_of_consts); i++)
        (c_pkt.sense)[i] = GRB_EQUAL;

    c_pkt.rhs = (double *) malloc(sizeof(double)*(c_pkt.num_of_consts));
    if(c_pkt.rhs == NULL){
        printf("allocation error in add_regular_constraints\n");
        return ALLOC_FAIL;
    }
    for(i=0; i<(c_pkt.num_of_consts); i++)
        (c_pkt.rhs)[i] = 1.0;

    status = add_sanity_constraints(env, model, &c_pkt);
    if(status != ILP_SUCCESS){
        free_packet(&c_pkt);
        return status;
    }
    status = add_rows_constraints(env, model, N, &c_pkt);
    if(status != ILP_SUCCESS){
        free_packet(&c_pkt);
        return status;
    }
    status = add_cols_constraints(env, model, N, &c_pkt);
    if(status != ILP_SUCCESS){
        free_packet(&c_pkt);
        return status;
    }
    status = add_blocks_constraints(env, model, N, block_rows, block_cols, &c_pkt);
    free_packet(&c_pkt);
    return status;
}

/*
 * Function to add current board constraints.  
 * if board[i][j] has a non-zero value k+1, meaning it's already filled,
 * then a constraint (X i,j,k = 1) gets added to the model.
 */ 
static RETURN_STATUS add_current_board_constraints(GRBenv *env, GRBmodel *model, int **board, int N)
{
    int num_of_filled_cells,i,j, error, counter;
    int *cbeg, *cind;
    double *cval, *rhs;  
    char *sense;

    num_of_filled_cells = 0;
    for(i=0; i<N; i++)
        for(j=0; j<N; j++)
            num_of_filled_cells += (board[i][j] ? 1 : 0);
    cbeg = (int *) malloc(sizeof(int)*num_of_filled_cells);
    if(cbeg == NULL){
        printf("allocation error in add_current_board_constraints\n");
        return ALLOC_FAIL;
    }
    for(i=0; i<num_of_filled_cells; i++)
        cbeg[i] = i;
    cind = (int *) malloc(sizeof(int)*num_of_filled_cells);
    if(cind == NULL){
        printf("allocation error in add_current_board_constraints\n");
        return ALLOC_FAIL;
    }
    counter = 0;
    for(i=0; i<N; i++)
        for(j=0; j<N; j++)
            if(board[i][j]){
                cind[counter] = i*N*N+j*N+board[i][j]-1;
                counter++;
            }
    cval = (double *) malloc(sizeof(double)*num_of_filled_cells);
    if(cval == NULL){
        printf("allocation error in add_current_board_constraints\n");
        return ALLOC_FAIL;
    }
    for(i=0; i<num_of_filled_cells; i++)
        cval[i] = 1.0;
    sense = (char *) malloc(sizeof(char)*num_of_filled_cells);
    if(sense == NULL){
        printf("allocation error in add_current_board_constraints\n");
        return ALLOC_FAIL;
    }
    for(i=0; i<num_of_filled_cells; i++)
        sense[i] = GRB_EQUAL;
    rhs = (double *) malloc(sizeof(double)*num_of_filled_cells);
    if(rhs == NULL){
        printf("allocation error in add_current_board_constraints\n");
        return ALLOC_FAIL;
    }
    for(i=0; i<num_of_filled_cells; i++)
        rhs[i] = 1.0;
    error = GRBaddconstrs(  model,
            num_of_filled_cells,
            num_of_filled_cells,
            cbeg,
            cind,
            cval,
            sense,
            rhs,
            NULL
            );
    if(error){
        printf("Error: could not add current board constraints\n");
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        return ILP_FAIL;
    }
    free(cbeg);
    free(cind);
    free(cval);
    free(sense);
    free(rhs);
    return ILP_SUCCESS; 
}

/* 
 *  Function to add the constraints to the model.
 *  -   There are 5 kinds of constraints, four of them are added in a separate function and other one in
 *      a separate function.
 *  -   Explanation for each constraint type is in the header of each function.
 */
static RETURN_STATUS add_constraints(GRBenv *env, GRBmodel *model, int **board, int block_rows, int block_cols)
{   
    int N;
    RETURN_STATUS status;
    N = block_rows * block_cols;
    status = add_regular_constraints(env, model, N, block_rows, block_cols);
    if(status != ILP_SUCCESS)
        return status;
    status = add_current_board_constraints(env, model, board, N);
    return status;
}
