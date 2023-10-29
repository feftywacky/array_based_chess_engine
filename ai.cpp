#include "ai.hpp"
#include "engine.hpp"
#include <iostream>
#include <array>
#include <algorithm>
#include <random>
#include <ctime>  // For seeding the random number generator
#include <chrono>
#include <iomanip> // Include this header for formatting output



std::unordered_map<std::string, int> pieceScore_mg = {
    {"p", 82}, {"N", 337}, {"B", 365}, {"R", 477}, {"Q", 1025}, {"K", 0}
};

std::unordered_map<std::string, int> pieceScore_eg = {
    {"p", 94}, {"N", 281}, {"B", 297}, {"R", 512}, {"Q", 936}, {"K", 0}
};

std::array<std::array<int, 8>, 8> wKnight_mg = {{
    {{-167, -89, -34, -49,  61, -97, -15, -107}},
    {{-73, -41,  72,  36,  23,  62,   7,  -17}},
    {{-47,  60,  37,  65,  84, 129,  73,   44}},
    {{-9,  17,  19,  53,  37,  69,  18,   22}},
    {{-13,   4,  16,  13,  28,  19,  21,   -8}},
    {{-23,  -9,  12,  10,  19,  17,  25,  -16}},
    {{-29, -53, -12,  -3,  -1,  18, -14,  -19}},
    {{-105, -21, -58, -33, -17, -28, -19,  -23}}
}};

std::array<std::array<int, 8>, 8> wKnight_eg = {{
    {{-58, -38, -13, -28, -31, -27, -63, -99}},
    {{-25,  -8, -25,  -2,  -9, -25, -24, -52}},
    {{-24, -20,  10,   9,  -1,  -9, -19, -41}},
    {{-17,   3,  22,  22,  22,  11,   8, -18}},
    {{-18,  -6,  16,  25,  16,  17,   4, -18}},
    {{-23,  -3,  -1,  15,  10,  -3, -20, -22}},
    {{-42, -20, -10,  -5,  -2, -20, -23, -44}},
    {{-29, -51, -23, -15, -22, -18, -50, -64}}
}};


std::array<std::array<int, 8>, 8> wBishop_mg = {{
    {{-29,   4, -82, -37, -25, -42,   7,  -8}},
    {{-26,  16, -18, -13,  30,  59,  18, -47}},
    {{-16,  37,  43,  40,  35,  50,  37,  -2}},
    {{ -4,   5,  19,  50,  37,  37,   7,  -2}},
    {{ -6,  13,  13,  26,  34,  12,  10,   4}},
    {{  0,  15,  15,  15,  14,  27,  18,  10}},
    {{  4,  15,  16,   0,   7,  21,  33,   1}},
    {{-33,  -3, -14, -21, -13, -12, -39, -21}}
}};

std::array<std::array<int, 8>, 8> wBishop_eg = {{
    {{-14, -21, -11,  -8, -7,  -9, -17, -24}},
    {{ -8,  -4,   7, -12, -3, -13,  -4, -14}},
    {{  2,  -8,   0,  -1, -2,   6,   0,   4}},
    {{ -3,   9,  12,   9, 14,  10,   3,   2}},
    {{ -6,   3,  13,  19,  7,  10,  -3,  -9}},
    {{-12,  -3,   8,  10, 13,   3,  -7, -15}},
    {{-14, -18,  -7,  -1,  4,  -9, -15, -27}},
    {{-23,  -9, -23,  -5, -9, -16,  -5, -17}}
}};


std::array<std::array<int, 8>, 8> wRook_mg = {{
    {{32,  42,  32,  51, 63,  9,  31,  43}},
    {{27,  32,  58,  62, 80, 67,  26,  44}},
    {{-5,  19,  26,  36, 17, 45,  61,  16}},
    {{-24, -11,   7,  26, 24, 35,  -8, -20}},
    {{-36, -26, -12,  -1,  9, -7,   6, -23}},
    {{-45, -25, -16, -17,  3,  0,  -5, -33}},
    {{-44, -16, -20,  -9, -1, 11,  -6, -71}},
    {{-19, -13,   1,  17, 16,  7, -37, -26}}
}};

std::array<std::array<int, 8>, 8> wRook_eg = {{
    {{13, 10, 18, 15, 12,  12,   8,   5}},
    {{11, 13, 13, 11, -3,   3,   8,   3}},
    {{ 7,  7,  7,  5,  4,  -3,  -5,  -3}},
    {{ 4,  3, 13,  1,  2,   1,  -1,   2}},
    {{ 3,  5,  8,  4, -5,  -6,  -8, -11}},
    {{-4,  0, -5, -1, -7, -12,  -8, -16}},
    {{-6, -6,  0,  2, -9,  -9, -11,  -3}},
    {{-9,  2,  3, -1, -5, -13,   4, -20}}
}};


std::array<std::array<int, 8>, 8> wQueen_mg = {{
    {{-28,   0,  29,  12,  59,  44,  43,  45}},
    {{-24, -39,  -5,   1, -16,  57,  28,  54}},
    {{-13, -17,   7,   8,  29,  56,  47,  57}},
    {{-27, -27, -16, -16,  -1,  17,  -2,   1}},
    {{ -9, -26,  -9, -10,  -2,  -4,   3,  -3}},
    {{-14,   2, -11,  -2,  -5,   2,  14,   5}},
    {{-35,  -8,  11,   2,   8,  15,  -3,   1}},
    {{ -1, -18,  -9,  10, -15, -25, -31, -50}}
}};

std::array<std::array<int, 8>, 8> wQueen_eg = {{
    {{ -9,  22,  22,  27,  27,  19,  10,  20}},
    {{-17,  20,  32,  41,  58,  25,  30,   0}},
    {{-20,   6,   9,  49,  47,  35,  19,   9}},
    {{  3,  22,  24,  45,  57,  40,  57,  36}},
    {{-18,  28,  19,  47,  31,  34,  39,  23}},
    {{-16, -27,  15,   6,   9,  17,  10,   5}},
    {{-22, -23, -30, -16, -16, -23, -36, -32}},
    {{-33, -28, -22, -43,  -5, -32, -20, -41}}
}};

std::array<std::array<int, 8>, 8> wKing_mg = {{
    {{-65,  23,  16, -15, -56, -34,   2,  13}},
    {{ 29,  -1, -20,  -7,  -8,  -4, -38, -29}},
    {{ -9,  24,   2, -16, -20,   6,  22, -22}},
    {{-17, -20, -12, -27, -30, -25, -14, -36}},
    {{-49,  -1, -27, -39, -46, -44, -33, -51}},
    {{-14, -14, -22, -46, -44, -30, -15, -27}},
    {{  1,   7,  -8, -64, -43, -16,   9,   8}},
    {{-15,  36,  12, -54,   8, -28,  24,  14}}
}};

std::array<std::array<int, 8>, 8> wKing_eg = {{
    {{-74, -35, -18, -18, -11,  15,   4, -17}},
    {{-12,  17,  14,  17,  17,  38,  23,  11}},
    {{ 10,  17,  23,  15,  20,  45,  44,  13}},
    {{ -8,  22,  24,  27,  26,  33,  26,   3}},
    {{-18,  -4,  21,  24,  27,  23,   9, -11}},
    {{-19,  -3,  11,  21,  23,  16,   7,  -9}},
    {{-27, -11,   4,  13,  14,   4,  -5, -17}},
    {{-53, -34, -21, -11, -28, -14, -24, -43}}
}};

std::array<std::array<int, 8>, 8> wPawn_mg = {{
    {{ 0,   0,   0,   0,   0,   0,  0,   0}},
    {{98, 134,  61,  95,  68, 126, 34, -11}},
    {{-6,   7,  26,  31,  65,  56, 25, -20}},
    {{-14,  13,   6,  21,  23,  12, 17, -23}},
    {{-27,  -2,  -5,  12,  17,   6, 10, -25}},
    {{-26,  -4,  -4, -10,   3,   3, 33, -12}},
    {{-35,  -1, -20, -23, -15,  24, 38, -22}},
    {{ 0,   0,   0,   0,   0,   0,  0,   0}}
}};

std::array<std::array<int, 8>, 8> wPawn_eg = {{
    {{ 0,   0,   0,   0,   0,   0,   0,   0}},
    {{178, 173, 158, 134, 147, 132, 165, 187}},
    {{ 94, 100,  85,  67,  56,  53,  82,  84}},
    {{ 32,  24,  13,   5,  -2,   4,  17,  17}},
    {{ 13,   9,  -3,  -7,  -7,  -8,   3,  -1}},
    {{  4,   7,  -6,   1,   0,  -5,  -1,  -8}},
    {{ 13,   8,   8,  10,  13,   0,   2,  -7}},
    {{  0,   0,   0,   0,   0,   0,   0,   0}}
}};

std::array<std::array<int, 8>, 8> bKnight_mg = flip(wKnight_mg, 0);
std::array<std::array<int, 8>, 8> bKnight_eg = flip(wKnight_eg, 0);
std::array<std::array<int, 8>, 8> bBishop_mg = flip(wBishop_mg, 0);
std::array<std::array<int, 8>, 8> bBishop_eg = flip(wBishop_eg, 0);
std::array<std::array<int, 8>, 8> bRook_mg = flip(wRook_mg, 0);
std::array<std::array<int, 8>, 8> bRook_eg = flip(wRook_eg, 0);
std::array<std::array<int, 8>, 8> bQueen_mg = flip(wQueen_mg, 0); 
std::array<std::array<int, 8>, 8> bQueen_eg = flip(wQueen_eg, 0); 
std::array<std::array<int, 8>, 8> bKing_mg = flip(wKing_mg, 0);
std::array<std::array<int, 8>, 8> bKing_eg = flip(bKing_eg, 0);
std::array<std::array<int, 8>, 8> bPawn_mg = flip(wPawn_mg, 0);
std::array<std::array<int, 8>, 8> bPawn_eg = flip(wPawn_eg, 0);

std::unordered_map<std::string, std::array<std::array<int, 8>, 8>> piecePositionScores_mg = {
    {"wN", wKnight_mg},
    {"bN", bKnight_mg},
    {"wB", wBishop_mg},
    {"bB", bBishop_mg},
    {"wQ", wQueen_mg},
    {"bQ", bQueen_mg},
    {"wR", wRook_mg},
    {"bR", bRook_mg},
    {"wp", wPawn_mg},
    {"bp", bPawn_mg},
    {"wK", wKing_mg},
    {"bK", bKing_mg}
};

std::unordered_map<std::string, std::array<std::array<int, 8>, 8>> piecePositionScores_eg = {
    {"wN", wKnight_eg},
    {"bN", bKnight_eg},
    {"wB", wBishop_eg},
    {"bB", bBishop_eg},
    {"wQ", wQueen_eg},
    {"bQ", bQueen_eg},
    {"wR", wRook_eg},
    {"bR", bRook_eg},
    {"wp", wPawn_eg},
    {"bp", bPawn_eg},
    {"wK", wKing_eg},
    {"bK", bKing_eg}
};

const int CHECKMATE = 100000;
const int STALEMATE = 0;
int DEPTH = 4;
int counter = 0;
MovePiece nextMove;

// FUNCTIONS

template <typename T, size_t Rows, size_t Cols>
std::array<std::array<T, Cols>, Rows> flip(const std::array<std::array<T, Cols>, Rows>& matrix, int axis) {
    std::array<std::array<T, Cols>, Rows> flippedMatrix = matrix;

    if (axis == 0) {
        std::reverse(flippedMatrix.begin(), flippedMatrix.end());
    } else if (axis == 1) {
        for (auto& row : flippedMatrix) {
            for (size_t j = 0, k = Cols - 1; j < k; ++j, --k) {
                std::swap(row[j], row[k]);
            }
        }
    }

    return flippedMatrix;
}

MovePiece findRandomMove(std::vector<MovePiece> validMoves)
{
    // Seed the random number generator
    std::mt19937 rng(std::time(nullptr));

    // Generate a random index within the range of the vector's size
    std::uniform_int_distribution<std::size_t> distribution(0, validMoves.size() - 1);
    std::size_t randomIndex = distribution(rng);

    return validMoves[randomIndex];
}


int findMoveNegaMaxAlphaBeta(GameState gameState, int depth, int alpha, int beta)
{
    counter++;
    if (depth == 0) {
        return evaluation(gameState);
        // return quiescence(gameState, 8, alpha, beta);
    }

    
    std::vector<MovePiece> validMoves = gameState.getValidMoves();
    // std::cout<<"here"<<std::endl;
    validMoves = move_ordering(gameState, validMoves);
    
    // validMoves = orderMoves(gameState, validMoves);

    for (int i=0;i<validMoves.size();i++) 
    {
        MovePiece move = validMoves[i];
        gameState.makeMove(move);
        float score = -findMoveNegaMaxAlphaBeta(gameState, depth - 1, -beta, -alpha);
        gameState.undoMove();
        if (score >= beta) 
        {
            return beta;
        }
        if (score > alpha) 
        {
            alpha = score;
            if (depth == DEPTH) 
            {
                nextMove = move;
                std::cout << move.toString() << " " << score / 100 << std::endl;
            }
        }
    }

    // if (gameState.staleMate) {
    //     alpha = 0;
    // }

    return alpha;
}

std::vector<MovePiece> move_ordering(GameState gameState, std::vector<MovePiece> validMoves)
{
    std::vector<int> scoreList;
    int captureMultiplier = 100;
    int checkBonus = 100;
    bool isEndGame = checkEndGame(gameState);

    for (MovePiece move : validMoves)
    {
        std::string piece = move.piece_moved;
        std::string piece_captured = move.piece_captured;
        int endRow = move.endRow;
        int endCol = move.endCol;
        int score = 0;

        if (!isEndGame)
        {
            if (piece_captured!="--")
                score += captureMultiplier * (pieceScore_mg[piece_captured.substr(1,1)] - pieceScore_mg[piece.substr(1,1)]);
            if (piece.substr(1,1) == "p" && (endRow == 7 || endRow == 0))
                score += pieceScore_mg["Q"]*100;
            if (piece_captured.substr(1,1)=="K")
                score += pieceScore_mg["R"]*100;
        }
        else
        {
            if (piece_captured!="--")
                score += captureMultiplier * (pieceScore_eg[piece_captured.substr(1,1)] - pieceScore_eg[piece.substr(1,1)]);
            if (piece.substr(1,1) == "p" && (endRow == 7 || endRow == 0))
                score += pieceScore_eg["Q"]*100;
            if (piece_captured.substr(1,1)=="K")
                score += pieceScore_eg["R"]*100;
        }
        scoreList.push_back(score);
    }

    // sorting
    std::vector<std::pair<MovePiece, int>> scorePair;
    for (int i=0;i<validMoves.size();i++)
        scorePair.push_back(std::make_pair(validMoves[i], scoreList[i]));

    std::sort(scorePair.begin(), scorePair.end(),
          [](const std::pair<MovePiece, int>& a, const std::pair<MovePiece, int>& b) {
              return a.second > b.second;
          });

    for (int i=0;i<scorePair.size();i++)
    {
        validMoves[i] = scorePair[i].first;
        scoreList[i] = scorePair[i].second;
    }
    
    return validMoves;

}

MovePiece findBestMove(GameState gameState)
{
    auto start = std::chrono::high_resolution_clock::now();
    counter = 0;

    findMoveNegaMaxAlphaBeta(gameState, DEPTH, -CHECKMATE, CHECKMATE);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double seconds = duration.count() / 1e6;
    std::cout << "Function execution time: " << std::setprecision(9) << seconds << " seconds" << std::endl;
    
    std::cout<<counter<<" "<<"non-quiescence"<<std::endl;
    
    return nextMove;
}



int evaluation(GameState gameState)
{
    if (gameState.checkMate)
    {
        if (gameState.whiteToMove)
            return -CHECKMATE;
        else    
            return CHECKMATE;
    }
    else if (gameState.staleMate)
        return STALEMATE;

    int score = 0;
    bool isEndGame = checkEndGame(gameState);

    for (int r=0;r<8;r++)
    {
        for (int c=0;c<8;c++)
        {
            std::string square = gameState.board[r][c]; 

            if (!isEndGame)
            {
                if (square != "--")
                {
                    int piecePositionScore = piecePositionScores_mg[square][r][c];

                    if (square.substr(0,1) == "w")
                        score += pieceScore_mg[square.substr(1,1)] + piecePositionScore;
                    else if (square.substr(0,1) == "b")
                        score -= pieceScore_mg[square.substr(1,1)] + piecePositionScore;
                }
            }

            else // endgame eval
            {
                if (square != "--")
                {
                    int piecePositionScore = piecePositionScores_eg[square][r][c];

                    if (square.substr(0,1) == "w")
                        score += pieceScore_eg[square.substr(1,1)] + piecePositionScore;
                    else if (square.substr(0,1) == "b")
                        score -= pieceScore_eg[square.substr(1,1)] + piecePositionScore;
                }
            }
        }
    }
    int turnMultiplier = gameState.whiteToMove ? 1 : -1;
    return score * turnMultiplier;
}


bool checkEndGame(GameState gameState)
{
    int wMajorPieceCount = 0;
    int bMajorPieceCount = 0;
    for (int r=0;r<8;r++)
    {
        for (int c=0;c<8;c++)
        {
            std::string square = gameState.board[r][c]; 

            if (square == "wN" || square == "wB" || square == "wR" || square == "wQ")
                wMajorPieceCount++;
            if (square == "bN" || square == "bB" || square == "bR" || square == "bQ")
                bMajorPieceCount++;
        }
    }
    
    if (wMajorPieceCount <= 3 && bMajorPieceCount <= 3)
        return true;
    return false;

}