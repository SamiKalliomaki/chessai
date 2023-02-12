#include "engine.h"
#include <algorithm>
#include <array>
#include <stdint.h>
#include <vector>

static void AppendMoveIfLegal(Position const &position,
                              std::vector<Move> &moves, board_coord row,
                              board_coord file, board_coord row_diff,
                              board_coord file_diff, Player enemy_color) {
  board_coord to_row = row + row_diff;
  board_coord to_file = file + file_diff;

  if (to_row < 0 || to_row >= kBoardSize)
    return;
  if (to_file < 0 || to_file >= kBoardSize)
    return;
  Piece piece = position.board[BoardIndex(to_row, to_file)];
  if (piece != Piece::kNone && GetPieceColor(piece) != enemy_color)
    return;

  moves.push_back(Move{.from = BoardIndex(row, file),
                       .to = BoardIndex(to_row, to_file),
                       .promotion = 0});
}

static void AppendWhitePawnMoves(Position const &position,
                                 std::vector<Move> &moves, board_coord row,
                                 board_coord file) {
  if (row >= kBoardSize - 1)
    return;

  std::vector<Move> base_moves;

  if (file > 0 && IsBlackPiece(position.board[BoardIndex(row + 1, file - 1)])) {
    base_moves.push_back(Move{.from = BoardIndex(row, file),
                              .to = BoardIndex(row + 1, file - 1),
                              .promotion = 0});
  }

  if (file < kBoardSize - 1 &&
      IsBlackPiece(position.board[BoardIndex(row + 1, file + 1)])) {
    base_moves.push_back(Move{.from = BoardIndex(row, file),
                              .to = BoardIndex(row + 1, file + 1),
                              .promotion = 0});
  }

  if (position.board[BoardIndex(row + 1, file)] == Piece::kNone) {
    base_moves.push_back(Move{.from = BoardIndex(row, file),
                              .to = BoardIndex(row + 1, file),
                              .promotion = 0});
  }

  if (row == 1 && position.board[BoardIndex(row + 1, file)] == Piece::kNone &&
      position.board[BoardIndex(row + 2, file)] == Piece::kNone) {
    base_moves.push_back(Move{.from = BoardIndex(row, file),
                              .to = BoardIndex(row + 2, file),
                              .promotion = 0});
  }

  constexpr std::array<Piece, 4> promotions{
      Piece::kWhiteBishop, Piece::kWhiteKnight, Piece::kWhiteRook,
      Piece::kWhiteQueen};
  if (row == kBoardSize - 2) {
    for (Piece promotion : promotions) {
      for (Move move : base_moves) {
        move.promotion = (uint8_t)promotion - (uint8_t)Piece::kWhitePawn;
        moves.push_back(move);
      }
    }
  } else {
    moves.insert(moves.end(), base_moves.begin(), base_moves.end());
  }
}

static void AppendBlackPawnMoves(Position const &position,
                                 std::vector<Move> &moves, board_coord row,
                                 board_coord file) {
  if (row == 0)
    return;

  std::vector<Move> base_moves;

  if (file > 0 && IsWhitePiece(position.board[BoardIndex(row - 1, file - 1)])) {
    base_moves.push_back(Move{.from = BoardIndex(row, file),
                              .to = BoardIndex(row - 1, file - 1),
                              .promotion = 0});
  }

  if (file < kBoardSize - 1 &&
      IsWhitePiece(position.board[BoardIndex(row - 1, file + 1)])) {
    base_moves.push_back(Move{.from = BoardIndex(row, file),
                              .to = BoardIndex(row - 1, file + 1),
                              .promotion = 0});
  }

  if (position.board[BoardIndex(row - 1, file)] == Piece::kNone) {
    base_moves.push_back(Move{.from = BoardIndex(row, file),
                              .to = BoardIndex(row - 1, file),
                              .promotion = 0});
  }

  if (row == kBoardSize - 2 &&
      position.board[BoardIndex(row - 1, file)] == Piece::kNone &&
      position.board[BoardIndex(row - 2, file)] == Piece::kNone) {
    base_moves.push_back(Move{.from = BoardIndex(row, file),
                              .to = BoardIndex(row - 2, file),
                              .promotion = 0});
  }

  constexpr std::array<Piece, 4> promotions{
      Piece::kBlackBishop, Piece::kBlackKnight, Piece::kBlackRook,
      Piece::kBlackQueen};
  if (row == 1) {
    for (Piece promotion : promotions) {
      for (Move move : base_moves) {
        move.promotion = (uint8_t)promotion - (uint8_t)Piece::kBlackPawn;
        moves.push_back(move);
      }
    }
  } else {
    moves.insert(moves.end(), base_moves.begin(), base_moves.end());
  }
}

static void AppendKnightMoves(Position const &position,
                              std::vector<Move> &moves, board_coord row,
                              board_coord file, Player enemy_color) {
  AppendMoveIfLegal(position, moves, row, file, -2, -1, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, -2, 1, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, -1, -2, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, -1, 2, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, 2, -1, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, 2, 1, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, 1, -2, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, 1, 2, enemy_color);
}

static void AppendTravelMoves(Position const &position,
                              std::vector<Move> &moves, board_coord row,
                              board_coord file, board_coord row_diff,
                              board_coord file_diff, Player enemy_color) {
  board_coord to_row = row;
  board_coord to_file = file;

  while (true) {
    to_row += row_diff;
    to_file += file_diff;

    if (to_row < 0 || to_row >= kBoardSize)
      return;
    if (to_file < 0 || to_file >= kBoardSize)
      return;

    Piece piece = position.board[BoardIndex(to_row, to_file)];
    if (piece != Piece::kNone && GetPieceColor(piece) != enemy_color)
      return;

    moves.push_back(Move{.from = BoardIndex(row, file),
                         .to = BoardIndex(to_row, to_file),
                         .promotion = 0});

    if (position.board[BoardIndex(to_row, to_file)] != Piece::kNone)
      return;
  }
}

static void AppendBishopMoves(Position const &position,
                              std::vector<Move> &moves, board_coord row,
                              board_coord file, Player enemy_color) {
  AppendTravelMoves(position, moves, row, file, -1, -1, enemy_color);
  AppendTravelMoves(position, moves, row, file, -1, 1, enemy_color);
  AppendTravelMoves(position, moves, row, file, 1, -1, enemy_color);
  AppendTravelMoves(position, moves, row, file, 1, 1, enemy_color);
}

static void AppendRookMoves(Position const &position, std::vector<Move> &moves,
                            board_coord row, board_coord file,
                            Player enemy_color) {
  AppendTravelMoves(position, moves, row, file, -1, 0, enemy_color);
  AppendTravelMoves(position, moves, row, file, 1, 0, enemy_color);
  AppendTravelMoves(position, moves, row, file, 0, -1, enemy_color);
  AppendTravelMoves(position, moves, row, file, 0, 1, enemy_color);
}

static void AppendKingMoves(Position const &position, std::vector<Move> &moves,
                            board_coord row, board_coord file,
                            Player enemy_color) {
  AppendMoveIfLegal(position, moves, row, file, -1, -1, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, -1, 0, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, -1, 1, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, 0, -1, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, 0, 1, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, 1, -1, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, 1, 0, enemy_color);
  AppendMoveIfLegal(position, moves, row, file, 1, 1, enemy_color);
}

static void AppendPieceMoves(Position const &position, std::vector<Move> &moves,
                             board_coord row, board_coord file,
                             Player enemy_color) {
  Piece piece = BlackToWhite(position.board[BoardIndex(row, file)]);

  switch (piece) {
  case Piece::kWhiteKnight:
    AppendKnightMoves(position, moves, row, file, enemy_color);
    break;
  case Piece::kWhiteBishop:
    AppendBishopMoves(position, moves, row, file, enemy_color);
    break;
  case Piece::kWhiteRook:
    AppendRookMoves(position, moves, row, file, enemy_color);
    break;
  case Piece::kWhiteQueen:
    AppendBishopMoves(position, moves, row, file, enemy_color);
    AppendRookMoves(position, moves, row, file, enemy_color);
    break;
  case Piece::kWhiteKing:
    AppendKingMoves(position, moves, row, file, enemy_color);
    break;
  default:
    break;
  }
}

static Piece GetPiece(Position const &position, board_coord row,
                      board_coord file) {
  if (row < 0 || row >= kBoardSize)
    return Piece::kNone;
  if (file < 0 || file >= kBoardSize)
    return Piece::kNone;
  return position.board[BoardIndex(row, file)];
}

static Piece FirstPieceAtDirection(Position const &position, board_coord row,
                                   board_coord file, board_coord row_diff,
                                   board_coord file_diff) {
  board_coord to_row = row;
  board_coord to_file = file;

  while (true) {
    to_row += row_diff;
    to_file += file_diff;

    if (to_row < 0 || to_row >= kBoardSize)
      return Piece::kNone;
    if (to_file < 0 || to_file >= kBoardSize)
      return Piece::kNone;

    Piece piece = position.board[BoardIndex(to_row, to_file)];
    if (piece != Piece::kNone)
      return piece;
  }
}

static bool IsAttacked(Position const &position, board_coord row,
                       board_coord file, Player enemy_color) {
  // TODO: We could do Piece::kWhiteKnight + enemy_color * difference
  Piece enemy_knight =
      enemy_color == Player::kWhite ? Piece::kWhiteKnight : Piece::kBlackKnight;
  Piece enemy_bishop =
      enemy_color == Player::kWhite ? Piece::kWhiteBishop : Piece::kBlackBishop;
  Piece enemy_rook =
      enemy_color == Player::kWhite ? Piece::kWhiteRook : Piece::kBlackRook;
  Piece enemy_queen =
      enemy_color == Player::kWhite ? Piece::kWhiteQueen : Piece::kBlackQueen;
  Piece enemy_pawn =
      enemy_color == Player::kWhite ? Piece::kWhitePawn : Piece::kBlackPawn;

  if (GetPiece(position, row - 2, file - 1) == enemy_knight)
    return true;
  if (GetPiece(position, row - 2, file + 1) == enemy_knight)
    return true;
  if (GetPiece(position, row - 1, file - 2) == enemy_knight)
    return true;
  if (GetPiece(position, row - 1, file + 2) == enemy_knight)
    return true;
  if (GetPiece(position, row + 1, file - 2) == enemy_knight)
    return true;
  if (GetPiece(position, row + 1, file + 2) == enemy_knight)
    return true;
  if (GetPiece(position, row + 2, file - 1) == enemy_knight)
    return true;
  if (GetPiece(position, row + 2, file + 1) == enemy_knight)
    return true;

  Piece piece = FirstPieceAtDirection(position, row, file, -1, 0);
  if (piece == enemy_rook || piece == enemy_queen)
    return true;
  piece = FirstPieceAtDirection(position, row, file, 1, 0);
  if (piece == enemy_rook || piece == enemy_queen)
    return true;
  piece = FirstPieceAtDirection(position, row, file, 0, -1);
  if (piece == enemy_rook || piece == enemy_queen)
    return true;
  piece = FirstPieceAtDirection(position, row, file, 0, 1);
  if (piece == enemy_rook || piece == enemy_queen)
    return true;

  piece = FirstPieceAtDirection(position, row, file, -1, -1);
  if (piece == enemy_bishop || piece == enemy_queen)
    return true;
  piece = FirstPieceAtDirection(position, row, file, -1, 1);
  if (piece == enemy_bishop || piece == enemy_queen)
    return true;
  piece = FirstPieceAtDirection(position, row, file, 1, -1);
  if (piece == enemy_bishop || piece == enemy_queen)
    return true;
  piece = FirstPieceAtDirection(position, row, file, 1, 1);
  if (piece == enemy_bishop || piece == enemy_queen)
    return true;

  board_coord pawn_row_delta = enemy_color == Player::kWhite ? -1 : 1;
  if (GetPiece(position, row + pawn_row_delta, file - 1) == enemy_pawn)
    return true;
  if (GetPiece(position, row + pawn_row_delta, file + 1) == enemy_pawn)
    return true;

  return false;
}

static std::vector<Move> GetPseudoLegalMoves(Position const &position) {
  std::vector<Move> moves;

  if (position.active_player == Player::kWhite) {
    for (board_coord row = 0; row < kBoardSize; row++) {
      for (board_coord file = 0; file < kBoardSize; file++) {
        Piece piece = position.board[BoardIndex(row, file)];
        if (piece == Piece::kWhitePawn) {
          AppendWhitePawnMoves(position, moves, row, file);
        } else if (IsWhitePiece(piece)) {
          AppendPieceMoves(position, moves, row, file, Player::kBlack);
        }
      }
    }
  } else {
    for (board_coord row = 0; row < kBoardSize; row++) {
      for (board_coord file = 0; file < kBoardSize; file++) {
        Piece piece = position.board[BoardIndex(row, file)];
        if (piece == Piece::kBlackPawn) {
          AppendBlackPawnMoves(position, moves, row, file);
        } else if (IsBlackPiece(piece)) {
          AppendPieceMoves(position, moves, row, file, Player::kWhite);
        }
      }
    }
  }

  return moves;
}

static bool IsLegalPosition(Position const &position) {
  Piece enemy_king = position.active_player == Player::kWhite
                         ? Piece::kBlackKing
                         : Piece::kWhiteKing;

  // Check if the current active player can capture enemy queen -> illegal
  // position.
  for (board_coord row = 0; row < kBoardSize; row++) {
    for (board_coord file = 0; file < kBoardSize; file++) {
      if (position.board[BoardIndex(row, file)] == enemy_king) {
        return !IsAttacked(position, row, file, position.active_player);
      }
    }
  }

  // No king on the board???
  return false;
}

static constexpr int kInfiniteScore = 9999;

static int ScorePosition(Position const &position) {
  int score = 0;
  for (board_index i = 0; i < kBoardSquares; i++) {
    Piece piece = position.board[i];
    if (IsWhitePiece(piece)) {
      score += GetPieceValue(piece);
    } else {
      score -= GetPieceValue(piece);
    }
  }
  return score;
}

void Engine::EnterPosition(Position const &position) {
  _current_position = position;
}

void Engine::StartSearch() {}

Move Engine::GetBestMove() {
  auto moves = GetPseudoLegalMoves(_current_position);

  std::sort(moves.begin(), moves.end(), [this](Move const &a, Move const &b) {
    Position position_a, position_b;
    position_a = position_b = _current_position;

    PlayMove(position_a, a);
    PlayMove(position_b, b);

    int score_a = ScorePosition(position_a);
    int score_b = ScorePosition(position_b);

    if (_current_position.active_player == Player::kWhite) {
      return score_b < score_a;
    } else {
      return score_a < score_b;
    }
  });

  for (Move const &move : moves) {
    Position position = _current_position;
    PlayMove(position, move);

    if (IsLegalPosition(position))
      return move;
  }

  return Move();
}