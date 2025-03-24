#ifndef TOKENTABLE_HPP_INCLUDED
#define TOKENTABLE_HPP_INCLUDED

#include <vector>
#include "Token.hpp"

class TokensTable {
public:
    enum class Mode {
        Saving,
        Reading
    };

public:
    template <typename T>
    void add(T &&entry) {
        tokens_.emplace_back(std::forward<T>(entry));
        tokenRecord_ = tokens_.back();
    }

    size_t count() const { return tokens_.size(); }

    TokenRecord nextToken();
    const TokenRecord &token() const { return tokenRecord_; };
    void setMode(Mode m) { mode_ = m; }
    void reset() { ptr_ = 0; }
    void clearCode() { tokens_.clear(); }

private:
    std::vector<TokenRecord> tokens_;
    TokenRecord tokenRecord_;  // FIXME do i need this ?
    size_t ptr_ = 0;
    Mode mode_ = Mode::Saving;
};

#endif