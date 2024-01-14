#pragma once

#include "Token.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

class Lexer
{
public:
    string code;
    string bufFuncName;
    vector<Token> AllTokens;
    vector<string> IdentifyString;
    map<string, TokenType> Identify;
    map<string, TokenType> FuncTypeGet;
    map<string, string> ValueConst;
    string **IdentifyValue = new string*;
    int idA = 0;

public:
    Lexer()
    {
        ifstream file;
        file.open("Programm.txt", ios::binary);

        if (!file.is_open()) throw "Ошибка открытия файла с программой";

        size_t size = file.seekg(0, ios::end).tellg();
        file.seekg(0);

        code.resize(size);
        file.read(&code[0], size);
    }

    void SplitToToken()
    {
        string BufLexem;

        for (char symbol: code)
        {
            if (IsSeparator(symbol))
            {
                if (!BufLexem.empty())
                {
                    Token newToken(BufLexem);
                    AllTokens.push_back(newToken);
                }

                if (symbol != ' ' && symbol != '\n' && symbol != '\t')
                {
                    string bufNewToke(1, symbol);
                    Token newToken(bufNewToke);
                    AllTokens.push_back(newToken);
                }

                BufLexem.clear();
            }

            else
            {
                BufLexem += symbol;
            }
        }
    }

    void MergeToken()
    {
        vector<Token> bufTokens;

        for (int id = 0; id < AllTokens.size(); id++)
        {
            if (id < AllTokens.size() - 1 && AllTokens[id].type == TokenType::NOTE && AllTokens[id+1].type == TokenType::EQUALLY)
            {
                string bufLexeme = AllTokens[id].lexeme + AllTokens[id+1].lexeme;
                Token newToken(bufLexeme);
                bufTokens.push_back(newToken);
                id ++;
                continue;
            }

            if (id < AllTokens.size() - 1 && AllTokens[id].type == TokenType::EQUALLY && AllTokens[id+1].type == TokenType::EQUALLY)
            {
                string bufLexeme = AllTokens[id].lexeme + AllTokens[id+1].lexeme;
                Token newToken(bufLexeme);
                bufTokens.push_back(newToken);
                id ++;
                continue;
            }

            bufTokens.push_back(AllTokens[id]);
        }

        AllTokens = bufTokens;
        bufTokens.clear();
    }

    bool IsSeparator(char symbol)
    {
        return symbol == ' ' || symbol == ';' || symbol == '=' || symbol == '-' || symbol == '+' || symbol == '\n' || symbol == '(' ||
               symbol == ')' || symbol == '[' || symbol == ']' || symbol == '!' || symbol == '>' || symbol == '<' || symbol == '.' || symbol == '\t';
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

        if (type == TokenType::NONE) return "NONE";

        return "IDENTIFIER";
    }

    void Table()
    {
        cout << std::setw(25) << "~~TABLE OF TOKENS~~\n" << endl;
        std::cout << std::setw(6) << "#     " << std::setw(16) << std::left << "TOKEN" << std::setw(22) << "TYPE\n" << endl;
        int id = 0;
        for (int i = 0; i < AllTokens.size(); i++)
        {
            AllTokens[i].Table(id++);
            if (AllTokens[i].type == TokenType::TYPEIDINT && AllTokens[i+1].type == TokenType::IDENTIFIER)
            {
                if (!std::count(IdentifyString.begin(), IdentifyString.end(), AllTokens[i+1].lexeme))
                {
                    IdentifyString.push_back(AllTokens[i+1].lexeme);
                    Identify[AllTokens[i+1].lexeme] = TokenType::CONSTINT;
                    i++;
                    AllTokens[i].Table(id++);
                    continue;
                }
                else
                {
                    cout << "Данный идентификатор уже определен - " << AllTokens[i+1].lexeme << endl;
                    throw "Лексическая ошибка";
                }
            }

            if (AllTokens[i].type == TokenType::TYPEIBOOL && AllTokens[i+1].type == TokenType::IDENTIFIER)
            {
                if (!std::count(IdentifyString.begin(), IdentifyString.end(), AllTokens[i+1].lexeme))
                {
                    IdentifyString.push_back(AllTokens[i+1].lexeme);
                    Identify[AllTokens[i+1].lexeme] = TokenType::CONSTBOOL;
                    i++;
                    AllTokens[i].Table(id++);
                    continue;
                }
                else
                {
                    cout << "Данный идентификатор уже определен - " << AllTokens[i+1].lexeme << endl;
                    throw "Лексическая ошибка";
                }
            }

            if (AllTokens[i].type == TokenType::TYPEISTRING && AllTokens[i+1].type == TokenType::IDENTIFIER)
            {
                if (!std::count(IdentifyString.begin(), IdentifyString.end(), AllTokens[i+1].lexeme))
                {
                    IdentifyString.push_back(AllTokens[i+1].lexeme);
                    Identify[AllTokens[i+1].lexeme] = TokenType::CONSTSTRING;
                    i++;
                    AllTokens[i].Table(id++);
                    continue;
                }
                else
                {
                    cout << "Данный идентификатор уже определен - " << AllTokens[i+1].lexeme << endl;
                    throw "Лексическая ошибка";
                }
            }

            if (AllTokens[i].type == TokenType::IDENTIFIER && !std::count(IdentifyString.begin(), IdentifyString.end(), AllTokens[i].lexeme))
            {
                cout << "Данный идентификатор не определен - " << AllTokens[i].lexeme << endl;
                throw "Лексическая ошибка";
            }

            if (AllTokens[i].type == TokenType::STARTFUNC && AllTokens[i+1].type == TokenType::IDENTIFIER)
            {
                if (!std::count(IdentifyString.begin(), IdentifyString.end(), AllTokens[i+1].lexeme))
                {
                    bufFuncName = AllTokens[i+1].lexeme;
                    FuncTypeGet[AllTokens[i+1].lexeme] = AllTokens[i+3].type;
                    IdentifyString.push_back(AllTokens[i+1].lexeme);
                    Identify[AllTokens[i+1].lexeme] = TokenType::NONE;
                    i++;
                    AllTokens[i].Table(id++);
                    continue;
                }
                else
                {
                    cout << "Данный идентификатор уже определен - " << AllTokens[i].lexeme << endl;
                    throw "Лексическая ошибка";
                }
            }

            if (AllTokens[i].type == TokenType::RETURN)
            {
                if (!std::count(IdentifyString.begin(), IdentifyString.end(), AllTokens[i+1].lexeme) && AllTokens[i+1].type != TokenType::CONSTBOOL &&
                    AllTokens[i+1].type != TokenType::CONSTINT && AllTokens[i+1].type != TokenType::CONSTSTRING)
                {
                    cout << "Данный идентификатор не определен - " << AllTokens[i+1].lexeme << endl;
                    throw "Лексическая ошибка";
                }
                if (!bufFuncName.empty())
                {
                    if (AllTokens[i+1].type == TokenType::IDENTIFIER) Identify[bufFuncName] = Identify[AllTokens[i+1].lexeme];
                    else if (AllTokens[i+1].type == TokenType::CONSTINT) Identify[bufFuncName] = TokenType::CONSTINT;
                    else if (AllTokens[i+1].type == TokenType::CONSTSTRING) Identify[bufFuncName] = TokenType::CONSTSTRING;
                    else if (AllTokens[i+1].type == TokenType::CONSTBOOL) Identify[bufFuncName] = TokenType::CONSTBOOL;
                    else
                    {
                        cout << "Неверно передаваемое значение в RETURN " << ToString(AllTokens[i+1].type) << endl;
                        throw "Лексическая ошибка";
                    }
                }
                else
                {
                    cout << "Функция не была определена" << endl;
                    throw "Лексическая ошибка";
                }
                bufFuncName.clear();
            }

            if ((AllTokens[i].type == TokenType::TYPEISTRING || AllTokens[i].type == TokenType::TYPEIBOOL || AllTokens[i].type == TokenType::TYPEIDINT) && !(AllTokens[i+1].type == TokenType::IDENTIFIER))
            {
                cout << " | " << AllTokens[i+1].lexeme << " | ";
                throw "Ожидается идентификатор! Лексическая ошибка!";
            }
        }

        id = 0;

        cout << "\n\n" << endl;
        cout << "       ~~TABLE OF ID~~\n" << endl;
        std::cout << std::setw(6) << "#     " << std::setw(16) << std::left << "ID" << std::setw(22) << "TYPE\n" << endl;
        for (auto [lex, type] : Identify) cout << std::setw(6) << id++ << std::setw(16) << std::left << lex << std::setw(22) << ToString(type) << endl;
    }

    void TableValue()
    {
        int id = 0;
        for (int i = 0; i < AllTokens.size(); i++)
        {
            int bufI = 0;

            if (AllTokens[i].type == TokenType::IDENTIFIER && AllTokens[i+1].type == TokenType::EQUALLY)
            {
                bufI = i;
                i += 2;
                if (i < AllTokens.size())
                {
                    string expression;
                    while (!(AllTokens[i].type == TokenType::SEMICOLON) && !(AllTokens[i].type == TokenType::ENDIF))
                    {
                        if (i >= AllTokens.size())
                        {
                            cout << " | " << AllTokens[bufI].lexeme << " | ";
                            throw "Ожидается ;\n Лексическая ошибка!";
                        }

                        if (AllTokens[i].type == TokenType::IDENTIFIER || AllTokens[i].type == TokenType::CONSTINT || AllTokens[i].type == TokenType::CONSTBOOL || AllTokens[i].type == TokenType::LESS ||
                            AllTokens[i].type == TokenType::CONSTSTRING || AllTokens[i].type == TokenType::PLUS || AllTokens[i].type == TokenType::MINUS || AllTokens[i].type == TokenType::MORE ||
                            AllTokens[i].type == TokenType::OPENS || AllTokens[i].type == TokenType::CLOSES || AllTokens[i].type == TokenType::COPENS || AllTokens[i].type == TokenType::CCLOSES)
                            expression += AllTokens[i].lexeme;
                        if ((AllTokens[i].type == TokenType::CONSTSTRING || AllTokens[i].type == TokenType::CONSTINT || AllTokens[i].type == TokenType::CONSTBOOL) &&
                            AllTokens[i+1].type == TokenType::SEMICOLON && AllTokens[i-1].type == TokenType::EQUALLY)
                            {
                                ValueConst[AllTokens[bufI].lexeme] = AllTokens[i].lexeme;
                            }
                        i++;
                    }

                    IdentifyValue[id] = new string[2];
                    IdentifyValue[id][0] = AllTokens[bufI].lexeme;
                    IdentifyValue[id][1] = expression;
                    id++;
                }
                else
                {
                    cout << " | " << AllTokens[i].lexeme << " | ";
                    throw "Ожидается выражение \n Лексическая ошибка!";
                }
            }
        }

        idA = id;
        cout << "\n\n" << endl;
        cout << "     ~~TABLE OF VALUE~~\n" << endl;
        std::cout << std::setw(6) << "#     " << std::setw(16) << std::left << "ID" << std::setw(22) << "VALUE\n" << endl;
        for (int i = 0; i < id; i++) cout << std::setw(6) << i << std::setw(16) << std::left << IdentifyValue[i][0] << std::setw(22) << IdentifyValue[i][1] << endl;
        //for (auto [key, value]: ValueConst) cout << key << "|" << value << endl;
    }

    void FuncOut()
    {
        cout << "\n\n     ~~TABLE OF FUNC TYPE~~\n" << endl;
        std::cout << std::setw(6) << "#     " << std::setw(16) << std::left << "ID" << std::setw(22) << "TYPE\n" << endl;
        int i = 0;
        for (auto [lex, type] : FuncTypeGet) cout << std::setw(6) << i++ << std::setw(16) << std::left << lex << std::setw(22) << ToString(FuncTypeGet[lex]) << endl;
        cout << "\n\n" << endl;
    }
};
