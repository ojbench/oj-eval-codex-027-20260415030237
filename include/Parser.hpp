#pragma once

#include <memory>
#include <optional>

#include "Token.hpp"

class Statement;
class Expression;

class ParsedLine {
 private:
  std::optional<int> line_number_;
  Statement* statement_;

 public:
  ParsedLine();
  ~ParsedLine();

  void setLine(int line);
  std::optional<int> getLine();
  void setStatement(Statement* stmt);
  Statement* getStatement() const;
  Statement* fetchStatement();
};

class Parser {
 public:
  ParsedLine parseLine(TokenStream& tokens, const std::string& originLine) const;

 private:
  Statement* parseStatement(TokenStream& tokens, const std::string& originLine) const;
  Statement* parseLet(TokenStream& tokens, const std::string& originLine) const;
  Statement* parsePrint(TokenStream& tokens, const std::string& originLine) const;
  Statement* parseInput(TokenStream& tokens, const std::string& originLine) const;
  Statement* parseGoto(TokenStream& tokens, const std::string& originLine) const;
  Statement* parseIf(TokenStream& tokens, const std::string& originLine) const;
  Statement* parseRem(TokenStream& tokens, const std::string& originLine) const;
  Statement* parseEnd(TokenStream& tokens, const std::string& originLine) const;

  Expression* parseExpression(TokenStream& tokens) const;
  Expression* parseExpression(TokenStream& tokens, int precedence) const;

  int getPrecedence(TokenType op) const;
  int parseLiteral(const Token* token) const;

  mutable int leftParentCount{0};
};

