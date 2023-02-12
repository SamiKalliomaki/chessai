#include "position.h"
#include <array>
#include <assert.h>
#include <stdint.h>
#include <string>

static uint8_t FromNotationSquare(char file, char row) {
  assert(file >= 'a');
  assert(file <= 'a' + kBoardSize);
  assert(row >= '1');
  assert(row <= '1' + kBoardSize);

  return static_cast<uint8_t>(row - '1') * kBoardSize +
         static_cast<uint8_t>(file - 'a');
}

static uint8_t PromoteTo(Piece piece) {
  assert(IsWhitePiece(piece));
  return (uint8_t)piece - (uint8_t)Piece::kWhitePawn;
}

static std::string NotationSquare(uint8_t pos) {
  uint8_t pos_row = pos / kBoardSize;
  uint8_t pos_file = pos % kBoardSize;

  std::string str = "a1";
  str[0] += pos_file;
  str[1] += pos_row;
  return str;
}

Move GetMove(std::string const &str) {
  assert(str.length() >= 4);

  size_t start_index = 0;
  // Some moves start with the type of piece moving, we want to ignore it.
  if (str[0] >= 'A' && str[0] <= 'Z') {
    assert(str.length() >= 5);
    start_index = 1;
  }

  Move move;
  move.from = FromNotationSquare(str[start_index + 0], str[start_index + 1]);

  if (str[start_index + 2] == 'x') {
    assert(str.length() >= start_index + 5);
    start_index++;
  }
  move.to = FromNotationSquare(str[start_index + 2], str[start_index + 3]);

  if (str.length() >= start_index + 5) {
    switch (str[start_index + 4]) {
    case 'q':
      move.promotion = PromoteTo(Piece::kWhiteQueen);
      break;
    case 'r':
      move.promotion = PromoteTo(Piece::kWhiteRook);
      break;
    case 'b':
      move.promotion = PromoteTo(Piece::kWhiteBishop);
      break;
    case 'n':
      move.promotion = PromoteTo(Piece::kWhiteKnight);
      break;
    default:
      assert(false);
    }
  }

  return move;
}

std::string ToNotation(Position const &position, Move move) {
  std::string result;
  // switch (BlackToWhite(position.board[move.from])) {
  // case Piece::kWhiteKnight:
  //   result += 'N';
  //   break;
  // case Piece::kWhiteBishop:
  //   result += 'B';
  //   break;
  // case Piece::kWhiteRook:
  //   result += 'R';
  //   break;
  // case Piece::kWhiteQueen:
  //   result += 'Q';
  //   break;
  // case Piece::kWhiteKing:
  //   result += 'K';
  //   break;
  // default:
  //   break;
  // }

  result += NotationSquare(move.from);
  // if (position.board[move.to] != Piece::kNone) {
  //   result += 'x';
  // }
  result += NotationSquare(move.to);

  if (move.promotion == PromoteTo(Piece::kWhiteKnight)) {
    result += 'n';
  } else if (move.promotion == PromoteTo(Piece::kWhiteBishop)) {
    result += 'b';
  } else if (move.promotion == PromoteTo(Piece::kWhiteRook)) {
    result += 'r';
  } else if (move.promotion == PromoteTo(Piece::kWhiteQueen)) {
    result += 'q';
  }

  return result;
}

Position GetStartingPosition() {
  Position position = Position();

  constexpr std::array<Piece, kBoardSize> piece_row{
      Piece::kWhiteRook,   Piece::kWhiteKnight, Piece::kWhiteBishop,
      Piece::kWhiteQueen,  Piece::kWhiteKing,   Piece::kWhiteBishop,
      Piece::kWhiteKnight, Piece::kWhiteRook,
  };

  for (uint8_t i = 0; i < kBoardSize; i++) {
    position.board[i] = piece_row[i];
  }
  for (uint8_t i = 0; i < kBoardSize; i++) {
    position.board[i + kBoardSize * (kBoardSize - 1)] =
        WhiteToBlack(piece_row[i]);
  }
  for (uint8_t i = 0; i < kBoardSize; i++) {
    position.board[kBoardSize + i] = Piece::kWhitePawn;
  }
  for (uint8_t i = 0; i < kBoardSize; i++) {
    position.board[kBoardSize * (kBoardSize - 2) + i] = Piece::kBlackPawn;
  }

  return position;
}

void PlayMove(Position &position, Move move) {
  position.board[move.to] =
      static_cast<Piece>((uint8_t)position.board[move.from] + move.promotion);
  position.board[move.from] = Piece::kNone;
  position.active_player = position.active_player == Player::kWhite
                               ? Player::kBlack
                               : Player::kWhite;
}