#include <iostream>
#include <memory>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include "Token.hpp"
#include "Statement.hpp"
#include "utils/Error.hpp"

static void print_help() {
  std::cout << "RUN" << "\n";
  std::cout << "LIST" << "\n";
  std::cout << "CLEAR" << "\n";
  std::cout << "QUIT" << "\n";
  std::cout << "HELP" << "\n";
}

int main() {
  Lexer lexer;
  Parser parser;
  Program program;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    try {
      TokenStream tokens = lexer.tokenize(line);
      if (tokens.empty()) continue;

      const Token* first = tokens.peek();
      if (first && first->type == TokenType::RUN) {
        program.run();
        continue;
      }
      if (first && first->type == TokenType::LIST) {
        program.list();
        continue;
      }
      if (first && first->type == TokenType::CLEAR) {
        program.clear();
        continue;
      }
      if (first && first->type == TokenType::QUIT) {
        break;
      }
      if (first && first->type == TokenType::HELP) {
        print_help();
        continue;
      }

      auto parsed = parser.parseLine(tokens, line);
      auto lineNum = parsed.getLine();
      if (lineNum.has_value()) {
        // program line or deletion
        Statement* stmt = parsed.fetchStatement();
        if (!stmt) {
          program.removeStmt(lineNum.value());
        } else {
          program.addStmt(lineNum.value(), stmt);
        }
      } else {
        // immediate execution
        Statement* stmt = parsed.fetchStatement();
        program.execute(stmt);
        delete stmt;
      }
    } catch (const BasicError& e) {
      std::cout << e.message() << "\n";
    }
  }
  return 0;
}
