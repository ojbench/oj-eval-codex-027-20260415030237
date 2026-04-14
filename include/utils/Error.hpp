#pragma once

#include <stdexcept>
#include <string>

class BasicError : public std::runtime_error {
 public:
  explicit BasicError(std::string&& message);

  const std::string& message() const noexcept;

 private:
  std::string message_;
};

