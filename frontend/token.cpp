#include "token.hpp"

std::string_view tokenKindToString(TokenKind kind)
{
    switch (kind)
    {
        case TokenKind::Fn:             return "Fn";
        case TokenKind::Mut:            return "Mut";
        case TokenKind::Const:          return "Const";
        case TokenKind::If:             return "If";
        case TokenKind::Else:           return "Else";
        case TokenKind::While:          return "While";
        case TokenKind::Return:         return "Return";

        case TokenKind::I64:            return "I64";
        case TokenKind::Bool:           return "Bool";
        case TokenKind::True:           return "True";
        case TokenKind::False:          return "False";

        case TokenKind::Identifier:     return "Identifier";
        case TokenKind::Integer:        return "Integer";

        case TokenKind::Plus:           return "Plus";
        case TokenKind::Minus:          return "Minus";
        case TokenKind::Star:           return "Star";

        case TokenKind::Equal:          return "Equal";
        case TokenKind::NotEqual:       return "NotEqual";
        case TokenKind::Less:           return "Less";
        case TokenKind::Greater:        return "Greater";
        case TokenKind::LessEqual:      return "LessEqual";
        case TokenKind::GreaterEqual:   return "GreaterEqual";

        case TokenKind::Assign:         return "Assign";
        case TokenKind::LParen:         return "LParen";
        case TokenKind::RParen:         return "RParen";
        case TokenKind::LBrace:         return "LBrace";
        case TokenKind::RBrace:         return "RBrace";
        case TokenKind::Comma:          return "Comma";
        case TokenKind::Semicolon:      return "Semicolon";
        case TokenKind::Colon:          return "Colon";
        case TokenKind::Arrow:          return "Arrow";

        case TokenKind::Eof:            return "Eof";
    }

    return "Unknown";
}