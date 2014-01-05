/*
 *RECURSION CONTEST ENTRY: "Intelligent" Connect Four
 *
 * By: Dylan Moore
 *
 * Dear Professor Roberts,
 *
 * I enjoy a good game of connect four every now and again. I thought you might too.
 * Cheers!
 * 
 * Dylan
 *
 *MOTIVATION FOR PROJECT: We briefly touched on the topic of minimax trees in class and they caught my imagination.
 * This project was my way of pursuing the opportunities they allow and learning as much as I could about them (negamax, alphabeta pruning) 
 * in the time frame I had.
 *
 *ENTRTY DESCRIPTION: This program emulates a connect for game where the human plays the computer.
 * the computer allows the user to set the difficulty level (which cooresponds to the depth of the minimax tree used)
 * and asks the user if they would like to see the minimax tree in action.
 *
 *IMPLIMENTATION: The core of the program is a function called "negamax". This function works the same as a minimax tree in principle, it calculates
 * the value of every board state (with some pruning where it is obvious that one branch is not ideal). The function calls itself and makes that value negative
 * to simulate the opponent's move. This value is added to the calcuated value of the given board state for the given player (which is necessary to check the 
 *intermediate board states and not just the nodes.)
 *
 *EVALUATING A GIVEN BOARD: The board is represented as a Grid of int's where a '0' is an empty space, a '1' is player 1 and a '2' is player 2 (a '3' 
 * represents a filler piece used in the victory() function.) To calcuate the value of a given board the program iterates over the grid and runs a recursive
 * funtion that looks for chains of a given type of piece. This technique was used because it made it simple to check diagonals, horizontals and verticals.
 * I discovered that the event horizon of the program would have to be limited by a depth of 7 (counting from 0) because beyond that point it just took too
 * long to work. Through testing I realized that the computer played well towards the end game but it had a weakness where, when all options were equal it would just choose the first open space. Also, because of it's event horizon it could not "see" wins that happened higher up on the board which would have
 * been obvious to a human player. I did my best to make the code more efficient but depth = 7 was still a boundary.
 * To work around this problem I ended up implimenting some "strategy" guidlines for the computer to differentiate between a variety of non-winning/losing boards. I'll lay out these strategies below:
 *
 *Connect four strategies:
 * Value (through negamax and board evaluation functions):
 * 1: win
 * 2: not lose
 * 3: not filled waiting spot 
 * 4: create waiting spot (till win) 
 * 5: horizontal 3 of a kind with 2 waiting spots
 * 6: diagonal 3 of a kind with waiting spot below
 * 7: horizontal 3 of a kind with waiting spot
 * 8: vertical 3 of a kind
 *
 * I watched expert connect four players and found two more key strategy elements, which in retrospect should have been obvious solutions.
 * Firstly, the middle column is the most valuable because it presents the most opportunites for a win. Secondly, given the fact that the 
 * computer's weakness is in predicting wins which happen higher up on the board it would be benificial for the computer to give some consideration
 * to piece altitude by valueing altitudes inequally. 
 * 
 *You will also notice that a scalar multiple of depth affects the evaluation of a board state. This is essential because a win 2 moves in advanced is more
 * valuable that a win 5 moves in advanced. Without these additions the computer will not recognize that even though a given move might cause a win five moves
 * in the future it will also cause a certain lose 3 moves in advanced and (because these cannot be considered equally) it is thus a bad move.
 *
 *
 *UNFINISHED ELEMENTS: I explored a lot with this project. I ended up writing parts of it java, partly to see how they compared, and partly because I still 
 * feel a bit more comfortable in that language. In an effort to push the event horizon of the program as far as I could I also read about and tried to 
 * impliment alpha-beta pruning. I think that I am really close now, I've included the code at the bottom, but it took me a while to figure it out on my own
 * (implimenting minimax was hard too actually) so unfortuantely it won't be a part of this version of the program. I plan to finish it though and see how well
 * I can make this silicon beast play.
 * 
 * Hope you enjoy playing, it was a real challenge and a lot of fun to make.
 *
 * D
 *
 */
 
 
 
 
 
 
#include <iostream>
#include "gwindow.h"
#include "console.h"
#include "simpio.h"
#include "math.h"
#include "gobjects.h"
#include "grid.h"
#include "gevents.h"
 
using namespace std;
 
void play(GWindow & gw);
double negamax (bool firstCheck, Vector<int> node, double depth, int & player, Grid<int> & board, Vector<int> & bestMove, GWindow & gw, double & loadingX, bool loadingBarDisplay);
GOval *circle (string c, double spacer);
void update(Grid<int> & board, GWindow & gw, bool loading, double & loadX);
int playerMoves (GWindow & gw);
int checkMove(int x, int y, int dx, int dy, int score, Grid<int> & board, int player);
double checkWin(Vector<int> coord, Grid<int> & board, int player);
Vector<Vector<int> > findMoves(Grid<int> & board);
Vector<int> makeMove(int width, Grid<int> & board);
void victory(Grid<int> & board, GWindow & gw);
bool yesOrNo(string input);
 
const double LOAD_WIDTH = 60;
const double LOAD_HEIGHT = 15;
int DEPTH = 7;
 
 
 
 
 
int main() {
    GWindow gw(600, 500);
    play(gw);
    return 0;
}
 
 
/* This fuction takes care of all gameplay. It orquestrates the human player and computer player's turns.
 * If desirable, the order of these turns can be switched here by a small amount of reordering.
 */
 
 
 
void play(GWindow & gw) {
     
    //                          init variables and board
    int pickedColumn = -1;
    bool firstCheck;
    int player = 1;
    int win = 0;
    Vector<int> bestMove;
    bestMove += 5, 3;
    Grid<int> board;
    board.resize(6, 7);
    foreach(int val in board) {
        val = 0;
    }
     
    //board[height 6][width 7]
    /*{0, 0, 0, 0, 0, 0, 0}, //              <----   A representation of the game board
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0}}*/
    Grid<int> emptyBoard = board; //useful for resetting board config
    double zero = 0.0;
    update(board, gw, false, zero);
    bool loadingBarDisplay = false;
    cout << "Welcome, this is Connect Four, you win by getting four adjacent pieces in a diagonal, row, or column." << endl << endl;
    cout << "Warning! selecting yes for the following WILL slow me down" << endl << endl;
    if(yesOrNo("Q: Would you like to see my soul? ")) {
        loadingBarDisplay = true;
    }
    cout << endl;
    if(yesOrNo("Q: Want me to take it easy on you this time? ")) {
        DEPTH = 3;
        cout << "I won't tell...  ;)" << endl;
    } else {
        cout << "Brave and foolish..." << endl << endl << endl;
    }
     
    cout<< "click on any column to play, the checker piece will fall to the bottom"<<endl;
    while(win == 0) {
        zero = 0.0;
        update(board, gw, false, zero);
        //                                  player 1 (HUMAN) moves
        player = 1;
        Vector<int> node;
        node += -1, -1; //node[row, column]
        while(node[0] == -1) {
            //waitForClick();
            //pickedColumn = stringToInteger(getLine("enter column Value: ")); //use this for text based game
            pickedColumn = playerMoves(gw);
            node = makeMove(pickedColumn, board);
        }
        board[node[0]][node[1]] = player;
        update(board, gw, false, zero);
        if(checkWin(node, board, player) >= 80) {
            win = 1;
            cout << endl << "win 1: node" << node[0] << node[1];
            victory(board, gw);
            cout << "you won!";
            break;
        }
        //                                  player 2 (COMPUTER) moves
        firstCheck = true;
        //double score = alphabeta(node, Double.NEGATIVE_INFINITY, //       alphabeta pruning-- unfinished
        //Double.POSITIVE_INFINITY, 0);
        double score = negamax(firstCheck, node, 0, player, board, bestMove, gw, zero, loadingBarDisplay);
        board[bestMove[0]][bestMove[1]] = player; //enters player 2's move
        if(score == INFINITY) {
            win = 2;
            cout<< "you just lost to a robot bro";
            //cout << "win 2: node" << node[0] << node[1]; //testing purposes
            victory(board, gw);
            break;
        }
    }
     
}
 
 
/*  Takes in the column (width) value and finds the bottom most space on the board to
 *  put a token.
 */
Vector<int> makeMove(int width, Grid<int> & board) {
    Vector<int> node;
    node += 0, 0; //fills with 2 empty values
    node[1] = width;
    for(int i = 0; i <= board.numRows(); i++) {
        if(board[i][node[1]] != 0) {
            node[0] = i-1;
            break;
        } else if(board[5][node[1]] == 0) {
            node[0] = 5;
            break;
        }
    }
    return node;
}
 
//print out the board to the console, useful for debugging
void printBoard(Grid<int> & board) {
    for (int i = 0; i < board.numRows(); i++) {
        cout << endl;
        for(int j = 0; j < board.numCols(); j++) {
            cout << board[i][j] << " ";
        }
    }
    cout << endl;
    //update(board);
}
 
double negamax (bool firstCheck, Vector<int> node, double depth, int & player, Grid<int> & board, Vector<int> & bestMove, GWindow & gw, double & loadingX, bool loadingBarDisplay) {
    double thisPlayerScore = -INFINITY;
    double max = thisPlayerScore;
    double score = thisPlayerScore;
    Vector<Vector<int> > allMoves = findMoves(board);
    if(player == 2) { //switches whose turn it is in this check
        player = 1;
    } else if (player == 1) {
        player = 2;
    }
    //checks for terminal node
    for (int i = 0; i < allMoves.size(); i++) {
        if(allMoves[i][0] != -1) {
            score = checkWin(allMoves[i], board, player);
            if (score > thisPlayerScore){
                thisPlayerScore = score;
            }
        }
    }
    if((depth >= DEPTH || thisPlayerScore >= 80) && firstCheck == false) {
        return thisPlayerScore;
    }
    firstCheck = false;
    for(int i = 0; i < allMoves.size(); i++) {
        if(allMoves[i][0] != -1) {
            board[allMoves[i][0]][allMoves[i][1]] = player;
            thisPlayerScore =  checkWin(allMoves[i], board, player);
            score = -(negamax(firstCheck, allMoves[i], depth+1, player, board, bestMove, gw, loadingX, loadingBarDisplay) - (depth+1)*1.1) +
            thisPlayerScore; //other player goes
            board[allMoves[i][0]][allMoves[i][1]] = 0;
             
            if(depth == 0 && thisPlayerScore >= 80) {
                score = INFINITY;
            }
            if(player == 2) {
                player = 1;
            } else {
                player = 2;
            }
             
            if (max < score) {
                max = score;
                if(depth == 0) {
                    bestMove = allMoves[i];
                }
            }
            if(loadingBarDisplay) { //watch the magic unfold...
                update(board, gw, true, loadingX);
                pause(15);
            }
        }
    }
    return max;
}
 
 
//you know... it had to be added
void victory(Grid<int> & board, GWindow & gw) {
    bool hasMore = true;
    double loadingValue = 0.0;
    while(hasMore) {
        hasMore = false;
        Vector<Vector<int> > allMoves = findMoves(board);
        for(int i = 0; i < allMoves.size(); i++) {
            if(allMoves[i][0] != -1) {
                board[allMoves[i][0]][allMoves[i][1]] = 3;
                update(board, gw, true, loadingValue);
                hasMore = true;
                pause(60);
            }
        }
    }
    waitForClick();
}
 
 
 
 
/* Calculates all possible moves and returns these values in a vector<vector<int> >
 */
 
Vector<Vector<int> > findMoves(Grid<int> & board) {
     
    //initializes a blank set of all the possible moves
    Vector<Vector<int> > allMoves;
    Vector<int> blank;
    blank += 0, 0;
    for (int i = 0; i < 7; i++) {
        allMoves += blank;
    }
     
     
     
    for(int i = 0; i < 7; i++) { // width
        Vector<int> coords;
        coords += 0, 0; // init coords with 2 blank spaces
        coords[1] = i; //coords[height][width]
        for(int j = 0; j < 6; j++) { // put the appropriate height value
            if(board[j][i] != 0) {
                coords[0] = j-1;
                allMoves[i] = coords;
                break;
            } else if(board[5][i] == 0) {
                coords[0] = 5;
                allMoves[i] = coords;
                break;
            }
        }
    }
    return allMoves;
}
 
double checkWin(Vector<int> coord, Grid<int> & board, int player) {
     
    //check for a win! (moving in all possible directions recursively)
    if (checkMove(coord[1], coord[0], 1, 1, 0, board, player) +
        checkMove(coord[1], coord[0], -1, -1, 0, board, player) + 1 >= 4 ||
        checkMove(coord[1], coord[0], 0, 1, 0, board, player) +
        checkMove(coord[1], coord[0], 0, -1, 0, board, player) + 1 >= 4 ||
        checkMove(coord[1], coord[0], 1, 0, 0, board, player) +
        checkMove(coord[1], coord[0], -1, 0, 0, board, player) + 1 >= 4 ||
        checkMove(coord[1], coord[0], -1, 1, 0, board, player) +
        checkMove(coord[1], coord[0], 1, -1, 0, board, player) + 1 >= 4) {
            return 80;
        } else {
            double value = 20;
            if(coord[1] == 3) {
                value += 4;
            }
            value += (5-coord[0])*0.25;
            return value;
        }
}
int checkMove(int x, int y, int dx, int dy, int score, Grid<int> & board, int player) {
    if (x+dx >= 0 && y+dy >= 0 && x+dx <= 6 && y+dy <= 5 &&
        board[y+dy][x+dx] == player) {
        score = checkMove(x+dx, y+dy, dx, dy, score+1, board, player);
        //println(x + " " + dx + " " + y + " " + dy);
        return score;
    } else return score;
}
 
//mouse clicks, chooses column
int playerMoves (GWindow & gw) {
    while (true) {
        double spacer = gw.getWidth()/29;
        GMouseEvent e = waitForEvent();
        if (e.getEventType() == MOUSE_PRESSED) {
            double x = 0;
            for (int i = 1; i <= 7; i++) {
                x += spacer;
                if(e.getX() <= x + i*spacer*3) {
                    return i-1;
                }
            }
        }
    }
}
 
//GWindow elements
void update(Grid<int> & board, GWindow & gw, bool loading, double & loadX) {
    //printBoard(board);                                    //NOTE: *****UNCHECK THIS TO SEE THE TEXT BASED VERSION *******
    gw.clear();
    GRect *background = new GRect(gw.getWidth(), gw.getHeight());
    background -> setFillColor("#0000CC");
    background -> setFilled(true);
    gw.add(background, 0, 0);
     
    if(loading) {
        GRect *loading = new GRect(LOAD_WIDTH, LOAD_HEIGHT);
        loading -> setFillColor("#FFCC66");
        loading -> setFilled(true);
        loading -> setColor("#FFCC66");
        gw.add(loading, loadX, 0);
        loadX += 20;
        if(loadX > gw.getWidth()) {
            loadX = -LOAD_WIDTH;
        }
    }
    double spacer = gw.getWidth()/29;
    double x = 0;
    for(int i = 0; i < 7; i++) {
        double y = 0;
        x += spacer;
        for(int j = 0; j < 6; j++) {
            y += spacer;
            string c = "lightGray";
            if(board[j][i] == 1) {
                if(loading) {
                    c = "#990000";
                } else {
                    c = "#C80000";
                }
            } else if (board[j][i] == 2) {
                if(loading) {
                    c = "#000033";
                } else {
                    c = "#660066";
                }
            } else if (board[j][i] == 3) {
                if(loading) {
                    c = "#996633";
                /*} else {
                    c = "#660066";*/
                }
            }
            gw.add(circle(c, spacer), x + i*spacer*3, y + j*spacer*3);
        }
    }
}
//draws a circle
 
GOval *circle (string c, double spacer) {
    GOval *circle = new GOval(3*spacer, 3*spacer);
    circle -> setColor("black");
    circle -> setFillColor(c);
    circle -> setFilled(true);
    return circle;
}
 
 
//useful for user interface, a simple function that gets a yes/no response
//and prevents the user from entering anything else
bool yesOrNo(string message) {
    while (true) {
        string temp = getLine(message);
        if (toLowerCase(temp) == "yes") return true;
        if (toLowerCase(temp) == "no") return false;
        cout << "please enter yes or no" << endl;
    }
     
}
 
/*
 * Note: I had trouble with this so I tried writing it in Java but I couldn't get it working there quite yet either
  
  
//useful function for future (C++)
 int max(int a, int b) {
 if (a >= b) {
 return a;
 } else {
 return b;
 }
 }
  
 (Java)
 private double alphabeta (int[] node, double alpha, double beta, double depth) {
 double thisPlayerScore = Double.NEGATIVE_INFINITY;
 double score = thisPlayerScore;
 int [][] allMoves = findMoves();
 if(player == 2) {
 player = 1;
 } else if (player == 1) {
 player = 2;
 }
 //checks for terminal node
 for (int i = 0; i < allMoves.length; i++) {
 if(allMoves[i][0] != -1) {
 score = checkWin(allMoves[i]);
 if (score > thisPlayerScore){
 thisPlayerScore = score;
 }
 }
 }
 if(depth >= theDepth || thisPlayerScore >= 80 && firstCheck == false) {
 return thisPlayerScore;
 }
 firstCheck = false;
 for(int i = 0; i < allMoves.length; i++) {
 if(allMoves[i][0] != -1) {
 board[allMoves[i][0]][allMoves[i][1]] = player;
 thisPlayerScore =  checkWin(allMoves[i]);
 score = -(alphabeta(allMoves[i], alpha, beta, depth+1) -
 (depth+1)*1.1) + thisPlayerScore; //other player goes
 board[allMoves[i][0]][allMoves[i][1]] = 0;
  
 if(depth == 0 && thisPlayerScore >= 80) {
 score = Double.POSITIVE_INFINITY;
 }
  
 if(player == 2) {
 if (score < beta) {
 beta = score;
 }
 player = 1;
 if(beta <= alpha) {
 break;
 }
 } else if(player == 1) {
 if(score > alpha) {
 alpha = score;
 }
 player = 2;
 if(beta <= alpha) {
 if(depth == 0) {
 bestMove = allMoves[i];
 }
 break;
 }
 }
 //update(board);
 //pause(20);
 }
 }
 if(player == 1) {
 return beta;
 } else {
 return alpha;
 }
  
 } */
