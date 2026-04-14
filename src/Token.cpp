#include "Token.hpp"

TokenStream::TokenStream(std::vector<Token>&& tokens)
    : tokens_(std::move(tokens)), cursor_(0) {}

const Token* TokenStream::peek() const {
  if (cursor_ >= static_cast<int>(tokens_.size())) {
    return nullptr;
  }
  return &tokens_[cursor_];
}

const Token* TokenStream::get() {
  const Token* current = peek();
  if (current != nullptr) {
    ++cursor_;
  }
  return current;
}

bool TokenStream::empty() const { return cursor_ >= static_cast<int>(tokens_.size()); }

void TokenStream::reset() { cursor_ = 0; }

int TokenStream::position() const { return cursor_; }

int TokenStream::size() const { return static_cast<int>(tokens_.size()); }

void TokenStream::push(Token&& token) { tokens_.push_back(std::move(token)); }

const std::vector<Token>& TokenStream::data() const { return tokens_; }

