#include "Moves_Linked_List.h"

/****************************************** Module ******************************************
 *  Module: Moves Linked List
 *  Implements: Moves_Linked_List.h
 *  Includes: None
 *  Usage:
 *     This module implements the functions used on the moves list. It is doubly circular
 *     linked list with a sentinel.Read the definitions_db.h documentation first to understand
 *     the structs used in the list.
 *     It is used by the Game module to perform all operations regarding the moves linked
 *     list.
 *******************************************************************************************/

/*  
 *  Function: alloc_move_node
 *      Functionality:
 *          Allocates an empty move_node structure that represents 
 *          a move in the moves linked list.
 *      Parameters:
 *          -   move_node **m_node_p_p  :   pointer to a pointer to a move_node that will get updated.
 *      Return Value:
 *          ALLOC_FAIL if couldn't allocate memory, ALLOC_SUCCESS if allocation was successfull.
 */
RETURN_STATUS alloc_move_node(move_node **m_node_p_p)
{
    move_node *m_node_p;
    m_node_p = (move_node *) malloc(sizeof(move_node));
    if(!m_node_p)
        return ALLOC_FAIL;
    m_node_p->next = NULL;
    m_node_p->prev = NULL;
    m_node_p->changes_list_head = NULL;
    m_node_p->changes_list_tail = NULL;
    *m_node_p_p = m_node_p;
    return ALLOC_SUCCESS;
}

/*  
 *  Function: free_move_node
 *      Functionality:
 *          Frees the move_node structure pointed to by the argument. It also de-allocates all the 
 *          change_node structures within the move node.
 *      Parameters:
 *          -   move_node *m_node_p : pointer to a move_node that will get freed.
 *      Return Value:
 *          void
 */
void free_move_node(move_node *m_node_p)
{
    change_node *c_node_p;
    /* free changes associated with move */
    c_node_p = m_node_p->changes_list_head;
    while(c_node_p){
        c_node_p = c_node_p->next;
        free(m_node_p->changes_list_head);
        m_node_p->changes_list_head = c_node_p;
    }
    free(m_node_p);
    return;
}

/*  
 *  Function: add_change_to_move
 *      Functionality:
 *          Allocates and adds a change_node to the already allocated (*move_node_p). The move_node
 *          is supposed to be already allocated, and the change_node is added to the tail of the change_node's
 *          linked list of the move_node.
 *      Parameters:
 *          -   move_node *m_node_p  :   pointer to a move_node that the change_node will be added to
 *          -   int cell_rows   :   the cell_rows member of the change_node struct to be allocated 
 *          -   int cell_cols   :   the cell_cols member of the change_node struct to be allocated 
 *          -   int z1          :   the z1 member of the change_node struct to be allocated 
 *          -   int z2          :   the z2 member of the change_node struct to be allocated 
 *      Return Value:
 *          ALLOC_FAIL if couldn't allocate memory, ALLOC_SUCCESS if allocation was successfull.
 */
RETURN_STATUS add_change_to_move(move_node *m_node_p, int cell_rows, int cell_cols, int z1, int z2)
{
    change_node *c_node_p;
    c_node_p = (change_node *) malloc(sizeof(change_node));
    if(!c_node_p)
        return ALLOC_FAIL;
    /* initialize change_node */
    c_node_p->cell_rows = cell_rows;
    c_node_p->cell_cols = cell_cols;
    c_node_p->z1 = z1;
    c_node_p->z2 = z2;
    c_node_p->next = NULL;
    /* add change_node to end of move_node's changes list */
    if(m_node_p->changes_list_tail == NULL) {
        m_node_p->changes_list_head = c_node_p;
        m_node_p->changes_list_tail = c_node_p;
    } else {
        m_node_p->changes_list_tail->next = c_node_p;
        m_node_p->changes_list_tail = c_node_p;
    }
    return ALLOC_SUCCESS;
}

/*  
 *  Function: add_move_node_to_list
 *      Functionality:
 *          Addes the move_node pointed to by m_node_p to the linked list after the node under curr_pointer 
 *          after deleting all the moves (and freeing their resources) between the current move and the end 
 *          of the list (or the sentinel because the list is circular).
 *          Also updates the curr_pointer to the move_node being added.
 *      Parameters:
 *          -   moves_list *m_list_p    :  pointer to the movs_list that will have the move_node added to 
 *          -   move_node *m_node_p  :   pointer to the move_node that will be added to the list
 *      Return Value:
 *          void
 */
void add_move_node_to_list(moves_list *m_list_p, move_node *m_node_p)
{
    /* delete all moves after the move in the curr_pointer */
    while(m_list_p->curr_pointer->next != &(m_list_p->sentinel))
        delete_move_node(m_list_p->curr_pointer->next);
    /* inserting the move_node after the curr_pointer as the last node of the list */
    m_node_p->next = &(m_list_p->sentinel);
    m_node_p->prev = m_list_p->curr_pointer;
    m_list_p->curr_pointer->next = m_node_p;
    m_list_p->sentinel.prev = m_node_p;
    /*updating the curr_pointer */
    m_list_p->curr_pointer = m_node_p;
    return;
}

/*  
 *  Function: can_redo
 *      Functionality:
 *          Checks if a redo operation is possible on the current moves_list of the game, 
 *          meaning their are moves to redo.
 *      Parameters:
 *          -   moves_list *m_list_p    :  pointer to the movs_list that the query is upon
 *      Return Value:
 *          1 if can perform redo, otherwise 0.
 */
int can_redo(moves_list *m_list_p)
{
    return (m_list_p->curr_pointer->next != &(m_list_p->sentinel));
}

/*  
 *  Function: can_undo
 *      Functionality:
 *          Checks if an undo operation is possible on the current moves_list of the game, 
 *          meaning their are moves to undo.
 *      Parameters:
 *          -   moves_list *m_list_p    :  pointer to the movs_list that the query is upon
 *      Return Value:
 *          1 if can perform undo, otherwise 0.
 */
int can_undo(moves_list *m_list_p)
{
    return (m_list_p->curr_pointer != &(m_list_p->sentinel));
}

/*  
 *  Function: inc_curr_pointer
 *      Functionality:
 *          Moves the curr_pointer to point to the next move_node in the list.
 *          Should be used only if can_redo(m_list_p).
 *          See documentation of the function: get_curr_pointer_changes_iter
 *      Parameters:
 *          -   moves_list *m_list_p    :  pointer to the movs_list that will have curr_pointer incremented
 *      Return Value:
 *          void
 */
void inc_curr_pointer(moves_list *m_list_p)
{
    m_list_p->curr_pointer = m_list_p->curr_pointer->next;
    return;
}

/*  
 *  Function: dec_curr_pointer
 *      Functionality:
 *          Moves the curr_pointer to point to the prevous move_node in the list.
 *          Should be used only if can_undo(m_list_p).
 *          See documentation of the function: get_curr_pointer_changes_iter
 *      Parameters:
 *          -   moves_list *m_list_p    :  pointer to the movs_list that will have curr_pointer decremented.
 *      Return Value:
 *          void
 */
void dec_curr_pointer(moves_list *m_list_p)
{
    m_list_p->curr_pointer = m_list_p->curr_pointer->prev;
    return;
}

/*  
 *  Function: get_curr_pointer_changes_iter
 *      Functionality:
 *          Returns the head of the changes_list associated with the move pointed
 *          to by the curr_pointer. The changes can be traversed by using the next 
 *          member of change_node until a NULL is reached indicating the end of the changes 
 *          list.  The curr_pointer shouldn't be pointing to the sentinel, 
 *      Usage:    
 *          This function is used by redo/undo functions to get the list of the change_nodes associated
 *          with the move pointed to by the curr_pointer. To perform a redo/unde, this function shall be called 
 *          in the following sequences with other functions defined above: 
 *              -   can_redo, inc_curr_pointer, get_curr_pointer_changes_iter
 *              -   can_undo, get_curr_pointer_changes_iter, dec_curr_pointer
 *      Parameters:
 *          -   moves_list *m_list_p    :  pointer to the movs_list containing the curr_pointer.
 *      Return Value:
 *          pointer to the head of the change_nodes list of the move under curr_pointer.
 */
change_node *get_curr_pointer_changes_iter(moves_list *m_list_p)
{
    return m_list_p->curr_pointer->changes_list_head;
}

/*  
 *  Function: init_moves_list
 *      Functionality:
 *          initializes the moves_list structure to have only the sentinel pointed to by
 *          curr_pointer.
 *      Parameters:
 *          -   moves_list *m_list_p : pointer to a moves_list structure that wil get initialized.
 *      Return Value:
 *          void
 */
void init_moves_list(moves_list *m_list_p)
{
    m_list_p->sentinel.next =  &(m_list_p->sentinel);
    m_list_p->sentinel.prev =  &(m_list_p->sentinel);
    m_list_p->sentinel.changes_list_head = NULL;
    m_list_p->sentinel.changes_list_tail = NULL;
    m_list_p->curr_pointer = &(m_list_p->sentinel);
}

/*  
 *  Function: delete_move_node
 *      Functionality:
 *          It removes the move_node from the moves_list that contains it, and frees the move_node structure
 *          pointed to by the argument. It also de-allocates all the change_node structures within the move node.
 *      Parameters:
 *          -   move_node *m_node_p : pointer to a move_node that will get deleted and freed
 *      Return Value:
 *          void
 */
void delete_move_node(move_node *m_node_p)
{
    m_node_p->prev->next = m_node_p->next;
    m_node_p->next->prev = m_node_p->prev;
    free_move_node(m_node_p);
    return;
}

/*  
 *  Function: free_moves_list
 *      Functionality:
 *          Frees all move_node structures in the moves_list and leaves the sentinel, at the end 
 *          the structure is the same as after a call to init_moves_list.
 *       *****************************************************************************************************
 *       *   We don't de-allocate the moves_list structure or the sentinel because these get statically      *
 *       *   allocated once for the whole game operation in Main_Aux.c                                       *
 *       *****************************************************************************************************
 *      Parameters:
 *          -   moves_list *m_list_p : pointer to a moves_list structure that wil get freed.
 *      Return Value:
 *          void
 */
void free_moves_list(moves_list *m_list_p)
{   
    while(m_list_p->sentinel.next != &(m_list_p->sentinel))
        delete_move_node(m_list_p->sentinel.next);
    m_list_p->curr_pointer = &(m_list_p->sentinel);
    return;
}
