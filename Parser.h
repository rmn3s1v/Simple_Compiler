#pragma once

#include "Token.h"
#include "Lexer.h"
#include "Asm.h"
#include <fstream>
#include <vector>
#include <stack>
#include <algorithm>
#include <map>

using namespace std;

class Parser
{
public:
    TokenType lexemeType;
    string lexeme;
    vector<Token> TokensForParser;
    map<string, TokenType> IdentifyTable;
    map<string, TokenType> FuncTable;
    map<string, string> ValueConst;
    stack<TokenType> CorrectnessOfTheDesignIf;
    stack<TokenType> CorrectnessOfTheDesignCycle;
    stack<TokenType> CorrectnessOfTheDesignFunc;
    stack<TokenType> CorrectnessOfTheDesignStart;
    stack<TokenType> AllCorrectness;
    int IDLexemeInVector = 0;

public:
    Parser(vector<Token> TokensForParser, map<string, TokenType> Identify, map<string, TokenType> FuncTypeGet, map<string, string> ValueConst)
    {
        this->TokensForParser = TokensForParser;
        this->IdentifyTable = Identify;
        this->FuncTable = FuncTypeGet;
        this->ValueConst = ValueConst;
    }

    TokenType Get()
    {
        this->lexeme = TokensForParser[IDLexemeInVector].lexeme;
        return TokensForParser[IDLexemeInVector++].type;
    }

    void Start()
    {
        this->lexemeType = Get();

        if (lexemeType == TokenType::STARTPROGRAMM)
        {
            CorrectnessOfTheDesignStart.push(TokenType::STARTPROGRAMM);
            this->lexemeType = Get();

            if (lexemeType == TokenType::SEMICOLON) Body();
            else
            {
                cout << "Ожидается ; перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else
        {
            cout << "Не найдена точка входа в программу. На месте: " << lexeme <<  " ожидается точка входа" << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void Body()
    {
        if (IDLexemeInVector == TokensForParser.size())
        {
            if (lexemeType != TokenType::ENDPROGRAMM)
            {
                cout << "Не найдена точка выхода из программы. На месте: " << lexeme <<  " ожидается точка выхода" << endl;
                throw "Синтаксическая ошибка";
            }
            cout << "Конец Синтаксического Анализа" << endl;
            Asm a(TokensForParser, IdentifyTable, FuncTable, ValueConst);
            a.SplitFunc();
            exit(0);
        }

        this->lexemeType = Get();

        if (lexemeType == TokenType::TYPEIDINT)
        {
            Identyfy();
            Body();
        }
        else if (lexemeType == TokenType::TYPEIBOOL)
        {
            Identyfy();
            Body();
        }
        else if (lexemeType == TokenType::TYPEISTRING)
        {
            Identyfy();
            Body();
        }
        else if (lexemeType == TokenType::IDENTIFIER)
        {
            int flagFind = 0;

            for (auto [lex, type] : IdentifyTable)
            {
                if (lex == lexeme) {flagFind = 1; break;}
            }

            if (flagFind)
            {
                Equally(lexeme);
                Body();
            }
            else
            {
                cout << "Идентификатор не определен - " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else if (lexemeType == TokenType::CYCLE)
        {
            AllCorrectness.push(lexemeType);
            CorrectnessOfTheDesignCycle.push(lexemeType);

            CheckIF();
            this->lexemeType = Get();

            if (lexemeType == TokenType::STARTCYCLE)
            {
                this->lexemeType = Get();

                if (lexemeType == TokenType::SEMICOLON)
                {
                    cout << "Ошибка! " << lexeme << " не должно быть"<< endl;
                    throw "Синтаксическая ошибка";
                }
                IDLexemeInVector--;
                Body();
            }
            else
            {
                cout << "Ожидается DO перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else if (lexemeType == TokenType::EXITCYCLE)
        {
            if (!CorrectnessOfTheDesignCycle.empty())
            {
                if (CorrectnessOfTheDesignCycle.top() != TokenType::CYCLE)
                {
                    cout << "Ожидается WHILE перед " << lexeme << endl;
                    throw "Синтаксическая ошибка";
                }
            }
            else
            {
                cout << "Ожидается WHILE перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }

            this->lexemeType = Get();

            if (lexemeType != TokenType::SEMICOLON)
            {
                cout << "Ожидается ; перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }

            Body();
        }
        else if (lexemeType == TokenType::ENDCYCLE)
        {
            if (!AllCorrectness.empty())
            {
                if (AllCorrectness.top() != TokenType::CYCLE)
                {
                    cout << "Ожидается конец условия перед " << lexeme << endl;
                    throw "Синтаксическая ошибка";
                }
                else
                {
                    AllCorrectness.pop();
                    CorrectnessOfTheDesignCycle.pop();
                }
            }
            else
            {
                cout << "Ожидается WHILE перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
            this->lexemeType = Get();

            if (lexemeType != TokenType::SEMICOLON)
            {
                cout << "Ожидается ; перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
            Body();
        }
        else if (lexemeType == TokenType::CONDITION)
        {
            AllCorrectness.push(lexemeType);
            CorrectnessOfTheDesignIf.push(lexemeType);

            CheckIF();
            this->lexemeType = Get();

            if (lexemeType == TokenType::THEN)
            {
                this->lexemeType = Get();

                if (lexemeType == TokenType::SEMICOLON)
                {
                    cout << "Ошибка! " << lexeme << " не должно быть"<< endl;
                    throw "Синтаксическая ошибка";
                }
                IDLexemeInVector--;
                Body();
            }
            else
            {
                cout << "Ожидается then перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else if (lexemeType == TokenType::ELSE)
        {
            if (!CorrectnessOfTheDesignIf.empty())
            {
                if (CorrectnessOfTheDesignIf.top() != TokenType::CONDITION)
                {
                    cout << "Ожидается if перед " << lexeme << endl;
                    throw "Синтаксическая ошибка";
                }
            }
            else
            {
                cout << "Ожидается if перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }

            Body();
        }
        else if (lexemeType == TokenType::ENDIF)
        {
            if (!AllCorrectness.empty())
            {
                if (AllCorrectness.top() != TokenType::CONDITION)
                {
                    cout << "Ожидается конец цикла перед " << lexeme << endl;
                    throw "Синтаксическая ошибка";
                }
                else
                {
                   AllCorrectness.pop();
                   CorrectnessOfTheDesignIf.pop();
                }
            }
            else
            {
                cout << "Ожидается endif перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
            this->lexemeType = Get();
            if (lexemeType != TokenType::SEMICOLON)
            {
                cout << "Ожидается ; перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
            Body();
        }
        else if (lexemeType == TokenType::STARTFUNC)
        {
            if (!AllCorrectness.empty())
            {
                cout << "Ожидается конец цикла/условия перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }

            if (!CorrectnessOfTheDesignFunc.empty())
            {
                cout << "Ожидается конец функции перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }

            CorrectnessOfTheDesignFunc.push(TokenType::STARTFUNC);

            this->lexemeType = Get();

            this->lexemeType = Get();

            if (lexemeType != TokenType::OPENS)
            {
                cout << "Ожидается ( перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }

            FuncParam();

        }
        else if (lexemeType == TokenType::RETURN)
        {
            IDLexemeInVector += 1;
            this->lexemeType = Get();
            //cout << ToString(lexemeType) << "!" << lexeme << "!" << IDLexemeInVector << endl;

            if (lexemeType != TokenType::SEMICOLON)
            {
                cout << "Ожидается ; перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
            Body();
        }
        else if (lexemeType == TokenType::ENDF)
        {
            if (CorrectnessOfTheDesignFunc.empty())
            {
                cout << "Ожидается начало функции перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }

            CorrectnessOfTheDesignFunc.pop();
            this->lexemeType = Get();
            if (lexemeType != TokenType::SEMICOLON)
            {
                cout << "Ожидается ; перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
            Body();
        }
        else if (lexemeType == TokenType::ENDPROGRAMM)
        {
            if (CorrectnessOfTheDesignStart.empty())
            {
                cout << "Не найдена точка входа в программу. На месте: " << lexeme <<  " ожидается точка входа" << endl;
                throw "Синтаксическая ошибка";
            }
            if (!AllCorrectness.empty())
            {
                cout << "Не найден конец if/while на месет " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }

            CorrectnessOfTheDesignStart.pop();
            this->lexemeType = Get();
            if (lexemeType != TokenType::POINT)
            {
                cout << "Ожидается . " << endl;
                throw "Синтаксическая ошибка";
            }
            if (IDLexemeInVector == TokensForParser.size())
            {
                cout << "Конец Синтаксического Анализа" << endl;
                Asm a(TokensForParser, IdentifyTable, FuncTable, ValueConst);
                a.SplitFunc();
                exit(0);
            }
            Start();
        }
        else if (lexemeType == TokenType::SEMICOLON)
        {
            cout << "Перед ; должно быть выражение" << endl;
            throw "Синтаксическая ошибка";
        }
        else if (lexemeType == TokenType::POINT)
        {
            cout << "Перед . должно быть END" << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void FuncParam()
    {
        this->lexemeType = Get();

        if(lexemeType == TokenType::TYPEIDINT)
        {
            FuncIdentyfy();
        }
        else if(lexemeType == TokenType::TYPEISTRING)
        {
            FuncIdentyfy();
        }
        else if(lexemeType == TokenType::TYPEIBOOL)
        {
            FuncIdentyfy();
        }
        else
        {
            cout << "Ожидается тип переменной перед " << lexeme << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void FuncIdentyfy()
    {
        this->lexemeType = Get();

        if (lexemeType == TokenType::IDENTIFIER)
        {
            this->lexemeType = Get();

            if(lexemeType == TokenType::CLOSES)
            {
                this->lexemeType = Get();

                if(lexemeType == TokenType::SEMICOLON)
                {
                    Body();
                }
                else
                {
                    cout << "Ожидается ; перед " << lexeme << endl;
                    throw "Синтаксическая ошибка";
                }
            }
            else
            {
                cout << "Ожидается ) перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else
        {
            cout << "Ожидается идентификатор перед " << lexeme << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void Identyfy()
    {
        this->lexemeType = Get();

        if (lexemeType == TokenType::IDENTIFIER)
        {

            this->lexemeType = Get();

            if(lexemeType == TokenType::SEMICOLON)
            {
                Body();
            }
            else
            {
                cout << "Ожидается ; перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else
        {
            cout << "Ожидается идентификатор перед " << lexeme << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void Equally(string lexemeBuf)
    {
        this->lexemeType = Get();

        if (lexemeType == TokenType::EQUALLY)
        {
            this->lexemeType = Get();
            if (lexemeType == TokenType::OPENS)
            {
                CheckBoolFunc();
                if(IdentifyTable[lexemeBuf] == TokenType::CONSTBOOL)
                {
                    Body();
                }
                else
                {
                    cout << "Несовпадение типов переменных " << lexemeBuf << endl;
                    throw "Синтаксическая ошибка";
                }
            }
            else
            {
                IDLexemeInVector -= 1;
                FindIdentify(lexemeBuf);
            }

        }
        else
        {
            cout << "Ожидается оператор = перед " << lexeme << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void FindIdentify(string lexemeBuf)
    {
        this->lexemeType = Get();

        if (lexemeType == TokenType::IDENTIFIER)
        {
            int flagFind = 0;

            for (auto [lex, type] : IdentifyTable)
            {
                if (lex == lexeme) {flagFind = 1; break;}
            }

            if (flagFind)
            {
                CheckType(lexemeBuf);
            }
            else
            {
                cout << "Идентификатор не определен - " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else if (lexemeType == TokenType::CONSTINT)
        {
            CheckType(lexemeBuf);
        }
        else if (lexemeType == TokenType::CONSTBOOL)
        {
            CheckType(lexemeBuf);
        }
        else if (lexemeType == TokenType::CONSTSTRING)
        {
            CheckType(lexemeBuf);
        }
        else
        {
            cout << "Ожидается выражение перед " << lexeme << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void CheckType(string lexemeBuf)
    {
        if (lexemeType == TokenType::IDENTIFIER)
        {
            if (IdentifyTable[lexeme] == IdentifyTable[lexemeBuf]) CheckSemicolon(lexemeBuf);
            else
            {
                cout << "Ошибка типов идентификаторов - " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else
        {
            if (lexemeType == IdentifyTable[lexemeBuf])
            {
                CheckSemicolon(lexemeBuf);
            }
            else
            {
                cout << "Ошибка типов идентификаторов - " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
    }

    void CheckSemicolon(string lexemeBuf)
    {
        this->lexemeType = Get();

        if (lexemeType == TokenType::SEMICOLON) Body();

        else if (lexemeType == TokenType::PLUS || lexemeType == TokenType::MINUS) FindIdentify(lexemeBuf);

        else if (lexemeType == TokenType::COPENS && IdentifyTable[lexemeBuf] == TokenType::CONSTSTRING)
        {
            this->lexemeType = Get();

            if (lexemeType == TokenType::CONSTINT || IdentifyTable[lexeme] == TokenType::CONSTINT)
            {
                this->lexemeType = Get();

                if (lexemeType == TokenType::CCLOSES)
                {
                    this->lexemeType = Get();
                    if (lexemeType == TokenType::SEMICOLON) Body();
                    else
                    {
                        cout << "Ожидается ; перед" << lexeme << endl;
                        throw "Синтаксическая ошибка";
                    }
                }
                else
                {
                    cout << "Ожидается ]" << lexeme << endl;
                    throw "Синтаксическая ошибка";
                }
            }
            else
            {
                cout << "Ожидается целое число " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else
        {
            cout << "Ожидается ; перед " << lexeme << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void CheckBoolFunc()
    {
        this->lexemeType = Get();

        if (lexemeType == TokenType::IDENTIFIER)
        {
            int flagFind = 0;
            string bufNameFunc = lexeme;

            for (auto [lex, type] : FuncTable)
            {
                if (lex == lexeme) {flagFind = 1; break;}
            }

            if (flagFind)
            {
                this->lexemeType = Get();

                if(lexemeType == TokenType::OPENS)
                {
                    this->lexemeType = Get();

                    if (IdentifyTable[lexeme] == TokenType::CONSTSTRING && FuncTable[bufNameFunc] == TokenType::TYPEISTRING)
                    {
                        Comparison(bufNameFunc);
                    }
                    else if (IdentifyTable[lexeme] == TokenType::CONSTINT && FuncTable[bufNameFunc] == TokenType::TYPEIDINT)
                    {
                        Comparison(bufNameFunc);
                    }
                    else if (IdentifyTable[lexeme] == TokenType::CONSTBOOL && FuncTable[bufNameFunc] == TokenType::TYPEIBOOL)
                    {
                        Comparison(bufNameFunc);
                    }
                    else
                    {
                        cout << ToString(IdentifyTable[lexeme]) << " ! " << ToString(FuncTable[bufNameFunc]) << endl;
                        cout << "Неверная передача типа переменной " << lexeme << endl;
                        throw "Синтаксическая ошибка";
                    }
                }
                else
                {
                    cout << "Ожидается ( перед " << lexeme << endl;
                    throw "Синтаксическая ошибка";
                }
            }
            else
            {
                cout << "Идентификатор не определен - " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else
        {
            cout << "Ожидается вызов функции перед " << lexeme << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void Comparison(string funcname)
    {
        this->lexemeType = Get();

        if (lexemeType == TokenType::CLOSES)
        {
            this->lexemeType = Get();

            if (lexemeType == TokenType::MORE || lexemeType == TokenType::LESS || lexemeType == TokenType::IFEQUALLY)
            {
                this->lexemeType = Get();

                if (lexemeType == TokenType::IDENTIFIER)
                {
                    if (IdentifyTable[funcname] == IdentifyTable[lexeme])
                    {
                        this->lexemeType = Get();

                        if (lexemeType == TokenType::CLOSES)
                        {
                            this->lexemeType = Get();

                            if (lexemeType == TokenType::SEMICOLON)
                            {
                                Body();
                            }
                            else
                            {
                                cout << "Ожидается ; перед " << lexeme << endl;
                                throw "Синтаксическая ошибка";
                            }
                        }
                        else
                        {
                            cout << "Ожидается ) перед " << lexeme << endl;
                            throw "Синтаксическая ошибка";
                        }
                    }
                    else
                    {
                        cout << "Несовпадение типов переменных " << lexeme << endl;
                        throw "Синтаксическая ошибка";
                    }
                }
                else
                {
                    if (IdentifyTable[funcname] == lexemeType)
                    {
                        this->lexemeType = Get();

                        if (lexemeType == TokenType::CLOSES)
                        {
                            this->lexemeType = Get();

                            if (lexemeType != TokenType::SEMICOLON)
                            {
                                cout << "Ожидается ; перед " << lexeme << endl;
                                throw "Синтаксическая ошибка";
                            }
                        }
                        else
                        {
                            cout << "Ожидается ) перед " << lexeme << endl;
                            throw "Синтаксическая ошибка";
                        }
                    }
                    else
                    {
                        cout << "Несовпадение типов переменных " << lexeme << endl;
                        throw "Синтаксическая ошибка";
                    }
                }
            }
            else
            {
                cout << "Ожидается оператор сравнения перед " << lexeme << endl;
                throw "Синтаксическая ошибка";
            }
        }
        else
        {
            cout << "Ожидается ) перед " << lexeme << endl;
            throw "Синтаксическая ошибка";
        }
    }

    void CheckIF()
    {
        if (!CheckExpression())
        {
            cout << "Неверное условие " << lexeme << endl;
            throw "Синтаксическая ошибка";
        }
    }

    bool CheckExpression()
    {
        this->lexemeType = Get();

        if (lexemeType == TokenType::IDENTIFIER)
        {
            string IDBufName = lexeme;

            if (IdentifyTable[lexeme] == TokenType::CONSTSTRING)
            {
                this->lexemeType = Get();

                if (lexemeType == TokenType::COPENS)
                {
                    this->lexemeType = Get();

                    if (lexemeType == TokenType::CONSTINT || IdentifyTable[lexeme] == TokenType::CONSTINT)
                    {
                        this->lexemeType = Get();

                        if (lexemeType == TokenType::CCLOSES)
                        {
                            this->lexemeType = Get();

                            if (lexemeType == TokenType::MORE || lexemeType == TokenType::LESS || lexemeType == TokenType::IFEQUALLY || lexemeType == TokenType::NOTEEQUALLY)
                            {
                                this->lexemeType = Get();

                                if (lexemeType == TokenType::IDENTIFIER)
                                {
                                    if ((IdentifyTable[IDBufName] != IdentifyTable[lexeme]) ||
                                        (IdentifyTable[IDBufName] == IdentifyTable[lexeme] && IdentifyTable[lexeme] != TokenType::CONSTSTRING))
                                    {
                                        cout << "Неверный тип идентификатора " << lexeme << endl;
                                        throw "Синтаксическая ошибка";
                                    }
                                    else
                                    {
                                        return true;
                                    }
                                }
                                else if(lexemeType == TokenType::ENDLINE)
                                {
                                    return true;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else
                            {
                                cout << "Ожидается оператор сравнения перед " << lexeme << endl;
                                throw "Синтаксическая ошибка";
                            }
                        }
                        else
                        {
                            cout << "Ожидается ] перед " << lexeme << endl;
                            throw "Синтаксическая ошибка";
                        }
                    }
                }
                else
                {
                    cout << "Ожидается [ перед " << lexeme << endl;
                    throw "Синтаксическая ошибка";
                }
            }
            else if (IdentifyTable[lexeme] == TokenType::CONSTINT || lexemeType == TokenType::CONSTINT)
            {
                this->lexemeType = Get();

                if (lexemeType == TokenType::MORE || lexemeType == TokenType::LESS || lexemeType == TokenType::IFEQUALLY || lexemeType == TokenType::NOTEEQUALLY)
                {
                    this->lexemeType = Get();

                    if (lexemeType == TokenType::IDENTIFIER && IdentifyTable[lexeme] == TokenType::CONSTINT || lexemeType == TokenType::CONSTINT)
                    {
                        return true;
                    }
                    else
                    {
                        cout << "Неверный тип идентификатора " << lexeme << endl;
                        throw "Синтаксическая ошибка";
                    }
                }
                else
                {
                    cout << "Ожидается оператор сравнения перед " << lexeme << endl;
                    throw "Синтаксическая ошибка";
                }
            }
            else if (lexemeType == TokenType::CONSTBOOL)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (lexemeType == TokenType::CONSTBOOL)
        {
            return true;
        }
        else
        {
            return false;
        }
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
};
