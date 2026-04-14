#pragma once

#include <string>
#include <vector>

enum class TokenType {
  // Keywords
  LET,
  PRINT,
  INPUT,
  END,
  REM,
  GOTO,
  IF,
  THEN,
  RUN,
  LIST,
  CLEAR,
  QUIT,
  HELP,
  INDENT,
  DEDENT,
  // Basic lexical units
  IDENTIFIER,
  NUMBER,
  REMINFO,
  // Operators and symbols
  PLUS,
  MINUS,
  MUL,
  DIV,
  EQUAL,
  GREATER,
  LESS,
  LEFT_PAREN,
  RIGHT_PAREN,
  COMMA,
  // Reserved placeholder
  UNKNOWN
};

struct Token {
  TokenType type{TokenType::UNKNOWN};
  std::string text{};
  int column{0};
};

class TokenStream {
 public:
  TokenStream() = default;
  explicit TokenStream(std::vector<Token>&& tokens);

  const Token* peek() const;
  const Token* get();
  bool empty() const;
  void reset();

  int position() const;
  int size() const;

  void push(Token&& token);
  const std::vector<Token>& data() const;

 private:
  std::vector<Token> tokens_{};
  int cursor_{0};
};
