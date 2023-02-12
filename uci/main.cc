#include <iostream>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream>

#include "engine/engine.h"
#include "engine/position.h"

class UCI {
public:
  UCI(std::istream &uci_in, std::ostream &uci_out)
      : _uci_in(uci_in), _uci_out(uci_out), _fatal_error(false) {}

  void run();

private:
  void handlePosition(std::istream &stream);
  void printError(std::string msg);

  std::istream &_uci_in;
  std::ostream &_uci_out;
  bool _fatal_error;
  Position _last_position;
};

void UCI::run() {
  std::string line;
  std::unique_ptr<Engine> engine = std::make_unique<Engine>();

  while (!_fatal_error && std::getline(_uci_in, line)) {
    std::istringstream stream(line);
    std::string command;
    stream >> command;

    if (command == "uci") {
      _uci_out << "id name Sami's Chess Engine" << std::endl;
      _uci_out << "id author Sami Kalliomäki" << std::endl;
      _uci_out << "uciok" << std::endl;
    } else if (command == "debug") {
      // Do nothing...
    } else if (command == "isready") {
      _uci_out << "readyok" << std::endl;
    } else if (command == "setoption") {
      // Do nothing...
    } else if (command == "register") {
      // Do nothing...
    } else if (command == "ucinewgame") {
      engine = std::make_unique<Engine>();
    } else if (command == "position") {
      handlePosition(stream);
    } else if (command == "go") {
      engine->EnterPosition(_last_position);
      engine->StartSearch();
      _uci_out << "bestmove "
               << ToNotation(_last_position, engine->GetBestMove())
               << std::endl;
    } else if (command == "stop") {
      // Do nothing...
    } else if (command == "ponderhit") {
      // Do nothing...
    } else if (command == "quit") {
      return;
    } else {
      _uci_out << "info string Unknown command: " << command << std::endl;
    }
  }
}

void UCI::handlePosition(std::istream &stream) {
  std::string format;
  stream >> format;

  if (format == "startpos") {
    _last_position = GetStartingPosition();
  } else if (format == "fen") {
    printError("Does not support fen positions yet");
    return;
  } else {
    printError("Unknown format: " + format);
    return;
  }

  std::string move;
  stream >> move;
  if (move != "moves") {
    printError("expected moves");
    return;
  }

  while (!stream.eof()) {
    stream >> move;
    PlayMove(_last_position, GetMove(move));
  }
}

void UCI::printError(std::string msg) {
  _uci_out << "info string Error: " << msg << std::endl;
  _fatal_error = true;
}

int main() {
  UCI uci(std::cin, std::cout);
  uci.run();
  return 0;
}