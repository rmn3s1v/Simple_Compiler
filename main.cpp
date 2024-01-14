#include <iostream>
#include "Lexer.h"
#include "Parser.h"


int main()
{
    try
    {
        Lexer lex;
        lex.SplitToToken();
        lex.MergeToken();
        lex.Table();
        lex.TableValue();
        lex.FuncOut();
        Parser pars(lex.AllTokens, lex.Identify, lex.FuncTypeGet, lex.ValueConst);
        pars.Start();
    }
    catch(const char*mssg)
    {
        cout << mssg << endl;
    }

}
