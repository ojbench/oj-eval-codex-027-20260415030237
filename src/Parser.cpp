#include "Parser.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Expression.hpp"
#include "Statement.hpp"
#include "utils/Error.hpp"

ParsedLine::ParsedLine() { statement_ = nullptr; }

ParsedLine::~ParsedLine() { delete statement_; }

void ParsedLine::setLine(int line) { line_number_.emplace(line); }

std::optional<int> ParsedLine::getLine() { return line_number_; }

void ParsedLine::setStatement(Statement* stmt) { statement_ = stmt; }

Statement* ParsedLine::getStatement() const { return statement_; }

Statement* ParsedLine::fetchStatement() {
  Statement* temp = statement_;
  statement_ = nullptr;
  return temp;
}

ParsedLine Parser::parseLine(TokenStream& tokens,
                             const std::string& originLine) const {
  ParsedLine result;

  const Token* firstToken = tokens.peek();
  if (firstToken && firstToken->type == TokenType::NUMBER) {
    result.setLine(parseLiteral(firstToken));
    tokens.get();
    if (tokens.empty()) {
      return result;
    }
    // compute statement text without leading line number
    int pos = firstToken->column;
    while (pos < static_cast<int>(originLine.size()) &&
           std::isspace(static_cast<unsigned char>(originLine[pos]))) {
      ++pos;
    }
    std::string stmtText = pos <= static_cast<int>(originLine.size())
                               ? originLine.substr(pos)
                               : std::string();
    result.setStatement(parseStatement(tokens, stmtText));
    return result;
  }

  result.setStatement(parseStatement(tokens, originLine));
  if (!tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }
  return result;
}

Statement* Parser::parseStatement(TokenStream& tokens,
                                  const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* token = tokens.get();
  if (!token) {
    throw BasicError("SYNTAX ERROR");
  }

  switch (token->type) {
    case TokenType::LET:
      return parseLet(tokens, originLine);
    case TokenType::PRINT:
      return parsePrint(tokens, originLine);
    case TokenType::INPUT:
      return parseInput(tokens, originLine);
    case TokenType::GOTO:
      return parseGoto(tokens, originLine);
    case TokenType::IF:
      return parseIf(tokens, originLine);
    case TokenType::REM:
      return parseRem(tokens, originLine);
    case TokenType::END:
      return parseEnd(tokens, originLine);
    case TokenType::INDENT:
      return new IndentStatement(originLine);
    case TokenType::DEDENT:
      return new DedentStatement(originLine);
    default:
      throw BasicError("SYNTAX ERROR");
  }
}

Statement* Parser::parseLet(TokenStream& tokens,
                            const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* varToken = tokens.get();
  if (!varToken || varToken->type != TokenType::IDENTIFIER) {
    throw BasicError("SYNTAX ERROR");
  }

  std::string varName = varToken->text;

  if (tokens.empty() || tokens.get()->type != TokenType::EQUAL) {
    throw BasicError("SYNTAX ERROR");
  }

  auto expr = parseExpression(tokens);
  return new LetStatement(originLine, varName, std::unique_ptr<Expression>(expr));
}

Statement* Parser::parsePrint(TokenStream& tokens,
                              const std::string& originLine) const {
  auto expr = parseExpression(tokens);
  return new PrintStatement(originLine, std::unique_ptr<Expression>(expr));
}

Statement* Parser::parseInput(TokenStream& tokens,
                              const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* varToken = tokens.get();
  if (!varToken || varToken->type != TokenType::IDENTIFIER) {
    throw BasicError("SYNTAX ERROR");
  }

  std::string varName = varToken->text;
  return new InputStatement(originLine, varName);
}

Statement* Parser::parseGoto(TokenStream& tokens,
                             const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* lineToken = tokens.get();
  if (!lineToken || lineToken->type != TokenType::NUMBER) {
    throw BasicError("SYNTAX ERROR");
  }

  int targetLine = parseLiteral(lineToken);
  return new GotoStatement(originLine, targetLine);
}

Statement* Parser::parseIf(TokenStream& tokens,
                           const std::string& originLine) const {
  auto leftExpr = parseExpression(tokens);

  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* opToken = tokens.get();
  char op;
  switch (opToken->type) {
    case TokenType::EQUAL:
      op = '=';
      break;
    case TokenType::GREATER:
      op = '>';
      break;
    case TokenType::LESS:
      op = '<';
      break;
    default:
      throw BasicError("SYNTAX ERROR");
  }

  auto rightExpr = parseExpression(tokens);

  if (tokens.empty() || tokens.get()->type != TokenType::THEN) {
    throw BasicError("SYNTAX ERROR");
  }

  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* lineToken = tokens.get();
  if (!lineToken || lineToken->type != TokenType::NUMBER) {
    throw BasicError("SYNTAX ERROR");
  }

  int targetLine = parseLiteral(lineToken);
  return new IfStatement(originLine, std::unique_ptr<Expression>(leftExpr), op,
                         std::unique_ptr<Expression>(rightExpr), targetLine);
}

Statement* Parser::parseRem(TokenStream& tokens,
                            const std::string& originLine) const {
  const Token* remInfo = tokens.get();
  if (!remInfo || remInfo->type != TokenType::REMINFO) {
    throw BasicError("SYNTAX ERROR");
  }
  return new RemStatement(originLine);
}

Statement* Parser::parseEnd(TokenStream& tokens,
                            const std::string& originLine) const {
  return new EndStatement(originLine);
}

Expression* Parser::parseExpression(TokenStream& tokens) const {
  return parseExpression(tokens, 0);
}

Expression* Parser::parseExpression(TokenStream& tokens, int precedence) const {
  Expression* left;

  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* token = tokens.get();
  if (!token) {
    throw BasicError("SYNTAX ERROR");
  }

  if (token->type == TokenType::NUMBER) {
    int value = parseLiteral(token);
    left = new ConstExpression(value);
  } else if (token->type == TokenType::IDENTIFIER) {
    left = new VariableExpression(token->text);
  } else if (token->type == TokenType::LEFT_PAREN) {
    ++leftParentCount;
    left = parseExpression(tokens, 0);

    if (tokens.empty() || tokens.get()->type != TokenType::RIGHT_PAREN) {
      throw BasicError("MISMATCHED PARENTHESIS");
    }
    --leftParentCount;
  } else {
    throw BasicError("SYNTAX ERROR");
  }

  while (!tokens.empty()) {
    const Token* opToken = tokens.peek();
    if (!opToken) break;

    if (opToken->type == TokenType::RIGHT_PAREN) {
      if (leftParentCount == 0) {
        throw BasicError("MISMATCHED PARENTHESIS");
      }
      break;
    }

    int opPrecedence = getPrecedence(opToken->type);
    if (opPrecedence == -1 || opPrecedence < precedence) {
      break;
    }

    tokens.get();

    char op;
    switch (opToken->type) {
      case TokenType::PLUS:
        op = '+';
        break;
      case TokenType::MINUS:
        op = '-';
        break;
      case TokenType::MUL:
        op = '*';
        break;
      case TokenType::DIV:
        op = '/';
        break;
      default:
        throw BasicError("SYNTAX ERROR");
    }

    auto right = parseExpression(tokens, opPrecedence + 1);
    left = new CompoundExpression(left, op, right);
  }

  return left;
}

int Parser::getPrecedence(TokenType op) const {
  switch (op) {
    case TokenType::PLUS:
    case TokenType::MINUS:
      return 1;
    case TokenType::MUL:
    case TokenType::DIV:
      return 2;
    default:
      return -1;
  }
}

int Parser::parseLiteral(const Token* token) const {
  if (!token || token->type != TokenType::NUMBER) {
    throw BasicError("SYNTAX ERROR");
  }

  try {
    size_t pos;
    int value = std::stoi(token->text, &pos);
    if (pos != token->text.length()) {
      throw BasicError("INT LITERAL OVERFLOW");
    }
    return value;
  } catch (const std::out_of_range&) {
    throw BasicError("INT LITERAL OVERFLOW");
  } catch (const std::invalid_argument&) {
    throw BasicError("SYNTAX ERROR");
  }
}
