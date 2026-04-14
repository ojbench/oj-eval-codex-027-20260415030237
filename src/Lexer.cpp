#include "Lexer.hpp"

#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/Error.hpp"

const std::unordered_map<std::string, TokenType> TABLE = {
    {"LET", TokenType::LET},     {"PRINT", TokenType::PRINT},
    {"INPUT", TokenType::INPUT}, {"END", TokenType::END},
    {"REM", TokenType::REM},     {"GOTO", TokenType::GOTO},
    {"IF", TokenType::IF},       {"THEN", TokenType::THEN},
    {"RUN", TokenType::RUN},     {"LIST", TokenType::LIST},
    {"CLEAR", TokenType::CLEAR}, {"QUIT", TokenType::QUIT},
    {"HELP", TokenType::HELP}};

TokenStream Lexer::tokenize(const std::string& line) const {
  std::vector<Token> tokens;
  int column = 0;
  while (column < static_cast<int>(line.size())) {
    char ch = line[column];
    if (std::isspace(static_cast<int>(ch))) {
      ++column;
      continue;
    }

    if (isLetterChar(ch)) {
      int start = column;
      ++column;
      while (column < static_cast<int>(line.size()) && isLetterChar(line[column])) {
        ++column;
      }
      std::string text = line.substr(start, column - start);
      TokenType type = matchKeyword(text);
      switch (type) {
        case TokenType::REM:
          tokens.push_back(Token{TokenType::REM, text, column});
          if (column < static_cast<int>(line.size())) {
            std::string comment = line.substr(column);
            tokens.push_back(Token{TokenType::REMINFO, comment, column + 1});
          }
          return TokenStream(std::move(tokens));
        case TokenType::UNKNOWN:
          tokens.push_back(Token{TokenType::IDENTIFIER, text, column});
          break;
        default:
          tokens.push_back(Token{type, text, column});
      }
      continue;
    }

    if (isNumberChar(ch)) {
      int start = column;
      while (column < static_cast<int>(line.size()) && isNumberChar(line[column])) {
        ++column;
      }
      std::string text = line.substr(start, column - start);
      tokens.push_back(Token{TokenType::NUMBER, text, column});
      continue;
    }

    TokenType symbolType = TokenType::UNKNOWN;
    switch (ch) {
      case '+':
        symbolType = TokenType::PLUS;
        break;
      case '-':
        symbolType = TokenType::MINUS;
        break;
      case '*':
        symbolType = TokenType::MUL;
        break;
      case '/':
        symbolType = TokenType::DIV;
        break;
      case '=':
        symbolType = TokenType::EQUAL;
        break;
      case '>':
        symbolType = TokenType::GREATER;
        break;
      case '<':
        symbolType = TokenType::LESS;
        break;
      case '(':
        symbolType = TokenType::LEFT_PAREN;
        break;
      case ')':
        symbolType = TokenType::RIGHT_PAREN;
        break;
      case ',':
        symbolType = TokenType::COMMA;
        break;
      default:
        break;
    }
    if (symbolType != TokenType::UNKNOWN) {
      tokens.push_back(Token{symbolType, std::string(1, ch), column});
      ++column;
      continue;
    }

    throw BasicError("Unexpected character '" + std::string(1, ch) +
                     "' at column " + std::to_string(column));
  }
  return TokenStream(std::move(tokens));
}

bool Lexer::isLetterChar(char ch) noexcept {
  return std::isalpha(static_cast<unsigned char>(ch));
}

bool Lexer::isNumberChar(char ch) noexcept {
  return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
}

TokenType Lexer::matchKeyword(const std::string& text) noexcept {
  auto it = TABLE.find(text);
  if (it != TABLE.end()) {
    return it->second;
  }
  return TokenType::UNKNOWN;
}

