#include "Program.hpp"

#include <iostream>

#include "Statement.hpp"
#include "utils/Error.hpp"

Program::Program() : programCounter_(-1), programEnd_(false) {}

void Program::addStmt(int line, Statement* stmt) { recorder_.add(line, stmt); }

void Program::removeStmt(int line) { recorder_.remove(line); }

void Program::run() {
  // initialize PC to first line
  programEnd_ = false;
  // find minimum line
  int pc = -1;
  pc = recorder_.nextLine(-1);
  programCounter_ = pc;
  while (programCounter_ != -1 && !programEnd_) {
    const int currentLine = programCounter_;
    const Statement* stmt = recorder_.get(currentLine);
    if (!stmt) {
      // line removed, go next
      programCounter_ = recorder_.nextLine(currentLine);
      continue;
    }
    try {
      // execute; statements may change PC
      const_cast<Statement*>(stmt)->execute(vars_, *this);
    } catch (const BasicError& e) {
      std::cout << e.message() << "\n";
      break;
    }
    if (!programEnd_) {
      // If PC unchanged by the statement, advance to next line
      if (programCounter_ == currentLine) {
        programCounter_ = recorder_.nextLine(currentLine);
      }
      // else, keep the PC as set by statement (GOTO/IF)
    }
  }
  resetAfterRun();
}

void Program::list() const { recorder_.printLines(); }

void Program::clear() {
  recorder_.clear();
  vars_.clear();
}

void Program::execute(Statement* stmt) {
  try {
    stmt->execute(vars_, *this);
  } catch (const BasicError& e) {
    std::cout << e.message() << "\n";
  }
}

int Program::getPC() const noexcept { return programCounter_; }

void Program::changePC(int line) { programCounter_ = line; }

void Program::programEnd() { programEnd_ = true; }

void Program::resetAfterRun() noexcept {
  programCounter_ = -1;
  programEnd_ = false;
}
