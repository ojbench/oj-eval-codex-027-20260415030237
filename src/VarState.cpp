#include "VarState.hpp"

#include <algorithm>

#include "utils/Error.hpp"

void VarState::setValue(const std::string& name, int value) { values_[name] = value; }

int VarState::getValue(const std::string& name) const {
  auto it = values_.find(name);
  if (it == values_.end()) {
    throw BasicError("VARIABLE NOT DEFINED");
  }
  return it->second;
}

void VarState::clear() { values_.clear(); }

