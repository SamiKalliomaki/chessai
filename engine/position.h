#pragma once

#include <stdint.h>
#include <string>
#include <vector>

typedef int_fast8_t board_coord;
typedef int_fast8_t board_index;

constexpr uint8_t kBoardSize = 8;
constexpr uint8_t kBoardSquares = kBoardSize * kBoardSize;

enum class Player : uint8_t { kWhite, kBlack };

inline Player InverseColor(Player player) {
  return player == Player::kWhite ? Player::kBlack : Player::kWhite;
}

constexpr uint8_t kPieceColorBit = 0b1000;

enum class Piece : uint8_t {
  kNone,

  kWhitePawn = 1,
  kWhiteKnight,
  kWhiteBishop,
  kWhiteRook,
  kWhiteQueen,
  kWhiteKing,
  kFirstWhite = kWhitePawn,
  kLastWhite = kWhiteKing,

  kBlackPawn = kPieceColorBit + 1,
  kBlackKnight,
  kBlackBishop,
  kBlackRook,
  kBlackQueen,
  kBlackKing,
  kFirstBlack = kBlackPawn,
  kLastBlack = kBlackKing,
};

inline bool IsWhitePiece(Piece piece) {
  return piece >= Piece::kFirstWhite && piece <= Piece::kLastWhite;
}

inline bool IsBlackPiece(Piece piece) {
  return piece >= Piece::kFirstBlack && piece <= Piece::kLastBlack;
}

inline Piece WhiteToBlack(Piece piece) {
  return static_cast<Piece>((uint8_t)piece | kPieceColorBit);
}

inline Piece BlackToWhite(Piece piece) {
  return static_cast<Piece>((uint8_t)piece & ~kPieceColorBit);
}

inline Player GetPieceColor(Piece piece) {
  return static_cast<Player>((uint8_t)piece / (uint8_t)Piece::kLastWhite);
}

inline int GetPieceValue(Piece piece) {
  switch (BlackToWhite(piece)) {
  case Piece::kWhitePawn:
    return 1;
  case Piece::kWhiteBishop:
  case Piece::kWhiteKnight:
    return 3;
  case Piece::kWhiteRook:
    return 5;
  case Piece::kWhiteQueen:
    return 9;
  default:
    return 0;
  }
}

struct Move {
  int8_t from;
  int8_t to;
  int8_t promotion;
};

struct Position {
  Player active_player;
  Piece board[kBoardSquares];
};

inline board_index BoardIndex(board_coord row, board_coord file) {
  return row * kBoardSize + file;
}

Move GetMove(std::string const &str);
std::string ToNotation(Position const &position, Move move);

Position GetStartingPosition();
void PlayMove(Position &position, Move move);