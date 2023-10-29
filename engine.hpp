#ifndef ENGINE_HPP
#define ENGINE_HPP


#include <iostream>
#include <chrono>
#include <array>
#include <string>
#include <functional>
#include <unordered_map>
#include <tuple>
#include <vector>


class CastleRights
{
    public: 
        bool wks;
        bool bks;
        bool wqs;
        bool bqs;

        // contructor
        CastleRights(bool wks=true, bool bks=true, bool wqs=true, bool bqs=true)
        {
            this->wks = wks; 
            this->bks = bks;
            this->wqs = wqs;
            this->bqs = bqs;
        }
};

class MovePiece
{
    private:
        // variables
        std::unordered_map<std::string, int> ranksToRows = { {"1",7}, {"2",6}, {"3",5}, {"4",4}, {"5",3}, {"6",2}, {"7",1}, {"8",0} };
        std::unordered_map<int, std::string> rowsToRanks = { {7,"1"}, {6,"2"}, {5,"3"}, {4,"4"}, {3,"5"}, {2,"6"}, {1,"7"}, {0,"8"} };
        std::unordered_map<std::string, int> filesToCols = { {"a",0}, {"b",1}, {"c",2}, {"d",3}, {"e",4}, {"f",5}, {"g",6}, {"h",7} };
        std::unordered_map<int, std::string> colsToFiles = { {0,"a"}, {1,"b"}, {2,"c"}, {3,"d"}, {4,"e"}, {5,"f"}, {6,"g"}, {7,"h"} };

        


    public:
        // variables 
        int startRow;
        int startCol;
        int endRow;
        int endCol;
        std::string piece_moved;
        std::string piece_captured; 
        bool isPawnPromo;
        bool isCapture;
        bool isCastleMove;
        bool isEnpassantMove;

        int moveID;
        int uniqueID;
        // default contructor
        MovePiece()
        {
            
        }
        // contructor 
        MovePiece(std::vector<int> startSQ, std::vector<int> endSQ, std::array<std::array<std::string, 8>, 8>& board, bool isEnpassant=false, bool isCastle=false, int uniqueID1=100)
        {
            startRow = startSQ[0];
            startCol = startSQ[1];
            endRow = endSQ[0];
            endCol = endSQ[1];
            piece_moved = board[startRow][startCol];
            piece_captured = board[endRow][endCol];

            isPawnPromo = false; 
            isCapture = false;
            
            uniqueID = uniqueID1;

            // pawn promo
            if (piece_moved == "wp" && endRow == 0 || piece_moved == "bp" && endRow == 7)
                isPawnPromo = true;
            
            // enpassant
            isEnpassantMove = isEnpassant;
            if (isEnpassantMove)
                if (piece_moved == "bp")
                    piece_captured = "wp";
                else
                    piece_captured = "bp";

            // castle move
            isCastleMove = isCastle;

            if (piece_captured != "--")
                isCapture = true;

            moveID = startRow*1000 + startCol*100 + endRow*10 + endCol;

        }


        // methods
        std::string getChessNotation()
        {
            return getRankFile(startRow, startCol) + getRankFile(endRow, endCol);
        }

        std::string getRankFile(int r, int c)
        {
            return colsToFiles[c] + rowsToRanks[r];
        }

        bool equals(const MovePiece& other) const
        {
            return moveID == other.moveID;
        }

        std::string toString()
        {
            std::string endSquare = getRankFile(endRow,endCol);
            std::string moveString = piece_moved.substr(1,1);
            if (isCapture)
                moveString += "x";
            return moveString + endSquare;
        }
        

};


class GameState
{
private:
    // variables

    // std::unordered_map< std::string, std::function<void(int, int, std::vector<MovePiece>)> > allPiecesMoveFunction = 
    // {
    //     {"p",getPawnMoves}, 
    //     {"N",getKnightMoves}, 
    //     {"R",getRookMoves},
    //     {"Q",getQueenMoves}, 
    //     {"K",getKingMoves}, 
    //     {"B",getBishopMoves}
    // };

    std::vector<std::vector<int>> pins;
    std::vector<std::vector<int>> checks;
    CastleRights currentCastleRights = CastleRights(true, true, true, true);
    std::vector<CastleRights> castleRightsLog = {CastleRights(currentCastleRights.wks, currentCastleRights.bks, currentCastleRights.wqs, currentCastleRights.bqs)};
    std::vector<int> enpassantPossible;
    std::vector<std::vector<int>> enpassantPossibleLog;

public:
    // variables
    bool whiteToMove;
    std::array<std::array<std::string, 8>, 8> board;
    std::vector<MovePiece> moveLog; 
    
    std::array<int,2> wKingLocation = {7,4};
    std::array<int,2> bKingLocation = {0,4};
    bool inCheck;
    bool checkMate;
    bool staleMate;
    

    // constructor
    GameState()
    {
        // initializeBoard();

        board = {{
            {"bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR"},
            {"bp", "bp", "bp", "bp", "bp", "bp", "bp", "bp"},
            {"--", "--", "--", "--", "--", "--", "--", "--"},
            {"--", "--", "--", "--", "--", "--", "--", "--"},
            {"--", "--", "--", "--", "--", "--", "--", "--"},
            {"--", "--", "--", "--", "--", "--", "--", "--"},
            {"wp", "wp", "wp", "wp", "wp", "wp", "wp", "wp"},
            {"wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"}
        }};

        whiteToMove = true;
        inCheck = false;
        checkMate = false;
        staleMate = false;


    }



    void makeMove(MovePiece move)
    {
        board[move.startRow][move.startCol] = "--";
        board[move.endRow][move.endCol] = move.piece_moved;
        moveLog.push_back(move);
        whiteToMove = !whiteToMove;

        // update king location 
        if (move.piece_moved == "wK")
        {
            wKingLocation[0] = move.endRow;
            wKingLocation[1] = move.endCol;
        }
        else if (move.piece_moved == "bK")
        {
            bKingLocation[0] = move.endRow;
            bKingLocation[1] = move.endCol;
        }
        

        // pawn promo
        if (move.isPawnPromo)
            board[move.endRow][move.endCol] = move.piece_moved.substr(0,1) + "Q";
        
        // enpassant
        if (move.isEnpassantMove)
            board[move.startRow][move.endCol] = "--"; // capture pawn

        // update enpassantPossible
        if (move.piece_moved.substr(1,1) == "p" && abs(move.startRow-move.endRow) == 2)
            enpassantPossible = {int((move.startRow+move.endRow)/2), move.startCol};
        else
            enpassantPossible.clear();
        enpassantPossibleLog.push_back(enpassantPossible);

        // castle move
        if (move.isCastleMove)
        {
            if (move.endCol-move.startCol == 2) // short castle 
            {
                
                board[move.endRow][move.endCol-1] = board[move.endRow][move.endCol+1]; // move rock opposite side of king
                board[move.endRow][move.endCol+1] = "--"; // erase old rook
            }
            else // long castle
            {
                board[move.endRow][move.endCol+1] = board[move.endRow][move.endCol-2];
                board[move.endRow][move.endCol-2] = "--";
            }
        }

        // update castling rights
        updateCastleRight(move);
        castleRightsLog.push_back(CastleRights(currentCastleRights.wks, currentCastleRights.bks, currentCastleRights.wqs, currentCastleRights.bqs));
        

    }

    void undoMove()
    {
        if (moveLog.size() > 0)
        {
            MovePiece move = moveLog.back();
            moveLog.pop_back();
            board[move.startRow][move.startCol] = move.piece_moved;
            board[move.endRow][move.endCol] = move.piece_captured;
            whiteToMove = !whiteToMove;
            
            // update king's location
            if (move.piece_moved == "wK")
            {
                wKingLocation[0] = move.startRow;
                wKingLocation[1] = move.startCol;
            }   
            else if (move.piece_moved == "bK")
            {
                bKingLocation[0] = move.startRow;
                bKingLocation[1] = move.startCol;
            }

            // undo castle rights
            castleRightsLog.pop_back(); // delete new castle right
            CastleRights newRights = castleRightsLog.back(); // set current castle right to now last one in the list
            currentCastleRights = CastleRights(newRights.wks, newRights.bks, newRights.wqs, newRights.bqs);
            // currentCastleRights = newRights;

            // undo castle move
            if (move.isCastleMove)
            {
                if (move.endCol-move.startCol == 2) // short castle
                {
                    board[move.endRow][move.endCol+1] = board[move.endRow][move.endCol-1];
                    board[move.endRow][move.endCol - 1] = "--";
                }
                else // long castle
                {
                    board[move.endRow][move.endCol - 2] = board[move.endRow][move.endCol+1];
                    board[move.endRow][move.endCol + 1] = "--";
                    
                }
            }

            // undo enpassant
            if (move.isEnpassantMove)
            {
                board[move.endRow][move.endCol] = "--";
                board[move.startRow][move.endCol] = move.piece_captured;
            }

            enpassantPossibleLog.pop_back();
            enpassantPossible = enpassantPossibleLog.back();


            // for AI algo
            checkMate = false;
            staleMate = false;
        }
    }

    // GET ALL VALID MOVES
    std::vector<MovePiece> getValidMoves()
    {
        
        std::vector<MovePiece> validMoves;

        std::vector<int> tempEmpassantPossible = enpassantPossible;
        CastleRights tempCastleRight = CastleRights(currentCastleRights.wks, currentCastleRights.bks, currentCastleRights.wqs, currentCastleRights.bqs);
        int kingRow;
        int kingCol;

        std::tuple<bool, std::vector<std::vector<int>>, std::vector<std::vector<int>>> temp;
        temp = findPinsAndChecks();

        
        inCheck = std::get<0>(temp);
        pins = std::get<1>(temp);
        checks = std::get<2>(temp);

        if (whiteToMove)
        {
            kingRow = wKingLocation[0];
            kingCol  = wKingLocation[1]; 
        }
        else
        {
            kingRow = bKingLocation[0];
            kingCol = bKingLocation[1];
        }

        if (inCheck)
        {
            // case 1: only one check. capture, block, or move king
            if (checks.size() == 1)
            {
                validMoves = getAllPossibleMoves();
                std::vector<int> check = checks[0];
                int checkRow = check[0];
                int checkCol = check[1];
                std::string pieceChecking = board[checkRow][checkCol]; // enemy piece checking the king
                std::vector<std::vector<int>> validSquares; // squares pieces can move to block check

                // if knight checking, either capture knight or move king
                if (pieceChecking.substr(1,1) == "N")
                {
                    validSquares = { {checkRow, checkCol} };
                }
                else // not knight checking
                {
                    for (int i=1;i<8;i++)
                    {
                        std::vector<int> validSQ = {kingRow + check[2]*i, kingCol  + check[3]*i}; // check[2] and check[3] are the directions of the check
                        validSquares.push_back(validSQ);
                        if (validSQ[0]==checkRow && validSQ[1]==checkCol) // once get to the checking piece we do not need to check squares beyond that
                            break;
                    }
                }

                // remove moves that don't block check or move king
                // when removing items from a list while iterating through, start from the end of the list
                for (int i=validMoves.size()-1;i>=0;i--)
                {
                    if (validMoves[i].piece_moved.substr(1,1) != "K") // does not move king so must block or capture
                    {
                        bool sameRow = check_num_in_list(validMoves[i].endRow, validSquares, std::string("row"));
                        bool sameCol = check_num_in_list(validMoves[i].endCol, validSquares, std::string("col"));
                        if (!(sameRow && sameCol)) // if not in valid square remove move from validMoves
                        {   
                            validMoves.erase(validMoves.begin()+i); // .begin() returns iterator pointing to the begining, while +i changes iterator position
                            // iterator points to the i-th element in the vector
                        }
                    }
                }

            } // end of single check
            else // double check can only move king
                getKingMoves(kingRow, kingCol, validMoves);
        
        } // end of checking if king is in check

        
        else // king not in check so all moves are valid
        {
            validMoves = getAllPossibleMoves();
        }

        

        if (validMoves.size()==0)
        {
            if(inCheck)
                checkMate = true;
            else
                staleMate = true;
        }
        else // for undo moves
        {
            checkMate = false;
            staleMate = false;
        }

        if (whiteToMove)
            getCastleMoves(wKingLocation[0], wKingLocation[1], validMoves);
        else    
            getCastleMoves(bKingLocation[0], bKingLocation[1], validMoves);

        currentCastleRights = tempCastleRight;
        
        enpassantPossible = tempEmpassantPossible;
        
        return validMoves;

    }

    bool check_num_in_list(int target, std::vector<std::vector<int>> arr, std::string name)
    {
        for (int i=0;i<arr.size();i++)
        {
            // for (int c=0;c<arr[r].size();c++)
            // {
            //     if (target == arr[r][c])
            //         return true;
            // }
            if (name=="row")
            {
                if (target == arr[i][0])
                    return true;
            }
            else if (name=="col")
            {
                if (target == arr[i][1])
                    return true;
            }
            return false;
        }
    }

    std::tuple<bool, std::vector<std::vector<int>>, std::vector<std::vector<int>>> findPinsAndChecks()
    {   
        // FINDS PINS AND CHECKS BY STARTING FROM KING POSIITON AND BRANCHING OUT ORTHOGONALLY AND DIAGONALLY (ALSO CHECKS FOR KNIGHTS)
        
        std::vector<std::vector<int>> pins;
        std::vector<std::vector<int>> checks;
        bool inCheck = false;
        std::string enemy_colour;
        std::string ally_colour;

        
        int startRow;
        int startCol;

        if (whiteToMove)
        {
            enemy_colour = "b";
            ally_colour = "w";
            startRow = wKingLocation[0];
            startCol = wKingLocation[1];
        }

        else
        {
            enemy_colour = "w";
            ally_colour = "b";
            startRow = bKingLocation[0];
            startCol = bKingLocation[1];
        }

        std::array<std::array<int,2>,8> directions = {{ {-1, 0}, {0, -1}, {1, 0}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} }};

        for (int j=0;j<directions.size();j++)
        {
            std::array<int,2> dir = directions[j];
            std::vector<int> possiblePins;
            
            for (int i=1;i<8;i++)
            {
                int endRow = startRow + dir[0]*i;
                int endCol = startCol + dir[1]*i;
   
                if (0<=endRow && endRow<8 && 0<=endCol && endCol<8)
                {
                    std::string endPiece = board[endRow][endCol];
                    if (endPiece.substr(0,1) == ally_colour && endPiece.substr(1,1) != "K")
                    {
                        if (possiblePins.empty()) // if empty means closest piece to king, could be a pin
                        {
                            possiblePins.push_back(endRow);
                            possiblePins.push_back(endCol);
                            possiblePins.push_back(dir[0]);
                            possiblePins.push_back(dir[1]); 
                        }
                        else
                            break; // possible pins is not empty thus second ally piece and is not a pin
                    }
                    else if (endPiece.substr(0,1) == enemy_colour) // piece is enemy colour
                    {
                        //  5 cases + 1 more for knight
                        //  1) rook
                        //  2) bishop
                        //  3) diagonally a pawn attacking king
                        //  4) queen
                        //  5) any direction 1 sq away there is king
                        std::string pieceType = endPiece.substr(1,1);
                        

                        if ( 
                            (0<=j && j<=3 && pieceType == "R") || 
                            (4<=j && j<= 7 && pieceType == "B") || 
                            (i==1 && pieceType=="p" && ((enemy_colour == "w" && 6<=j && j<=7) || (enemy_colour == "b" && 4<=j && j<=5))) || 
                            (pieceType == "Q") ||
                            (i==0 && pieceType == "K")
                            )
                        {
                            if (possiblePins.empty()) // no blocks on orthogoanl or diagonal therefore check
                            {
                                inCheck = true;
                                std::vector<int> temp = {endRow, endCol, dir[0], dir[1] };
                                checks.push_back(temp);
                                break;
                            }

                            else // piece blocking therefore pin present
                                pins.push_back(possiblePins);
                        }
                        else
                            break; // no piece detected in orthogonal or diagonal therefore no check
                    }
                }
                else
                    break; // off board
            }
        } // END OF CHECKING DIAGONAL AND ORTHOGONAL FOR PIECES, CHECKS AND PINS

        // exception -> knight move
        std::array<std::array<int,2>,8> knight_dir = {{ {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1} }}; 

        for (int i=0;i<knight_dir.size();i++)
        {
            int endRow = startRow + knight_dir[i][0];
            int endCol = endCol + knight_dir[i][1];
            if (0<= endRow && endRow <8 && 0<=endCol && endCol<8)
            {
                std::string endPiece = board[endRow][endCol];
                if (endPiece.substr(0,1) == enemy_colour && endPiece.substr(1,1) == "N")
                {
                    inCheck = true;
                    std::vector<int> temp = {endRow, endCol, knight_dir[i][0], knight_dir[i][1] };
                    checks.push_back(temp);
                }
            }
        }

        return std::make_tuple(inCheck, pins, checks);
    }

    // ALL MOVE GENERATION
    std::vector<MovePiece> getAllPossibleMoves()
    {
        
        std::vector<MovePiece> moves;

        for (int r=0;r<board.size();r++)
        {
            for (int c=0;c<board[r].size();c++)
            {


                std::string turn_colour = board[r][c].substr(0,1);
                
                if (turn_colour == "w" && whiteToMove || turn_colour == "b" && !whiteToMove)
                {
                    std::string piece = board[r][c].substr(1,1);
                    // allPiecesMoveFunction[piece](r,c,moves);
                    if (piece == "p")
                        getPawnMoves(r,c,moves);
                    else if (piece == "N")
                        getKnightMoves(r,c,moves);
                    else if (piece == "B")
                        getBishopMoves(r,c,moves);
                    else if (piece == "R")
                        getRookMoves(r,c,moves);
                    else if (piece == "Q")
                        getQueenMoves(r,c,moves);
                    else if (piece == "K")
                        getKingMoves(r,c,moves);
                }
            }
        }
        
        return moves;
    }



    // PIECE MOVE GENERATION
    void getPawnMoves(int r, int c, std::vector<MovePiece>& moves)
    {
        bool piecePinned = false;
        std::array<int,2> pinDirection;
        std::string enemy_colour;

        for (int i=pins.size()-1;i>=0;i--)
        {
            if (pins[i][0] == r && pins[i][1] == c)
            {
                piecePinned = true;
                pinDirection = {pins[i][2], pins[i][3]};
                pins.erase(pins.begin()+i);
                break;
            }
        }
        
        if (whiteToMove)
            enemy_colour = "b";
        else
            enemy_colour = "w";
        
        
        if (whiteToMove)
        {
            if (board[r-1][c] == "--") // one sq pawn move
            {
                if (!piecePinned || pinDirection[0] == -1 && pinDirection[1] == 0) // ensures piece is not pinned or the pin direction is in the direction the piece moves
                {
                    moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r-1,c}, board));
                    if (r==6 && board[r-2][c] == "--") // two sq pawn move
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r-2,c}, board));
                }
            }
            
            // CAPTURES
            if (c-1>=0)
            {
                if (board[r-1][c-1].substr(0,1) == enemy_colour)
                {
                    if (!piecePinned || pinDirection[0] == -1 && pinDirection[1] == -1)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r-1,c-1}, board));
                }

                else if (enpassantPossible.size()!=0 && enpassantPossible[0] == r-1 && enpassantPossible[1] == c-1)
                {
                    if (!piecePinned || pinDirection[0] == -1 && pinDirection[1] == -1)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r-1,c-1}, board, true));
                }
            }

            if (c+1<=7)
            {
                if (board[r-1][c+1].substr(0,1) == enemy_colour)
                {
                    if (!piecePinned || pinDirection[0] == -1 && pinDirection[1] == 1)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r-1,c+1}, board));
                }
                else if (enpassantPossible.size()!=0 && enpassantPossible[0] == r-1 && enpassantPossible[1] == c+1)
                {
                    if (!piecePinned || pinDirection[0] == -1 && pinDirection[1] == 1)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r-1,c+1}, board, true));
                }
                
            }
            
        }
        else // black pawn moves
        {
            if (board[r+1][c] == "--") // one sq pawn move
            {
                if (!piecePinned || pinDirection[0] == 1 && pinDirection[1] == 0) // ensures piece is not pinned or the pin direction is in the direction the piece moves
                {
                    moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r+1,c}, board));
                    if (r==1 && board[r+2][c] == "--") // two sq pawn move
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r+2,c}, board));
                }
                
            }

            // CAPTURES
            if (c-1>=0)
            {
                if (board[r+1][c-1].substr(0,1) == enemy_colour)
                {
                    if (!piecePinned || pinDirection[0] == 1 && pinDirection[1] == -1)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r+1,c-1}, board));
                }
                else if (enpassantPossible.size()!=0 && enpassantPossible[0] == r+1 && enpassantPossible[1] == c-1)
                {
                    if (!piecePinned || pinDirection[0] == 1 && pinDirection[1] == 1)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r+1,c-1}, board, true));
                }
            }

            if (c+1<=7)
            {
                if (board[r+1][c+1].substr(0,1) == enemy_colour)
                {
                    if (!piecePinned || pinDirection[0] == 1 && pinDirection[1] == 1)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r+1,c+1}, board));
                }
                else if (enpassantPossible.size()!=0 && enpassantPossible[0] == r+1 && enpassantPossible[1] == c+1)
                {
                    if (!piecePinned || pinDirection[0] == 1 && pinDirection[1] == 1)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r+1,c+1}, board, true));
                }
            }   
        }
    }

    void getKnightMoves(int r, int c, std::vector<MovePiece>& moves)
    {
        bool piecePinned = false;
        std::string enemy_colour;
        

        for (int i=pins.size()-1;i>=0;i--)
        {
            if (pins[i][0] == r && pins[i][1] == c)
            {
                piecePinned = true;
                pins.erase(pins.begin()+i);
                break;
            }
        }
        
        std::array<std::array<int,2>,8> directions = {{ {-2,-1},{-2, 1},{-1, -2},{-1, 2},{1, -2}, {1, 2}, {2, -1}, {2, 1}}};
        if (whiteToMove)
            enemy_colour = "b";
        else
            enemy_colour = "w";
        
        
        
        for (int i=0;i<directions.size();i++)
        {
            int endRow = r + directions[i][0];
            int endCol = c + directions[i][1];
            if (0<=endRow && endRow<8 && 0<=endCol && endCol<8)
            {
                if (!piecePinned)
                {
                    std::string endPiece = board[endRow][endCol].substr(0,1);
                    if (board[endRow][endCol] == "--" || endPiece==enemy_colour)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{endRow,endCol}, board));
                }
            }
        }
    }

    void getBishopMoves(int r, int c, std::vector<MovePiece>& moves)
    {
        bool piecePinned = false;
        std::array<int,2> pinDirection;
        std::string enemy_colour;

        for (int i=pins.size()-1;i>=0;i--)
        {
            if (pins[i][0] == r && pins[i][1] == c)
            {
                piecePinned = true;
                pinDirection = {pins[i][2], pins[i][3]};
                pins.erase(pins.begin()+i);
                break;
            }
        }

        std::array<std::array<int,2>,4> directions = {{ {-1,-1},{-1,1},{1,-1},{1,1} }};
        if (whiteToMove)
            enemy_colour = "b";
        else    
            enemy_colour = "w";
        
        for (int i=0;i<directions.size();i++)
        {
            for (int j=1;j<8;j++)
            {
                int endRow = r + directions[i][0]*j;
                int endCol = c+ directions[i][1]*j;

                if (0<=endRow && endRow<8 && 0<=endCol && endCol<8)
                {
                    if(!piecePinned || pinDirection[0] == directions[i][0] && pinDirection[1] == directions[i][1] || 
                        pinDirection[0] == directions[i][0]*-1 && pinDirection[1] == directions[i][1]*-1)
                    {
                        std::string endPiece = board[endRow][endCol];
                        
                        if (endPiece == "--")
                            moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{endRow,endCol}, board));
                        else if (endPiece.substr(0,1) == enemy_colour)
                        {
                            moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{endRow,endCol}, board));
                            break;
                        }
                        else
                            break; // friendly piece
                    }
                }
                else
                    break; // off board
            }
        }
    }

    void getRookMoves(int r, int c, std::vector<MovePiece>& moves)
    {
        bool piecePinned = false;
        std::array<int,2> pinDirection;
        std::string enemy_colour;

        for (int i=pins.size()-1;i>=0;i--)
        {
            if (pins[i][0] == r && pins[i][1] == c)
            {
                piecePinned = true;
                pinDirection = {pins[i][2], pins[i][3]};
                // rook generate queen moves
                if (board[r][c].substr(1,1) != "Q") // can't remove queen from pin on rook moves only remove it on bishop moves 
                    pins.erase(pins.begin()+i);
                break;
            }
        }

        std::array<std::array<int,2>,4> directions = {{ {0,-1},{0,1},{-1,0},{1,0} }};
        if (whiteToMove)
            enemy_colour = "b";
        else    
            enemy_colour = "w";

        for (int i=0;i<directions.size();i++)
        {
            for (int j=1;j<8;j++)
            {
                int endRow = r + directions[i][0]*j;
                int endCol = c+ directions[i][1]*j;

                if (0<=endRow && endRow<8 && 0<=endCol && endCol<8)
                {
                    if(!piecePinned || pinDirection[0] == directions[i][0] && pinDirection[1] == directions[i][1] || 
                        pinDirection[0] == directions[i][0]*-1 && pinDirection[1] == directions[i][1]*-1)
                    {
                        std::string endPiece = board[endRow][endCol];
                        
                        if (endPiece == "--")
                            moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{endRow,endCol}, board));
                        else if (endPiece.substr(0,1) == enemy_colour)
                        {
                            moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{endRow,endCol}, board));
                            break;
                        }
                        else
                            break; // friendly piece
                    }
                }
                else
                    break; // off board
            }
        }

    }

    void getQueenMoves(int r, int c, std::vector<MovePiece>& moves)
    {
        getBishopMoves(r,c,moves);
        getRookMoves(r,c,moves);
    }

    void getKingMoves(int r, int c, std::vector<MovePiece>& moves)
    {
        std::array<int,8> rowMoves = {-1,-1,-1,0,0,1,1,1};
        std::array<int,8> colMoves = {-1,0,1,-1,1,-1,0,1};
        std::string allyColor;
        if (whiteToMove)
            allyColor = "w";
        else
            allyColor = "b";

        for (int i=0;i<8;i++)
        {
            int endRow = r + rowMoves[i];
            int endCol = c + colMoves[i];
            if (0<=endRow && endRow<8 && 0<=endCol && endCol<8)
            {
                std::string endPiece = board[endRow][endCol];
                if (endPiece.substr(0,1) != allyColor) 
                {
                    // place king on sq and check for checks
                    if (allyColor == "w")
                        wKingLocation = {endRow, endCol};
                    else
                        bKingLocation = {endRow, endCol};
                    
                    bool inCheck = std::get<0>(findPinsAndChecks());
                    if (!inCheck)
                        moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{endRow,endCol}, board));
                    
                    // place king back on original sq
                    if (allyColor == "w")
                        wKingLocation = {r,c};
                    else
                        bKingLocation = {r,c};
                }
            }
        }
    }



    // CASTLING
    void updateCastleRight(MovePiece move)
    {
        if (move.piece_moved == "wK")
        {
            currentCastleRights.wks = false;
            currentCastleRights.wqs = false;
        }
        else if (move.piece_moved == "bK")
        {
            currentCastleRights.bks = false;
            currentCastleRights.bqs = false;
        }
        else if (move.piece_moved == "wR")
        {
            if (move.startRow == 7)
            {
                if (move.startCol==0)
                    currentCastleRights.wqs = false;
                if (move.startCol == 7)
                    currentCastleRights.wks = false;
            }
        }
        else if (move.piece_moved == "bR")
        {
            if (move.startRow == 0)
            {
                if (move.startCol==0)
                    currentCastleRights.bqs = false;
                if (move.startCol == 7)
                    currentCastleRights.bks = false;
            }
        }

        // if rook is captured, don't allow castle
        if (move.piece_captured == "wR")
        {
            if (move.endRow == 7)
            {
                if (move.endCol == 0)
                    currentCastleRights.wqs = false;
                if (move.endCol == 7)
                    currentCastleRights.wks = false;
            }
        }
        else if (move.piece_captured == "bR")
        {
            if (move.endRow == 0)
            {
                if (move.endCol == 0)
                    currentCastleRights.bqs = false;
                if (move.endCol == 7)
                    currentCastleRights.bks = false;
            }
        }
    }

    void getCastleMoves(int r, int c, std::vector<MovePiece>& moves)
    {
        if (sqUnderAttack(r, c))
            return; // can't castle if sqUnderAttack is the king aka in check
        if (whiteToMove && currentCastleRights.wks || (!whiteToMove) && currentCastleRights.bks)
            getKsCastleMoves(r,c,moves);
        if (whiteToMove && currentCastleRights.wqs || (!whiteToMove) && currentCastleRights.bqs)
            getQsCastleMoves(r,c,moves);
    }

    void getKsCastleMoves(int r, int c, std::vector<MovePiece>& moves)
    {
        if (board[r][c+1] == "--" && board[r][c+2] == "--")
        {
            if ((!sqUnderAttack(r,c+1)) && (!sqUnderAttack(r,c+2)))
            {
                
                moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r,c+2}, board, false, true, 123));
            }
        }
    }

    void getQsCastleMoves(int r, int c, std::vector<MovePiece>& moves)
    {
        if (board[r][c-1] == "--" && board[r][c-2] == "--" && board[r][c-3] == "--")
        {
            if ((!sqUnderAttack(r,c-1)) && (!sqUnderAttack(r,c-2)))
                moves.push_back(MovePiece(std::vector<int>{r,c}, std::vector<int>{r,c-2}, board, false, true));
        }
    }

    // UTILITY
    bool sqUnderAttack(int r, int c)
    {
        whiteToMove = !whiteToMove;
        std::vector<MovePiece> enemy_moves = getAllPossibleMoves();
        whiteToMove = !whiteToMove;

        for (int i=0;i<enemy_moves.size();i++)
        {
            MovePiece move = enemy_moves[i];
            if (move.endRow == r && move.endCol == c)
            {   
                return true;
            }
        }
        return false;
    }
};



#endif // ENGINE_HPP