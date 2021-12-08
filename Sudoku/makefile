CC = gcc
OBJS = ILP_Solver.o LP_Solver.o Back_Tracking_Solver.o Board_Print.o Main.o Game_Operator.o Main_Aux.o Parser.o Game.o Moves_Linked_List.o File_IO.o 
 
EXEC = sudoku-console
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC) all: $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $(EXEC) -lm
ILP_Solver.o: ILP_Solver.c ILP_Solver.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
LP_Solver.o: LP_Solver.c LP_Solver.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Back_Tracking_Solver.o: Back_Tracking_Solver.c Back_Tracking_Solver.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Board_Print.o: Board_Print.c Board_Print.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Parser.o: Parser.c Parser.h 
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Main.o: Main.c Main_Aux.h 
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Main_Aux.o: Main_Aux.c Main_Aux.h Parser.h Game_Operator.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Game_Operator.o: Game_Operator.c Game_Operator.h Game_IF.h 
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Game.o: Game.c Game_IF.h Game_IO_IF.h Moves_Linked_List.h Board_Print.h File_IO.h ILP_Solver.h Back_Tracking_Solver.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Moves_Linked_List.o: Moves_Linked_List.c Moves_Linked_List.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
File_IO.o: File_IO.c File_IO.h Game_IO_IF.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
clean:
	rm -f *.o $(EXEC)
	rm -f *.log *.lp
