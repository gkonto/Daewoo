#include "TokenTable.hpp"
#include "Token.hpp"

TokenRecord TokensTable::nextToken() {
    if (mode_ == Mode::Saving) {
        tokenRecord_ = tokens_.back();
        return tokenRecord_;
    } else {
        if (ptr_ < count()) {
            tokenRecord_ = tokens_[ptr_++];
            return tokenRecord_;
        } else {
            tokenRecord_ = TokenRecord();
            tokenRecord_.setCode(TokenCode::tEndofStream);
        }
    }
    return tokenRecord_;
}