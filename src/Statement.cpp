#include "Statement.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

#include "Program.hpp"
#include "VarState.hpp"
#include "utils/Error.hpp"

Statement::Statement(std::string source) : source_(std::move(source)) {}

const std::string& Statement::text() const noexcept { return source_; }

// Let
LetStatement::LetStatement(std::string source, std::string var,
                           std::unique_ptr<Expression> expr)
    : Statement(std::move(source)), var_(std::move(var)), expr_(std::move(expr)) {}

void LetStatement::execute(VarState& state, Program&) const {
  int val = expr_->evaluate(state);
  state.setValue(var_, val);
}

// Print
PrintStatement::PrintStatement(std::string source,
                               std::unique_ptr<Expression> expr)
    : Statement(std::move(source)), expr_(std::move(expr)) {}

void PrintStatement::execute(VarState& state, Program&) const {
  int val = expr_->evaluate(state);
  std::cout << val << "\n";
}

// Input
InputStatement::InputStatement(std::string source, std::string var)
    : Statement(std::move(source)), var_(std::move(var)) {}

void InputStatement::execute(VarState& state, Program&) const {
  std::cout << "? " << std::flush;
  long long v;
  if (!(std::cin >> v)) {
    throw BasicError("INVALID NUMBER");
  }
  if (v > std::numeric_limits<int>::max() ||
      v < std::numeric_limits<int>::min()) {
    throw BasicError("INPUT OVERFLOW");
  }
  state.setValue(var_, static_cast<int>(v));
  // eat trailing newline if present so next getline in main() works
  if (std::cin.peek() == '\n') {
    std::cin.get();
  }
}

// Goto
GotoStatement::GotoStatement(std::string source, int target)
    : Statement(std::move(source)), target_(target) {}

void GotoStatement::execute(VarState&, Program& program) const {
  program.changePC(target_);
}

// If
IfStatement::IfStatement(std::string source, std::unique_ptr<Expression> lhs,
                         char op, std::unique_ptr<Expression> rhs, int target)
    : Statement(std::move(source)), lhs_(std::move(lhs)), rhs_(std::move(rhs)), op_(op), target_(target) {}

void IfStatement::execute(VarState& state, Program& program) const {
  int lv = lhs_->evaluate(state);
  int rv = rhs_->evaluate(state);
  bool cond = false;
  switch (op_) {
    case '=': cond = (lv == rv); break;
    case '<': cond = (lv < rv); break;
    case '>': cond = (lv > rv); break;
    default: throw BasicError("SYNTAX ERROR");
  }
  if (cond) program.changePC(target_);
}

// Rem
RemStatement::RemStatement(std::string source) : Statement(std::move(source)) {}

void RemStatement::execute(VarState&, Program&) const { /* no-op */ }

// End
EndStatement::EndStatement(std::string source) : Statement(std::move(source)) {}

void EndStatement::execute(VarState&, Program& program) const {
  program.programEnd();
}

