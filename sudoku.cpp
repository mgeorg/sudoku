
#include <stdio.h>
#include <stdlib.h>

class square3 {
  public:
  square3();
  void clear();
  int get();
  bool set(int val);
  bool setImpossible(int val);
  bool data[9];
};

square3::square3() {
  clear();
}

void square3::clear() {
  for(int i=0;i<9;i++) {
    data[i] = true;
  }
}

int square3::get() {
  int first=-1;
  int last=-2;
  for(int i=0;i<9;i++) {
    if(data[i]) {
      if(first == -1) {
        first = i;
      }
      last = i;
    }
  }
  if(last == -2) {
    printf("ERROR, nothing possible\n");
    exit(1);
  }
  if(first == last) {
    return first;
  }
  return -1;
}

bool square3::set(int val) {
  bool changed = false;
  if(val >= 0 && val < 9) {
    if(val == get()) return false; // so we don't set changed flag
    for(int i=0;i<9;i++) {
      data[i] = false;
    }
    data[val] = true;
    changed = true;
  }
  return changed;
}

bool square3::setImpossible(int val) {
  bool changed = false;
  if(val >= 0 && val < 9) {
    if(data[val]) {
      data[val] = false;
      changed = true;
    }
  }
  return changed;
}

class sudoku3;
typedef void (sudoku3::*sudoku3Helper)(int rows[], int cols[]);
#define CALL_SUDOKU3HELPER(object,function) ((object).*(function))

class sudoku3 {
  public:
  sudoku3();
  void removeImpossible();
  void findOneSquarePossible();
  void only2SquaresCanBeNumberHelper(int rows[], int cols[]);
  void only2SquaresHavePossibilityHelper(int rows[], int cols[]);
  void doRowColTile(sudoku3Helper helper);
//  void doRowColTile(void (sudoku3::*helper)(int rows[], int cols[]));
  void only2SquaresCanBeNumber();
  void only2SquaresHavePossibility();
  void print();
  void printDetail();
  void printDetailAll();
  void initialize(int *vals);
  void setSquare(int row, int col, int val);
  void setSquareImpossible(int row, int col, int val);

  square3 square[81];
  bool changed;
};

sudoku3::sudoku3() {
  changed = false;
  for(int i=0;i<81;i++) {
    square[i].clear();
  }
}

void sudoku3::setSquare(int row, int col, int val) {
  // careful of side effects
  changed = square[col*9+row].set(val) || changed;
}

void sudoku3::setSquareImpossible(int row, int col, int val) {
  // careful of side effects
  changed = square[col*9+row].setImpossible(val) || changed;
}

void sudoku3::removeImpossible() {
  for(int col=0;col<9;col++) {
    for(int row=0;row<9;row++) {
      // check same row
      for(int cur=0;cur<9;cur++) {
        if(cur != row) {
          setSquareImpossible(row,col,square[col*9+cur].get());
        }
      }
      // check same col
      for(int cur=0;cur<9;cur++) {
        if(cur != col) {
          setSquareImpossible(row,col,square[cur*9+row].get());
        }
      }
      // check same square
      for(int curR=(row/3)*3;curR<(row/3)*3+3;curR++) {
        for(int curC=(col/3)*3;curC<(col/3)*3+3;curC++) {
          if(curC != col || curR != row) {
            setSquareImpossible(row,col,square[curC*9+curR].get());
          }
        }
      }
    }
  }
}

void sudoku3::findOneSquarePossible() {
  int first[9];
  int last[9];
  // check each column
  for(int col=0;col<9;col++) {
    for(int i=0;i<9;i++) {
      first[i] = -1;
      last[i] = -2;
    }
    for(int cur=0;cur<9;cur++) {
      for(int i=0;i<9;i++) {
        if(square[col*9+cur].data[i]) {
          if(first[i] == -1) {
            first[i] = cur;
          }
          last[i] = cur;
        }
      }
    }
    for(int i=0;i<9;i++) {
      if(first[i] == last[i]) {
        setSquare(first[i],col,i);
      }
    }
  }
  // check each row
  for(int row=0;row<9;row++) {
    for(int i=0;i<9;i++) {
      first[i] = -1;
      last[i] = -2;
    }
    for(int cur=0;cur<9;cur++) {
      for(int i=0;i<9;i++) {
        if(square[cur*9+row].data[i]) {
          if(first[i] == -1) {
            first[i] = cur;
          }
          last[i] = cur;
        }
      }
    }
    for(int i=0;i<9;i++) {
      if(first[i] == last[i]) {
        setSquare(row,first[i],i);
      }
    }
  }
  // check each 3x3 tile
  for(int tile=0;tile<9;tile++) {
    int tileR=(tile%3)*3;
    int tileC=(tile/3)*3;
    for(int i=0;i<9;i++) {
      first[i] = -1;
      last[i] = -2;
    }
    for(int cur=0;cur<9;cur++) {
      int curR=(cur%3);
      int curC=(cur/3);
      for(int i=0;i<9;i++) {
        if(square[(tileC+curC)*9+tileR+curR].data[i]) {
          if(first[i] == -1) {
            first[i] = cur;
          }
          last[i] = cur;
        }
      }
    }
    for(int i=0;i<9;i++) {
      if(first[i] == last[i]) {
        int curR=(first[i]%3);
        int curC=(first[i]/3);
        setSquare(tileR+curR,tileC+curC,i);
      }
    }
  }
}

void sudoku3::only2SquaresCanBeNumber() {
  doRowColTile(&sudoku3::only2SquaresCanBeNumberHelper);
}

void sudoku3::only2SquaresHavePossibility() {
  doRowColTile(&sudoku3::only2SquaresHavePossibilityHelper);
}

// removes other possiblilities that those squares could have been
void sudoku3::only2SquaresCanBeNumberHelper(int rows[], int cols[]) {
  int first[9];
  int second[9];
  int last[9];
  for(int i=0;i<9;i++) {
    first[i] = -1;
    second[i] = -1;
    last[i] = -2;
  }
  for(int cur=0;cur<9;cur++) {
    for(int i=0;i<9;i++) {
      if(square[cols[cur]*9+rows[cur]].data[i]) {
        if(first[i] == -1) {
          first[i] = cur;
        } else if(second[i] == -1) {
          second[i] = cur;
        }
        last[i] = cur;
      }
    }
  }
  for(int i=0;i<9;i++) {
    if(second[i] == last[i]) {
      for(int k=i+1;k<9;k++) {
        if(first[i] == first[k] && second[i] == second[k] &&
           last[k] == second[k]) {
          for(int j=0;j<9;j++) {
            if(j != i && j != k) {
              setSquareImpossible(rows[first[i]],cols[first[i]],j);
              setSquareImpossible(rows[second[i]],cols[second[i]],j);
            }
          }
        }
      }
    }
  }
}

// removes other squares that have those possibilities
void sudoku3::only2SquaresHavePossibilityHelper(int rows[], int cols[]) {
  int first[9];
  int second[9];
  int last[9];
  for(int i=0;i<9;i++) {
    first[i] = -1;
    second[i] = -1;
    last[i] = -2;
  }
  for(int i=0;i<9;i++) {
    for(int cur=0;cur<9;cur++) {
      if(square[cols[cur]*9+rows[cur]].data[i]) {
        if(first[cur] == -1) {
          first[cur] = i;
        } else if(second[cur] == -1) {
          second[cur] = i;
        }
        last[cur] = i;
      }
    }
  }
  for(int cur=0;cur<9;cur++) {
    if(second[cur] == last[cur]) {
      for(int k=cur+1;k<9;k++) {
        if(first[cur] == first[k] && second[cur] == second[k] &&
           last[k] == second[k]) {
          for(int j=0;j<9;j++) {
            if(j != cur && j != k) {
              setSquareImpossible(rows[j],cols[j],first[cur]);
              setSquareImpossible(rows[j],cols[j],second[cur]);
            }
          }
        }
      }
    }
  }
}

void sudoku3::doRowColTile(sudoku3Helper helper) {
  int rows[9];
  int cols[9];
  // check each column
  for(int k=0;k<9;k++) {
    for(int i=0;i<9;i++) {
      rows[i] = i;
      cols[i] = k;
    }
    CALL_SUDOKU3HELPER(*this,helper)(rows,cols);
  }
  // check each row
  for(int k=0;k<9;k++) {
    for(int i=0;i<9;i++) {
      rows[i] = k;
      cols[i] = i;
    }
    CALL_SUDOKU3HELPER(*this,helper)(rows,cols);
  }
  // check each 3x3 tile
  for(int k=0;k<9;k++) {
    int tileR=(k%3)*3;
    int tileC=(k/3)*3;
    for(int i=0;i<9;i++) {
      rows[i] = tileR+i%3;
      cols[i] = tileC+i/3;
    }
    CALL_SUDOKU3HELPER(*this,helper)(rows,cols);
  }
}

void sudoku3::print() {
  for(int row=0;row<9;row++) {
    for(int col=0;col<9;col++) {
      int num = square[col*9+row].get();
      if(num == -1) {
        printf(" -");
      } else {
        printf(" %1d",num+1);
      }
    }
    printf("\n");
  }
}

void sudoku3::printDetail() {
  printf("    0    1    2    3    4    5    6    7    8  \n");
  printf(" +---------------------------------------------\n");
  for(int row=0;row<9;row++) {
    for(int skip=0;skip<8;skip+=4) {
      printf("%d|",row);
      for(int col=0;col<9;col++) {
        printf(" ");
        int tmp=0;
        int printed=0;
        for(int i=0;i<9;i++) {
          if(square[col*9+row].data[i]) {
            if(tmp>=skip && tmp<skip+4) {
              printed++;
              printf("%d",i);
            }
            tmp++;
          }
        }
        for(int i=0;i<4-printed;i++) {
          printf(" ");
        }
      }
      printf("\n");
    }
  }
}

void sudoku3::printDetailAll() {
  for(int row=0;row<9;row++) {
    for(int skip=0;skip<9;skip+=3) {
      for(int col=0;col<9;col++) {
        printf(" ");
        int tmp=0;
        int printed=0;
        for(int i=0;i<9;i++) {
          if(square[col*9+row].data[i]) {
            if(tmp>=skip && tmp<skip+3) {
              printed++;
              printf("%d",i);
            }
            tmp++;
          }
        }
        for(int i=0;i<3-printed;i++) {
          printf(" ");
        }
      }
      printf("\n");
    }
  }
}

void sudoku3::initialize(int *vals) {
  // vals is not in the same major order (not that it matters much)
  for(int col=0;col<9;col++) {
    for(int row=0;row<9;row++) {
      setSquare(row,col,vals[row*9+col]-1);
    }
  }
}

int main() {
  sudoku3 board;
  int easyBoard2[81] =
  {
    -1,  7, -1,  4, -1, -1,  0, -1,  8,
    5,  8,  6,  7,  3, -1, -1,  2, -1,
    -1, -1, -1,  2, -1,  5, -1, -1, -1,
    -1, -1,  8, -1, -1, -1, -1, -1,  0,
    -1,  4, -1,  5, -1,  2, -1,  7, -1,
    0, -1, -1, -1, -1, -1,  5, -1, -1,
    -1, -1, -1,  1, -1,  7, -1, -1, -1,
    -1,  2, -1, -1,  6,  3,  1,  8,  4,
    6, -1,  1, -1, -1,  4, -1,  3, -1,
  };

  int easyBoard[81] =
  {
    -1,-1,-1,-1,-1,5,-1,-1,1,
    -1,8,-1,-1,-1,-1,-1,6,3,
    -1,-1,6,4,3,-1,-1,9,8,
    7,-1,-1,5,-1,9,3,-1,-1,
    9,-1,-1,-1,1,-1,-1,-1,7,
    -1,-1,3,8,-1,2,-1,-1,4,
    8,5,-1,-1,2,4,6,-1,-1,
    6,1,-1,-1,-1,-1,-1,8,-1,
    2,-1,-1,9,-1,-1,-1,-1,-1,
  };

  int mediumBoard[81] =
  {
    -1,3,-1,-1,7,-1,-1,2,5,
    4,-1,-1,2,-1,-1,-1,-1,8,
    1,-1,-1,-1,8,3,-1,-1,-1,
    -1,4,-1,-1,-1,6,-1,8,7,
    3,-1,-1,-1,-1,-1,-1,-1,6,
    6,7,-1,9,-1,-1,-1,1,-1,
    -1,-1,-1,4,9,-1,-1,-1,2,
    7,-1,-1,-1,-1,1,-1,-1,4,
    8,6,-1,-1,2,-1,-1,9,-1,
  };

  int hardBoard[81] =
  {
    1,2,4,-1,5,-1,-1,-1,9,
    -1,-1,8,-1,-1,4,2,-1,1,
    -1,9,-1,8,-1,-1,-1,-1,-1,
    6,-1,9,-1,2,-1,-1,-1,-1,
    -1,-1,2,-1,-1,-1,7,-1,-1,
    -1,-1,-1,-1,8,-1,9,-1,3,
    -1,-1,-1,-1,-1,6,-1,9,-1,
    2,-1,7,4,-1,-1,1,-1,-1,
    9,-1,-1,-1,3,-1,4,7,6,
  };

  int veryHardBoard[81] =
  {
    -1,8,-1,-1,6,-1,-1,-1,-1,
    -1,-1,6,8,-1,4,2,-1,-1,
    -1,7,-1,1,-1,-1,8,6,-1,
    1,-1,-1,-1,7,5,-1,-1,-1,
    -1,6,-1,9,-1,8,-1,2,-1,
    -1,-1,-1,6,4,-1,-1,-1,5,
    -1,2,3,-1,-1,9,-1,5,-1,
    -1,-1,1,3,-1,6,9,-1,-1,
    -1,-1,-1,-1,2,-1,-1,1,-1,
  };

  int testBoard[81] =
  {
    -1, -1, -1,  7, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1,  2,  0, -1, -1, -1,
    -1, -1, -1, -1, -1,  1, -1, -1, -1,
-1, -1, -1, -1,  3,  4, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
};
  int testBoard2[81] =
{
 0,  1,  2,  6, -1, -1,  3,  4,  5,
 3,  4,  5, -1, -1, -1,  6,  7,  8,
 6,  7,  8, -1, -1, -1,  0,  1,  2,
-1, -1, -1,  7, -1, -1, -1, -1, -1,
-1, -1, -1,  8, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1,  8, -1, -1, -1, -1, -1, -1,  7,
 7, -1, -1, -1, -1, -1, -1,  8, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
};
  int testBoard3[81] =
{
-1, -1, -1,  2, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1,  3,  2,
 3, -1,  2, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1,  3,  2, -1,
-1,  2,  3, -1, -1, -1, -1, -1, -1,
-1, -1, -1,  3, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1,  2, -1,  3,
 2,  3, -1, -1, -1, -1, -1, -1, -1,
};
  int testBoard4[81] =
{
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1,
};
//  board.initialize(easyBoard);
//  board.initialize(mediumBoard);
//  board.initialize(hardBoard);
  board.initialize(veryHardBoard);
//  board.initialize(hardBoardGuess2);
//  board.initialize(testBoard3);
  int row,col,val;
  while(1) {
    while(board.changed) {
      while(board.changed) {
        while(board.changed) {
          printf("\n\n");
          board.print();
          board.changed = false;
          board.removeImpossible();
        }
        printf("\n***Iter of 1\n");
        board.print();
        board.changed = false;
        board.findOneSquarePossible();
      }
      printf("\n***Iter of 2\n");
      board.print();
      board.changed = false;
      board.only2SquaresCanBeNumber();
      board.only2SquaresHavePossibility();
    }
    board.printDetail();
    scanf("%d %d %d",&row,&col,&val);
    board.setSquare(row,col,val);
  }

}



