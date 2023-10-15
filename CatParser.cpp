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

class Interpreter
{
private:
    const std::vector<std::string> tokens;
public:
    Interpreter(std::ostream& out_stream)
    {
        
    }

    // Evaluate & interpret one tokenized statement
    void evaluate(const std::vector<std::string>& tokens) //call for every line
    {
        //start parsing here
    }
};

// Tokenized expression
std::vector<std::string> tokens =
{}; //tokens here
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

int main()
{
    Interpreter CatParser();
}