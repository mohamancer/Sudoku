
#include "Main_Aux.h"

/*  
 *  Function: main  
 *      Functionality:
 *          The main function of the executable. It seeds the RNG, prints starting/ending
 *          messages and calls the start_new_game function in Main_Aux
 *      Parameters:
 *          none
 *      Return Value:
 *          none
 */
int main()
{   
    srand(time(NULL));

    printf("Start Sudoku\n");

    /* Calls Main_Aux */
    start_new_game();

    printf("Exiting\n");
    return 0;
}

