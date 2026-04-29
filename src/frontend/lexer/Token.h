#pragma once
#include <string>
#include <string_view>

enum class TokenType
{
    LPAREN,             // (
    RPAREN,             // )
    LBRACE,             // {
    RBRACE,             // }
    LBRACKET,           // [
    RBRACKET,           // ]
    COMMA,              // ,
    DOT,                // .
    MINUS,              // -
    PLUS,               // +
    SEMICOLON,          // ;
    SLASH,              // /
    STAR,               // *
    PERCENT,            // %
    COLON,              // :
    CARET,              // ^
    TILDE,              // ~

    BANG,               // !
    BANG_EQUAL,         // !=
    EQUAL,              // =
    EQUAL_EQUAL,        // ==
    GREATER,            // >
    GREATER_EQUAL,      // >=
    LESS,               // <
    LESS_EQUAL,         // <=
    COLON_EQUAL,        // :=
    QUESTION_DOT,       // ?.
    QUESTION_QUESTION,  // ??
    PLUS_PLUS,          // ++
    MINUS_MINUS,        // --
    AND_AND,            // &&
    OR_OR,              // ||
    BIT_AND,            // &
    BIT_OR,             // |
    LEFT_SHIFT,         // <<
    RIGHT_SHIFT,        // >>
    AND_NOT,            // &^
    ELLIPSIS,           // ...

    IDENTIFIER,         // имя переменной / функции
    STRING,             // "строка" или `сырая строка`
    INT_LIT,            // 123
    FLOAT_LIT,          // 123.456

    PACKAGE,            // package
    IMPORT,             // import
    FUNC,               // func
    STRUCT,             // struct
    VAR,                // var
    CONST,              // const
    IF,                 // if
    ELSE,               // else
    FOR,                // for
    RANGE,              // range
    BREAK,              // break
    CONTINUE,           // continue
    SWITCH,             // switch
    CASE,               // case
    DEFAULT,            // default
    MATCH,              // match
    RETURN,             // return
    DEFER,              // defer
    TRUE,               // true
    FALSE,              // false
    NIL,                // nil

    EOF_TOKEN,          // конец входного потока
    ERROR               // ошибка лексера
};

struct Token {
	TokenType type;
	std::string lexeme;
	int line;
};