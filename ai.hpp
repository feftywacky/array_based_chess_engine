#ifndef AI_HPP
#define AI_HPP

#include <iostream>
#include <chrono>
#include <array>
#include <string>
#include <functional>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <array>
#include "engine.hpp"


extern std::unordered_map<std::string, int> pieceScore_mg;
extern std::unordered_map<std::string, int> pieceScore_eg;

extern std::unordered_map<std::string, std::array<std::array<int, 8>, 8>> piecePositionScores_mg;
extern std::unordered_map<std::string, std::array<std::array<int, 8>, 8>> piecePositionScores_eg;


// WHITE PIECE SQ TABLE
extern std::array<std::array<int, 8>, 8> wKnight_mg;
extern std::array<std::array<int, 8>, 8> wKnight_eg;
extern std::array<std::array<int, 8>, 8> wBishop_mg;
extern std::array<std::array<int, 8>, 8> wBishop_eg;
extern std::array<std::array<int, 8>, 8> wRook_mg;
extern std::array<std::array<int, 8>, 8> wRook_eg;
extern std::array<std::array<int, 8>, 8> wQueen_mg;
extern std::array<std::array<int, 8>, 8> wQueen_eg;
extern std::array<std::array<int, 8>, 8> wKing_mg;
extern std::array<std::array<int, 8>, 8> wKing_eg;
extern std::array<std::array<int, 8>, 8> wPawn_mg;
extern std::array<std::array<int, 8>, 8> wPawn_eg;

// BLACK PIECE SQ TABLE
extern std::array<std::array<int, 8>, 8> bKnight_mg;
extern std::array<std::array<int, 8>, 8> bKnight_eg;
extern std::array<std::array<int, 8>, 8> bBishop_mg;
extern std::array<std::array<int, 8>, 8> bBishop_eg;
extern std::array<std::array<int, 8>, 8> bRook_mg;
extern std::array<std::array<int, 8>, 8> bRook_eg;
extern std::array<std::array<int, 8>, 8> bQueen_mg;
extern std::array<std::array<int, 8>, 8> bQueen_eg;
extern std::array<std::array<int, 8>, 8> bKing_mg;
extern std::array<std::array<int, 8>, 8> bKing_eg;
extern std::array<std::array<int, 8>, 8> bPawn_mg;
extern std::array<std::array<int, 8>, 8> bPawn_eg;

extern const int CHECKMATE;
extern const int STALEMATE;
extern int depth;
extern MovePiece nextMove;
extern int counter;



// FUNCTIONS
template <typename T, size_t Rows, size_t Cols>
std::array<std::array<T, Cols>, Rows> flip(const std::array<std::array<T, Cols>, Rows>& matrix, int axis);

MovePiece findRandomMove(std::vector<MovePiece> validMoves);

MovePiece findBestMove(GameState gameState);

bool checkEndGame(GameState gameState);

int evaluation(GameState gameState);

std::vector<MovePiece> move_ordering(GameState gameState, std::vector<MovePiece> validMoves);

#endif