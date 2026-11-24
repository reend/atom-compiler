#include "lexer.hpp"

Lexer::Lexer(std::string_view source)
    : source_(source) {}

bool Lexer::atEnd() const 
{
    return pos_ >= source_.size();
}

char Lexer::peek() const 
{
    return atEnd() ? '\0' : source_[pos_];
}

char Lexer::advance() 
{
    if (atEnd()) 
    {
        return '\0';
    }

    char c = source_[pos_++];

    if (c == '\0') 
    {
        ++line_;
        column_ = 1;
    } 
    else 
    {
        ++column_;
    }

    return c;
}

bool Lexer::match(char expected) 
{
    if (atEnd() || peek() != expected) 
    {
        return false;
    }

    advance();
    return true;
}

void Lexer::skipWhitespace() 
{
    while (!atEnd()) 
    {
        switch (peek()) 
        {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                advance();
                break;
            case '/':
                if (pos_ + 1 >= source_.size() ||
                    source_[pos_ + 1] != '/') 
                {
                        return;
                }

                while (!atEnd() && peek() != '\n') 
                {
                    advance();
                }
                break;
            default:
                break;
        }
    }
}