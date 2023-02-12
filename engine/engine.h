#pragma once

#include "position.h"
#include <unordered_set>

struct HashEntry {
  Position position;
};

template <> struct std::hash<HashEntry> {
  std::size_t operator()(HashEntry const &e) const noexcept {
    std::size_t hash_value = 0;
    for (uint8_t i = 0; i < kBoardSquares; i++) {
      hash_value <<= 1;
      hash_value ^= (std::size_t)e.position.board[i];
    }

    hash_value <<= 1;
    hash_value ^= (std::size_t)e.position.active_player;

    return hash_value;
  }
};

class Engine {
public:
  void EnterPosition(Position const &position);
  void StartSearch();
  Move GetBestMove();

private:
  Position _current_position;
  std::unordered_set<HashEntry> _hash_table;
};