
sudoku:	sudoku.o
	g++ -o sudoku sudoku.o

sudoku.o:	sudoku.cpp
	g++ -c sudoku.cpp

clean:
	rm -f *.o sudoku
