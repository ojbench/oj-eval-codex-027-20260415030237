#include "Expression.hpp"

#include "VarState.hpp"
#include "utils/Error.hpp"

ConstExpression::ConstExpression(int value) : value_(value) {}

int ConstExpression::evaluate(const VarState&) const { return value_; }

VariableExpression::VariableExpression(std::string name)
    : name_(std::move(name)) {}

int VariableExpression::evaluate(const VarState& state) const {
  return state.getValue(name_);
}

CompoundExpression::CompoundExpression(Expression* left, char op,
                                       Expression* right)
    : left_(left), right_(right), op_(op) {}

CompoundExpression::~CompoundExpression() {
  delete left_;
  delete right_;
}

int CompoundExpression::evaluate(const VarState& state) const {
  int lhs = left_->evaluate(state);
  int rhs = right_->evaluate(state);

  switch (op_) {
    case '+':
      return lhs + rhs;
    case '-':
      return lhs - rhs;
    case '*':
      return lhs * rhs;
    case '/':
      if (rhs == 0) {
        throw BasicError("DIVIDE BY ZERO");
      }
      return lhs / rhs;
    default:
      throw BasicError("UNSUPPORTED OPERATOR");
  }
}

