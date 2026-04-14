#pragma once

#include <string>

#include "Token.hpp"

class Lexer {
 public:
  TokenStream tokenize(const std::string& line) const;

 private:
  static bool isLetterChar(char ch) noexcept;
  static bool isNumberChar(char ch) noexcept;
  static TokenType matchKeyword(const std::string& text) noexcept;
};

