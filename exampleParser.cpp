#include <iostream>
#include <vector>
#include <string>

/*
C@ Grammar for L6
Stmt:= ConfigStmt | AssgStmt | PrintStmt
ConfigStmt:= config [ dec | hex | bin ]
AssgStmt:= Variable = MathExp
PrintStmt := print MathExp
MathExp := SumExp
SumExp := ProductExp [ + ProductExp | – ProductExp ]*
ProductExp := PrimaryExp [ * PrimaryExp | / PrimaryExp ]*
PrimaryExp := Int | Variable | ( MathExp )
Variable := [a-zA-z][a-zA-z0-9]*
Int := -?[0-9]+
*/


/*
Example of code in file:

config dec
print 1 + 1
print 3 + 3 * 3
print ( 3 + 3 ) * 3
x = 2 - -2
y = x
z = y * ( 16 / ( y - 2 ) )
print x
print y
print z
config hex
print z
config bin
print z
*/

/*
output from example:

2
12
18
4
4
32
0x20
0000000000100000
*/

/*
LogExp := OrExp
OrExp := AndExp [ || AndExp ]*
AndExp := CmpExp [ && CmpExp ]*
CmpExp := PrimaryExp [ == PrimaryExp | < PrimaryExp | > PrimaryExp ]*
PrimaryExp := Int | ( LogExp )
Int := -?[0-9]+
*/

// Tokenized expression
std::vector<std::string> tokens =
{ "1","&&","1","<","2","&&","0","||","1","(","1","==","1",")" };
// Current position
int position = 0;
// Reserved end-of-expression string symbol (ETX = end-of-text in UNICODE)
const std::string ETX = "\u0003";

// Return token @steps ahead
std::string peek(int steps)
{
    if (position + steps >= tokens.size()) return ETX;
    return tokens[position + steps];
}

// Return current token
std::string peek()
{
    return peek(0);
}


// Advance to the next token.
// @token is a safeguard to make sure the caller knows what is being consumed.
void consume(const std::string& token)
{
    std::string next_token = peek();
    if (next_token == ETX)
        throw std::runtime_error("Consumed past last token\n");
    if (next_token != token)
        throw std::runtime_error("Could not consume token " + token + "\n");
    ++position;
}

bool is_int(const std::string& token)
{
    // ...
}

int parse_PrimaryExp()
{
    int value;
    std::string next_token = peek();
    // Number
    if (is_int(next_token))
    {
        value = std::stoi(next_token);
        consume(next_token);
    }
    // Parenthesis expression: ( LogExp )
    else if (next_token == "(")
    {
        consume("(");
        value = parse_LogExp();
        if (peek() == ")")
            consume(")");
        else
            throw std::runtime_error("Expected: )\n");
    }
    // No valid PrimaryExp found, which is an error
    else
        throw std::runtime_error("expected int or ( )");
    return value;
}

bool parse_CmpExp()
{
    // Parse the left-hand-side block
    int result = parse_PrimaryExp();
    // Parse right-hand-side blocks
    std::string next_token = peek();
    while (1)
    {
        if (next_token == "==")
        {
            consume("==");
            result = (result == parse_PrimaryExp());
        }
        else if (next_token == "<")
        {
            consume("<");
            result = (result < parse_PrimaryExp());
        }
        else if (next_token == ">")
        {
            consume(">");
            result = (result > parse_PrimaryExp());
        }
        else
            break;
        next_token = peek();
    }
    return (bool)result;
}

bool parse_AndExp()
{
    // Parse the left-hand-side block
    bool result = parse_CmpExp();
    // Parse right-hand-side blocks
    std::string next_token = peek();
    while (1)
    {
        if (next_token == "&&")
        {
            consume("&&");
            result = result && parse_CmpExp();
        }
        else
            break;
        next_token = peek();
    }
    return result;
}

bool parse_OrExp()
{
    // Parse the left-hand-side block
    bool result = parse_AndExp();
    // Parse right-hand-side blocks
    std::string next_token = peek();
    while (1)
    {
        if (next_token == "||")
        {
            consume("||");
            result = (result || parse_AndExp());
        }
        else
            break;
        next_token = peek();
    }
    return result;
}

bool parse_LogExp()
{
    return parse_OrExp();
}

bool evaluate()
{
    return parse_LogExp();
}

int main()
{
    evaluate();
}