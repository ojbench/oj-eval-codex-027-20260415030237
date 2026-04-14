#include "utils/Error.hpp"

#include <utility>

BasicError::BasicError(std::string&& message)
    : std::runtime_error(message), message_(std::move(message)) {}

const std::string& BasicError::message() const noexcept { return message_; }

