#pragma once

#include "Token.h"
#include "Lexer.h"
#include <fstream>
#include <vector>
#include <stack>
#include <algorithm>
#include <map>

using namespace std;

class Asm
{
public:
    TokenType lexemeType;
    string lexeme;
    vector<Token> TokensForAsm;
    vector<Token> TokensFunc;
    vector<Token> TokensMain;
    map<string, TokenType> IdentifyTable;
    map<string, TokenType> FuncTable;
    map<string, int> TablePTR;
    map<string, string> ValueConst;
    int idPTR = 0;
    int idLexeme = 0;
    string metka = ".L";
    int idMetka = 1;
    string code = "\n\n";

public:
    Asm(vector<Token> TokensForAsm, map<string, TokenType> Identify, map<string, TokenType> FuncTypeGet, map<string, string> ValueConst)
    {
        this->TokensForAsm = TokensForAsm;
        this->IdentifyTable = Identify;
        this->FuncTable = FuncTypeGet;
        this->ValueConst = ValueConst;
    }

    TokenType Get()
    {
        this->lexeme = TokensForAsm[idLexeme].lexeme;
        return TokensForAsm[idLexeme].type;
    }

    void SplitFunc()
    {
        // разделяем на функции и main
        while(idLexeme < TokensForAsm.size())
        {
            this->lexemeType = Get();
            if (lexemeType == TokenType::STARTFUNC)
            {
                TokensFunc.push_back(TokensForAsm[idLexeme]);
                idLexeme++;
                while (lexemeType != TokenType::ENDF)
                {
                    this->lexemeType = Get();
                    TokensFunc.push_back(TokensForAsm[idLexeme]);
                    idLexeme++;
                }
                this->lexemeType = Get();
                TokensFunc.push_back(TokensForAsm[idLexeme]);
                idLexeme++;
            }
            else
            {
                TokensMain.push_back(TokensForAsm[idLexeme]);
                idLexeme++;
            }
        }

        idLexeme = 0;

        for (auto [id, type] : IdentifyTable)
        {
            if (type == TokenType::CONSTINT)
            {
                idPTR += 4;
                TablePTR[id] = idPTR;
            }
            if (type == TokenType::CONSTSTRING)
            {
                idPTR += 6;
                TablePTR[id] = idPTR;
            }
            if (type == TokenType::CONSTBOOL)
            {
                idPTR += 1;
                TablePTR[id] = idPTR;
            }
        }
        // for (auto [id, ptr] : TablePTR)
        // {
        //     cout << id << "|" << ptr << endl;
        // }
        // for (int ip = 0; ip < TokensMain.size(); ip++)
        // {
        //     cout << TokensMain[ip].lexeme << endl;
        // }
        CreateCode();
    }

    void CreateCode()
    {
        string bufExpression[6];
        TokenType bufType[6];
        string bufExpression1[6];
        TokenType bufType1[6];
        string bufIfExpression[6];
        TokenType bufIfType[6];
        string bufMetka1;
        string bufMetka2;
        string bufMetka3;
        string bufIfMetka1;
        //string bufIfMetka2;
        int idExp = 0;
        int idTyp = 0;
        int idIfExp = 0;
        int idIfTyp = 0;
        int flagExit = 0;
        int flagExitF = 0;
        for (int i = 0; i < TokensFunc.size(); i++)
        {
            if (TokensFunc[i].type == TokenType::STARTFUNC)
            {
                code += TokensFunc[i+1].lexeme + TokensFunc[i+2].lexeme + TokensFunc[i+3].lexeme + TokensFunc[i+5].lexeme + ":\n";
                code += "push   rbp\n";
                code += "mov    pbr, rsp\n";
                code += "mov    QWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+1].lexeme]) + "], rdi\n";
                i += 5;
                continue;
            }
            else if (TokensFunc[i].type == TokenType::IDENTIFIER && TokensFunc[i+1].type == TokenType::EQUALLY &&
                     (TokensFunc[i+2].type == TokenType::CONSTINT || TokensFunc[i+2].type == TokenType::CONSTSTRING ||TokensFunc[i+2].type == TokenType::CONSTBOOL) &&
                     TokensFunc[i+3].type == TokenType::SEMICOLON)
            {
                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], " + TokensFunc[i+2].lexeme + "\n";
                i += 2;
                continue;
            }
            else if (TokensFunc[i].type == TokenType::IDENTIFIER && TokensFunc[i+1].type == TokenType::EQUALLY &&
                    (TokensFunc[i+2].type == TokenType::CONSTINT || TokensFunc[i+2].type == TokenType::CONSTSTRING || TokensFunc[i+2].type == TokenType::CONSTBOOL ||
                     TokensFunc[i+2].type == TokenType::IDENTIFIER) && (TokensFunc[i+3].type == TokenType::PLUS || TokensFunc[i+3].type == TokenType::MINUS) &&
                    (TokensFunc[i+4].type == TokenType::CONSTINT || TokensFunc[i+4].type == TokenType::CONSTSTRING || TokensFunc[i+4].type == TokenType::CONSTBOOL ||
                     TokensFunc[i+4].type == TokenType::IDENTIFIER) && TokensFunc[i+5].type == TokenType::SEMICOLON)
            {
                if (TokensFunc[i+2].type == TokenType::IDENTIFIER)
                {
                    if (TokensFunc[i+2].lexeme == TokensFunc[i].lexeme)
                    {
                        if (TokensFunc[i+4].type == TokenType::IDENTIFIER)
                        {
                            if (TokensFunc[i+4].lexeme == TokensFunc[i].lexeme)
                            {
                                if (TokensFunc[i+3].type == TokenType::PLUS) code += "sal    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "]\n";
                                else code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], 0\n";
                            }
                            else
                            {
                                if (TokensFunc[i+3].type == TokenType::PLUS)
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+4].lexeme]) + "]\n";
                                    code += "add    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                                }
                                else
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+4].lexeme]) + "]\n";
                                    code += "sub    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                                }
                            }
                        }
                        else
                        {
                            if (TokensFunc[i+3].type == TokenType::PLUS) code += "add    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], " + TokensFunc[i+4].lexeme + "\n";
                            else code += "sub    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], " + TokensFunc[i+4].lexeme + "\n";
                        }
                    }
                    else
                    {
                        if (TokensFunc[i+4].type == TokenType::IDENTIFIER)
                        {
                            if (TokensFunc[i+4].lexeme == TokensFunc[i+2].lexeme)
                            {
                                if (TokensFunc[i+3].type == TokenType::PLUS)
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+2].lexeme]) + "]\n";
                                    code += "add    eax, eax\n";
                                    code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                                }
                                else code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], 0\n";
                            }
                            else
                            {
                                if (TokensFunc[i+3].type == TokenType::PLUS)
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+2].lexeme]) + "]\n";
                                    code += "add    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+4].lexeme]) + "]\n";
                                    code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                                }
                                else
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+2].lexeme]) + "]\n";
                                    code += "sub    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+4].lexeme]) + "]\n";
                                    code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                                }
                            }
                        }
                        else
                        {
                            if (TokensFunc[i+3].type == TokenType::PLUS)
                            {
                                code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+2].lexeme]) + "]\n";
                                code += "add    eax, " + TokensFunc[i+4].lexeme + "\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                            }
                            else
                            {
                                code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+2].lexeme]) + "]\n";
                                code += "sub    eax, " + TokensFunc[i+4].lexeme + "\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                            }
                        }
                    }
                }
                else
                {
                    if (TokensFunc[i+4].type == TokenType::IDENTIFIER)
                    {
                        if (TokensFunc[i+4].lexeme == TokensFunc[i].lexeme)
                        {
                            if (TokensFunc[i+3].type == TokenType::PLUS) code += "add    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], " + TokensFunc[i+2].lexeme + "\n";
                            else
                            {
                                code += "mov    eax, " + TokensFunc[i+2].lexeme + "\n";
                                code += "sub    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "]\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                            }
                        }
                        else
                        {
                            if (TokensFunc[i+3].type == TokenType::PLUS)
                            {
                                code += "mov    eax, " + TokensFunc[i+2].lexeme + "\n";
                                code += "add    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+4].lexeme]) + "]\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                            }
                            else
                            {
                                code += "mov    eax, " + TokensFunc[i+2].lexeme + "\n";
                                code += "sub    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+4].lexeme]) + "]\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], eax\n";
                            }
                        }
                    }
                    else
                    {
                        if (TokensFunc[i+3].type == TokenType::PLUS)
                        {
                            code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], " + to_string(stoi(TokensFunc[i+2].lexeme) + stoi(TokensFunc[i+4].lexeme)) + "\n";
                        }
                        else
                        {
                            code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i].lexeme]) + "], " + to_string(stoi(TokensFunc[i+2].lexeme) - stoi(TokensFunc[i+4].lexeme))+ "\n";
                        }
                    }
                }
                i += 5;
                continue;
            }
            else if (TokensFunc[i].type == TokenType::CYCLE)
            {
                bufMetka1 = metka + to_string(idMetka++);
                code += "jmp    " + bufMetka1 + "\n";
                bufMetka2 = metka + to_string(idMetka++);
                code += bufMetka2 + ":\n";
                i++;

                while (TokensFunc[i].type != TokenType::STARTCYCLE)
                {
                    bufExpression[idExp++] = TokensFunc[i].lexeme;
                    bufType[idTyp++] = TokensFunc[i++].type;
                }
                continue;
            }
            else if (TokensFunc[i].type == TokenType::ENDCYCLE)
            {
                code += bufMetka1 + ":\n";
                if (bufType[0] == TokenType::CONSTBOOL) code += "jmp    " + bufMetka2 + "\n";
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::MORE && bufType[2] == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "], DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[2]]) +"]\n";
                    code += "jg     " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::LESS && bufType[2] == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "], DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[2]]) +"]\n";
                    code += "jle    " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::NOTEEQUALLY && bufType[2] == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "], DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[2]]) +"]\n";
                    code += "jne    " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::IFEQUALLY && bufType[2] == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "], DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[2]]) +"]\n";
                    code += "je    " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::MORE && bufType[2] == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "], " + bufExpression[2] + "\n";
                    code += "jg     " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::LESS && bufType[2] == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "], " + bufExpression[2] + "\n";
                    code += "jle    " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::NOTEEQUALLY && bufType[2] == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "], " + bufExpression[2] + "\n";
                    code += "jne    " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::IFEQUALLY && bufType[2] == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "], " + bufExpression[2] + "\n";
                    code += "je     " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::COPENS && bufType[4] == TokenType::MORE)
                {
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[2]]) + "]\n";
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "+eax]\n";
                    code += "cmp    eax, 0dh\n";
                    code += "jg     " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::COPENS && bufType[4] == TokenType::LESS)
                {
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[2]]) + "]\n";
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "+eax]\n";
                    code += "cmp    eax, 0dh\n";
                    code += "jle    " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::COPENS && bufType[4] == TokenType::NOTEEQUALLY)
                {
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[2]]) + "]\n";
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "+eax]\n";
                    code += "cmp    eax, 0dh\n";
                    code += "jne    " + bufMetka2+ "\n";
                }
                else if (bufType[0] == TokenType::IDENTIFIER && bufType[1] == TokenType::COPENS && bufType[4] == TokenType::IFEQUALLY)
                {
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[2]]) + "]\n";
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression[0]]) + "+eax]\n";
                    code += "cmp    eax, 0dh\n";
                    code += "je     " + bufMetka2 + "\n";
                }

                if (flagExitF)
                {
                    code += bufMetka3 + ":\n";
                }
            }
            else if (TokensFunc[i].type == TokenType::RETURN)
            {
                if (TokensFunc[i+1].type == TokenType::IDENTIFIER)
                {
                    code += "mov    eax, DWORD PTR [rbp-"+to_string(TablePTR[TokensFunc[i+1].lexeme]) + "]\n";
                    code += "pop    rbp\nret\n";
                }
                else
                {
                    code += "mov    eax, " + TokensFunc[i+1].lexeme +"\n";
                    code += "pop    rbp\nret\n";
                }
                i++;
            }
            else if (TokensFunc[i].type == TokenType::CONDITION)
            {
                bufIfMetka1 = metka + to_string(idMetka++);
                if (TokensFunc[i+1].type == TokenType::IDENTIFIER && TokensFunc[i+2].type == TokenType::MORE && TokensFunc[i+3].type == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+1].lexeme]) + "], DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+3].lexeme]) +"]\n";
                    code += "jg     " + bufIfMetka1+ "\n";
                }
                else if (TokensFunc[i+1].type == TokenType::IDENTIFIER && TokensFunc[i+2].type == TokenType::LESS && TokensFunc[i+3].type == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+1].lexeme]) + "], DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+3].lexeme]) +"]\n";
                    code += "jle    " + bufIfMetka1+ "\n";
                }
                else if (TokensFunc[i+1].type == TokenType::IDENTIFIER && TokensFunc[i+2].type == TokenType::NOTEEQUALLY && TokensFunc[i+3].type == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+1].lexeme]) + "], DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+3].lexeme]) +"]\n";
                    code += "jne    " + bufIfMetka1+ "\n";
                }
                else if (TokensFunc[i+1].type == TokenType::IDENTIFIER && TokensFunc[i+2].type == TokenType::IFEQUALLY && TokensFunc[i+3].type == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+1].lexeme]) + "], DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+3].lexeme]) +"]\n";
                    code += "je    " + bufIfMetka1+ "\n";
                }
                else if (TokensFunc[i+1].type == TokenType::IDENTIFIER && TokensFunc[i+2].type == TokenType::MORE && TokensFunc[i+3].type == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+1].lexeme]) + "], " + TokensFunc[i+3].lexeme + "\n";
                    code += "jg     " + bufIfMetka1+ "\n";
                }
                else if (TokensFunc[i+1].type == TokenType::IDENTIFIER && TokensFunc[i+2].type == TokenType::LESS && TokensFunc[i+3].type == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+1].lexeme]) + "], " + TokensFunc[i+3].lexeme + "\n";
                    code += "jle    " + bufIfMetka1+ "\n";
                }
                else if (TokensFunc[i+1].type == TokenType::IDENTIFIER && TokensFunc[i+2].type == TokenType::NOTEEQUALLY && TokensFunc[i+3].type == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+1].lexeme]) + "], " + TokensFunc[i+3].lexeme + "\n";
                    code += "jne    " + bufIfMetka1+ "\n";
                }
                else if (TokensFunc[i+1].type == TokenType::IDENTIFIER && TokensFunc[i+2].type == TokenType::IFEQUALLY && TokensFunc[i+3].type == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensFunc[i+1].lexeme]) + "], " + TokensFunc[i+3].lexeme + "\n";
                    code += "je     " + bufIfMetka1+ "\n";
                }
                i+=3;
            }
            else if (TokensFunc[i].type == TokenType::ELSE)
            {
                code += bufIfMetka1 + ":\n";
            }
            else if (TokensFunc[i].type == TokenType::EXITCYCLE)
            {
                bufMetka3 = metka + to_string(idMetka++);
                flagExitF = 1;
                code += "jmp    " + bufMetka3 + "\n";
            }
        }
        idTyp = 0;
        idExp = 0;
        for (int i = 0; i < TokensMain.size(); i++)
        {
            if (TokensMain[i].type == TokenType::STARTPROGRAMM) code += "\nmain:\n";
            else if (TokensMain[i].type == TokenType::IDENTIFIER && TokensMain[i+1].type == TokenType::EQUALLY &&
                     (TokensMain[i+2].type == TokenType::CONSTINT || TokensMain[i+2].type == TokenType::CONSTSTRING ||TokensMain[i+2].type == TokenType::CONSTBOOL) &&
                     TokensMain[i+3].type == TokenType::SEMICOLON)
            {
                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], " + TokensMain[i+2].lexeme + "\n";
                i += 2;
                continue;
            }
            else if (TokensMain[i].type == TokenType::IDENTIFIER && TokensMain[i+1].type == TokenType::EQUALLY &&
                    (TokensMain[i+2].type == TokenType::CONSTINT || TokensMain[i+2].type == TokenType::CONSTSTRING || TokensMain[i+2].type == TokenType::CONSTBOOL ||
                     TokensMain[i+2].type == TokenType::IDENTIFIER) && (TokensMain[i+3].type == TokenType::PLUS || TokensMain[i+3].type == TokenType::MINUS) &&
                    (TokensMain[i+4].type == TokenType::CONSTINT || TokensMain[i+4].type == TokenType::CONSTSTRING || TokensMain[i+4].type == TokenType::CONSTBOOL ||
                     TokensMain[i+4].type == TokenType::IDENTIFIER) && TokensMain[i+5].type == TokenType::SEMICOLON)
            {
                if (TokensMain[i+2].type == TokenType::IDENTIFIER)
                {
                    if (TokensMain[i+2].lexeme == TokensMain[i].lexeme)
                    {
                        if (TokensMain[i+4].type == TokenType::IDENTIFIER)
                        {
                            if (TokensMain[i+4].lexeme == TokensMain[i].lexeme)
                            {
                                if (TokensMain[i+3].type == TokenType::PLUS) code += "sal    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "]\n";
                                else code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], 0\n";
                            }
                            else
                            {
                                if (TokensMain[i+3].type == TokenType::PLUS)
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+4].lexeme]) + "]\n";
                                    code += "add    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                                }
                                else
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+4].lexeme]) + "]\n";
                                    code += "sub    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                                }
                            }
                        }
                        else
                        {
                            if (TokensMain[i+3].type == TokenType::PLUS) code += "add    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], " + TokensMain[i+4].lexeme + "\n";
                            else code += "sub    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], " + TokensMain[i+4].lexeme + "\n";
                        }
                    }
                    else
                    {
                        if (TokensMain[i+4].type == TokenType::IDENTIFIER)
                        {
                            if (TokensMain[i+4].lexeme == TokensMain[i+2].lexeme)
                            {
                                if (TokensMain[i+3].type == TokenType::PLUS)
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+2].lexeme]) + "]\n";
                                    code += "add    eax, eax\n";
                                    code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                                }
                                else code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], 0\n";
                            }
                            else
                            {
                                if (TokensMain[i+3].type == TokenType::PLUS)
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+2].lexeme]) + "]\n";
                                    code += "add    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+4].lexeme]) + "]\n";
                                    code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                                }
                                else
                                {
                                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+2].lexeme]) + "]\n";
                                    code += "sub    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+4].lexeme]) + "]\n";
                                    code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                                }
                            }
                        }
                        else
                        {
                            if (TokensMain[i+3].type == TokenType::PLUS)
                            {
                                code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+2].lexeme]) + "]\n";
                                code += "add    eax, " + TokensMain[i+4].lexeme + "\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                            }
                            else
                            {
                                code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+2].lexeme]) + "]\n";
                                code += "sub    eax, " + TokensMain[i+4].lexeme + "\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                            }
                        }
                    }
                }
                else
                {
                    if (TokensMain[i+4].type == TokenType::IDENTIFIER)
                    {
                        if (TokensMain[i+4].lexeme == TokensMain[i].lexeme)
                        {
                            if (TokensMain[i+3].type == TokenType::PLUS) code += "add    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], " + TokensMain[i+2].lexeme + "\n";
                            else
                            {
                                code += "mov    eax, " + TokensMain[i+2].lexeme + "\n";
                                code += "sub    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "]\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                            }
                        }
                        else
                        {
                            if (TokensMain[i+3].type == TokenType::PLUS)
                            {
                                code += "mov    eax, " + TokensMain[i+2].lexeme + "\n";
                                code += "add    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+4].lexeme]) + "]\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                            }
                            else
                            {
                                code += "mov    eax, " + TokensMain[i+2].lexeme + "\n";
                                code += "sub    eax, DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+4].lexeme]) + "]\n";
                                code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], eax\n";
                            }
                        }
                    }
                    else
                    {
                        if (TokensMain[i+3].type == TokenType::PLUS)
                        {
                            code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], " + to_string(stoi(TokensMain[i+2].lexeme) + stoi(TokensMain[i+4].lexeme)) + "\n";
                        }
                        else
                        {
                            code += "mov    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i].lexeme]) + "], " + to_string(stoi(TokensMain[i+2].lexeme) - stoi(TokensMain[i+4].lexeme))+ "\n";
                        }
                    }
                }
                i += 5;
                continue;
            }
             else if (TokensMain[i].type == TokenType::IDENTIFIER && TokensMain[i+1].type == TokenType::EQUALLY && TokensMain[i+2].type == TokenType::OPENS)
            {
                code += "call   " + TokensMain[i+3].lexeme +"\n";
                if (TokensMain[i+7].type == TokenType::MORE)
                {
                    code += "cmp    eax, DWORD PTR [rbp-"+to_string(TablePTR[TokensMain[i+8].lexeme])+"]\n";
                    code += "setl   al\n";
                    code += "mov    DWORD PTR [rbp-"+to_string(TablePTR[TokensMain[i].lexeme])+"], al\n";
                }
                else if (TokensMain[i+7].type == TokenType::LESS)
                {
                    code += "cmp    eax, DWORD PTR [rbp-"+to_string(TablePTR[TokensMain[i+8].lexeme])+"]\n";
                    code += "setg   al\n";
                    code += "mov    DWORD PTR [rbp-"+to_string(TablePTR[TokensMain[i].lexeme])+"], al\n";
                }
                else if (TokensMain[i+7].type == TokenType::NOTEEQUALLY)
                {
                    code += "cmp    eax, DWORD PTR [rbp-"+to_string(TablePTR[TokensMain[i+8].lexeme])+"]\n";
                    code += "setne  al\n";
                    code += "mov    DWORD PTR [rbp-"+to_string(TablePTR[TokensMain[i].lexeme])+"], al\n";
                }
                else if (TokensMain[i+7].type == TokenType::IFEQUALLY)
                {
                    code += "cmp    eax, DWORD PTR [rbp-"+to_string(TablePTR[TokensMain[i+8].lexeme])+"]\n";
                    code += "sete   al\n";
                    code += "mov    DWORD PTR [rbp-"+to_string(TablePTR[TokensMain[i].lexeme])+"], al\n";
                }
                i+=9;
            }
            else if (TokensMain[i].type == TokenType::CYCLE)
            {
                bufMetka1 = metka + to_string(idMetka++);
                code += "jmp    " + bufMetka1 + "\n";
                bufMetka2 = metka + to_string(idMetka++);
                code += bufMetka2 + ":\n";
                i++;

                while (TokensMain[i].type != TokenType::STARTCYCLE)
                {
                    bufExpression1[idExp++] = TokensMain[i].lexeme;
                    bufType1[idTyp++] = TokensMain[i++].type;
                }
                continue;
            }
            else if (TokensMain[i].type == TokenType::ENDCYCLE)
            {
                code += bufMetka1 + ":\n";
                if (bufType1[0] == TokenType::CONSTBOOL) code += "jmp    " + bufMetka2 + "\n";
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::MORE && bufType1[2] == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "], DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[2]]) +"]\n";
                    code += "jg     " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::LESS && bufType1[2] == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "], DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[2]]) +"]\n";
                    code += "jle    " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::NOTEEQUALLY && bufType1[2] == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "], DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[2]]) +"]\n";
                    code += "jne    " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::IFEQUALLY && bufType1[2] == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "], DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[2]]) +"]\n";
                    code += "je    " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::MORE && bufType1[2] == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "], " + bufExpression1[2] + "\n";
                    code += "jg     " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::LESS && bufType1[2] == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "], " + bufExpression1[2] + "\n";
                    code += "jle    " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::NOTEEQUALLY && bufType1[2] == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "], " + bufExpression1[2] + "\n";
                    code += "jne    " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::IFEQUALLY && bufType1[2] == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "], " + bufExpression1[2] + "\n";
                    code += "je     " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::COPENS && bufType1[4] == TokenType::MORE)
                {
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[2]]) + "]\n";
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "+eax]\n";
                    code += "cmp    eax, 0dh\n";
                    code += "jg     " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::COPENS && bufType1[4] == TokenType::LESS)
                {
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[2]]) + "]\n";
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "+eax]\n";
                    code += "cmp    eax, 0dh\n";
                    code += "jle    " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::COPENS && bufType1[4] == TokenType::NOTEEQUALLY)
                {
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[2]]) + "]\n";
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "+eax]\n";
                    code += "cmp    eax, 0dh\n";
                    code += "jne    " + bufMetka2+ "\n";
                }
                else if (bufType1[0] == TokenType::IDENTIFIER && bufType1[1] == TokenType::COPENS && bufType1[4] == TokenType::IFEQUALLY)
                {
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[2]]) + "]\n";
                    code += "mov    eax, DWORD PTR [rbp-" + to_string(TablePTR[bufExpression1[0]]) + "+eax]\n";
                    code += "cmp    eax, 0dh\n";
                    code += "je     " + bufMetka2 + "\n";
                }
                if (flagExit)
                {
                    code += bufMetka3 +":\n";
                }
            }
            else if (TokensMain[i].type == TokenType::CONDITION)
            {
                bufIfMetka1 = metka + to_string(idMetka++);
                if (TokensMain[i+1].type == TokenType::IDENTIFIER && TokensMain[i+2].type == TokenType::MORE && TokensMain[i+3].type == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+1].lexeme]) + "], DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+3].lexeme]) +"]\n";
                    code += "jg     " + bufIfMetka1+ "\n";
                }
                else if (TokensMain[i+1].type == TokenType::IDENTIFIER && TokensMain[i+2].type == TokenType::LESS && TokensMain[i+3].type == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+1].lexeme]) + "], DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+3].lexeme]) +"]\n";
                    code += "jle    " + bufIfMetka1+ "\n";
                }
                else if (TokensMain[i+1].type == TokenType::IDENTIFIER && TokensMain[i+2].type == TokenType::NOTEEQUALLY && TokensMain[i+3].type == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+1].lexeme]) + "], DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+3].lexeme]) +"]\n";
                    code += "jne    " + bufIfMetka1+ "\n";
                }
                else if (TokensMain[i+1].type == TokenType::IDENTIFIER && TokensMain[i+2].type == TokenType::IFEQUALLY && TokensMain[i+3].type == TokenType::IDENTIFIER)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+1].lexeme]) + "], DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+3].lexeme]) +"]\n";
                    code += "je    " + bufIfMetka1+ "\n";
                }
                else if (TokensMain[i+1].type == TokenType::IDENTIFIER && TokensMain[i+2].type == TokenType::MORE && TokensMain[i+3].type == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+1].lexeme]) + "], " + TokensMain[i+3].lexeme + "\n";
                    code += "jg     " + bufIfMetka1+ "\n";
                }
                else if (TokensMain[i+1].type == TokenType::IDENTIFIER && TokensMain[i+2].type == TokenType::LESS && TokensMain[i+3].type == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+1].lexeme]) + "], " + TokensMain[i+3].lexeme + "\n";
                    code += "jle    " + bufIfMetka1+ "\n";
                }
                else if (TokensMain[i+1].type == TokenType::IDENTIFIER && TokensMain[i+2].type == TokenType::NOTEEQUALLY && TokensMain[i+3].type == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+1].lexeme]) + "], " + TokensMain[i+3].lexeme + "\n";
                    code += "jne    " + bufIfMetka1+ "\n";
                }
                else if (TokensMain[i+1].type == TokenType::IDENTIFIER && TokensMain[i+2].type == TokenType::IFEQUALLY && TokensMain[i+3].type == TokenType::CONSTINT)
                {
                    code += "cmp    DWORD PTR [rbp-" + to_string(TablePTR[TokensMain[i+1].lexeme]) + "], " + TokensMain[i+3].lexeme + "\n";
                    code += "je     " + bufIfMetka1+ "\n";
                }
                i+=3;
            }
            else if (TokensMain[i].type == TokenType::ELSE)
            {
                code += bufIfMetka1 + ":\n";
            }
            else if (TokensMain[i].type == TokenType::EXITCYCLE)
            {
                bufMetka3 = metka + to_string(idMetka++);
                flagExit = 1;
                code += "jmp    " + bufMetka3 + "\n";
            }
        }

        cout << code;
    }
};
