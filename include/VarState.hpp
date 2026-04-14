#pragma once

#include <string>
#include <unordered_map>

class VarState {
 public:
  void setValue(const std::string& name, int value);
  int getValue(const std::string& name) const;
  void clear();

 private:
  std::unordered_map<std::string, int> values_;
};

