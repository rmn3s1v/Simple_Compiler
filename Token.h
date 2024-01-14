#pragma once

#include <iostream>
#include <string>
#include <regex>
#include <iomanip>

using namespace std;

regex regular1("([a-zA-Z]{1})(\\w)+");
regex regular2("([a-zA-Z]{1})");
cmatch result1;
cmatch result2;

// типы токенов
enum class TokenType
{
    STARTPROGRAMM,  // тип точки входа PROGRAMM
    ENDPROGRAMM,    // тип конца программы END

    TYPEIDINT,      // тип индификатор integer
    TYPEIBOOL,      // тип индификатор bool
    TYPEISTRING,    // тип индификатор string
    IDENTIFIER,     // тип идентификатора имя

    CYCLE,          // тип условия цикла WHILE
    STARTCYCLE,     // тип начала цикла DO
    ENDCYCLE,       // тип конца цикла ENDWHILE
    EXITCYCLE,      // тип выхода из цикла EXITWHILE

    CONDITION,      // тип условия if
    THEN,           // тип then
    ELSE,           // тип else
    ENDIF,          // тип endif

    STARTFUNC,      // тип начала функции FUNC
    RETURN,         // тип возвращаемого значения
    ENDF,           // тип конца функции

    SEPARATOR,      // тип разделителя пробел

    OPENS,          // тип (
    CLOSES,         // тип )
    COPENS,         // тип [
    CCLOSES,        // тип ]
    MORE,           // тип >
    LESS,           // тип <
    NOTE,           // тип !
    NOTEEQUALLY,    // тип !
    EQUALLY,        //тип =
    IFEQUALLY,      // тип ==
    PLUS,           // тип +
    MINUS,          // тип -
    POINT,          // тип .
    SEMICOLON,      // тип ;
    ENDLINE,        // тип endline

    CONSTINT,        // тип переменной int
    CONSTBOOL,       // тип переменной bool
    CONSTSTRING,     // тип переменной string

    NONE,            // тип NONE (для функций)
};

class Token
{
public:
    string lexeme;
    TokenType type;

public:
    Token(string lexeme)
    {
        this->lexeme = lexeme;

        if (TokenDefinition(lexeme) == TokenType::IDENTIFIER)
        {
            if(!INDCorrectness(lexeme)) { cout <<" | " << lexeme <<" | "; throw "Ошибка: неверно введен идентификатор";}
            else this->type = TokenDefinition(lexeme);
        }
        else
        {
            this->type = TokenDefinition(lexeme);
        }
    }

    TokenType TokenDefinition(string lexeme)
    {
        if (lexeme == "integer") return TokenType::TYPEIDINT;

        if (lexeme == "bool") return TokenType::TYPEIBOOL;

        if (lexeme == "string") return TokenType::TYPEISTRING;

        if (lexeme == "PROGRAMM") return TokenType::STARTPROGRAMM;

        if (lexeme == "END") return TokenType::ENDPROGRAMM;

        if (lexeme == "WHILE") return TokenType::CYCLE;

        if (lexeme == "DO") return TokenType::STARTCYCLE;

        if (lexeme == "EXITWHILE") return TokenType::EXITCYCLE;

        if (lexeme == "ENDWHILE") return TokenType::ENDCYCLE;

        if (lexeme == "if") return TokenType::CONDITION;

        if (lexeme == "then") return TokenType::THEN;

        if (lexeme == "else") return TokenType::ELSE;

        if (lexeme == "endif") return TokenType::ENDIF;

        if (lexeme == "FUNC") return TokenType::STARTFUNC;

        if (lexeme == "RETURN") return TokenType::RETURN;

        if (lexeme == "ENDF") return TokenType::ENDF;

        if (lexeme == "+") return TokenType::PLUS;

        if (lexeme == "-") return TokenType::MINUS;

        if (lexeme == ">") return TokenType::MORE;

        if (lexeme == "<") return TokenType::LESS;

        if (lexeme == "!=") return TokenType::NOTEEQUALLY;

        if (lexeme == "!") return TokenType::NOTE;

        if (lexeme == "=") return TokenType::EQUALLY;

        if (lexeme == "==") return TokenType::IFEQUALLY;

        if (lexeme == "(") return TokenType::OPENS;

        if (lexeme == ")") return TokenType::CLOSES;

        if (lexeme == "[") return TokenType::COPENS;

        if (lexeme == "]") return TokenType::CCLOSES;

        if (lexeme == ";") return TokenType::SEMICOLON;

        if (lexeme == ".") return TokenType::POINT;

        if (lexeme == "endline") return TokenType::ENDLINE;

        if (IsString(lexeme)) return TokenType::CONSTSTRING;

        if (IsDigit(lexeme)) return TokenType::CONSTINT;

        if (lexeme == "false") return TokenType::CONSTBOOL;

        if (lexeme == "true") return TokenType::CONSTBOOL;

        return TokenType::IDENTIFIER;
    }

    bool IsString(string lexeme)
    {
        return lexeme.front() == '"' && lexeme.back() == '"';
    }

    bool IsDigit(string lexeme)
    {
        for (int id = 0; id < lexeme.size(); id++)
        {
            if (lexeme[id] < '0' || lexeme[id] > '9') return false;

            if (id == 0 && lexeme[id] == '0' && lexeme[id+1] > '0' && lexeme[id+1] <= '9') {cout << " | " << lexeme << " | "; throw "Ожидается число!";}
        }

        return true;
    }

    bool INDCorrectness(string lexeme)
    {
        if (!regex_match(lexeme.c_str(), result1, regular1) && !regex_match(lexeme.c_str(), result2, regular2)) return false;

        return true;
    }

    string ToString(TokenType type)
    {
        if (type == TokenType::STARTPROGRAMM) return "STARTPROGRAMM";

        if (type == TokenType::ENDPROGRAMM) return "ENDPROGRAMM";

        if (type == TokenType::TYPEIDINT) return "TYPEIDINT";

        if (type == TokenType::TYPEIBOOL) return "TYPEIBOOL";

        if (type == TokenType::TYPEISTRING) return "TYPEISTRING";

        if (type == TokenType::CYCLE) return "CYCLE";

        if (type == TokenType::STARTCYCLE) return "STARTCYCLE";

        if (type == TokenType::ENDCYCLE) return "ENDCYCLE";

        if (type == TokenType::EXITCYCLE) return "EXITCYCLE";

        if (type == TokenType::CONDITION) return "CONDITION";

        if (type == TokenType::THEN) return "THEN";

        if (type == TokenType::ELSE) return "ELSE";

        if (type == TokenType::ENDIF) return "ENDIF";

        if (type == TokenType::STARTFUNC) return "STARTFUNC";

        if (type == TokenType::RETURN) return "RETURN";

        if (type == TokenType::ENDF) return "ENDF";

        if (type == TokenType::SEPARATOR) return "SEPARATOR";

        if (type == TokenType::OPENS) return "OPENS";

        if (type == TokenType::CLOSES) return "CLOSES";

        if (type == TokenType::COPENS) return "COPENS";

        if (type == TokenType::CCLOSES) return "CCLOSES";

        if (type == TokenType::MORE) return "MORE";

        if (type == TokenType::LESS) return "LESS";

        if (type == TokenType::NOTEEQUALLY) return "NOTEEQUALLY";

        if (type == TokenType::NOTE) return "NOTE";

        if (type == TokenType::EQUALLY) return "EQUALLY";

        if (type == TokenType::IFEQUALLY) return "IFEQUALLY";

        if (type == TokenType::PLUS) return "PLUS";

        if (type == TokenType::MINUS) return "MINUS";

        if (type == TokenType::POINT) return "POINT";

        if (type == TokenType::ENDLINE) return "ENDLINE";

        if (type == TokenType::SEMICOLON) return "SEMICOLON";

        if (type == TokenType::CONSTINT) return "CONSTINT";

        if (type == TokenType::CONSTBOOL) return "CONSTBOOL";

        if (type == TokenType::CONSTSTRING) return "CONSTSTRING";

        return "IDENTIFIER";
    }

    void Table(int number)
    {
        cout << std::setw(6) << number << std::setw(16) << lexeme << std::setw(22) << ToString(type) << endl;
    }

    void TableIdn(int number)
    {
        cout << std::setw(6) << number << std::setw(16) << lexeme << std::setw(22) << ToString(type) << endl;
    }
};
