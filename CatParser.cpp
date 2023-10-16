#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

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
public:
    Interpreter(std::ostream& out_stream): position(0), tokens(), output_stream(out_stream), ETX("\u0003"){} //constructor

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

    // Evaluate & interpret one tokenized statement
    void evaluate(const std::vector<std::string>& tokens) //call for every line
    {
        std::cout << "Evaluating" << std::endl;
    }

private:
    const std::vector<std::string> tokens;
    int position;
    const std::string ETX = "\u0003"; // Reserved end-of-expression string symbol (ETX = end-of-text in UNICODE)
    std::ostream& output_stream;
};










int main()
{
    Interpreter catParser();
    
    std::string token;
    std::fstream codeFile;
    std::vector<std::string> tokens;
    codeFile.open("catCode.txt", std::ios::in);
    if (codeFile.is_open()) 
    {
        std::string line;
        while (getline(codeFile, line)) // itterate all lines in file
        {
            std::stringstream ss(line);
            while (ss >> token) //stores token based on whitespace
            {
                tokens.push_back(token); //store a line of tokens
            }
            
            //parse line to interpretor here and run evaluate

            tokens.clear(); //clear tokens for next line
        }
        codeFile.close();
    }
    return 0;
}

// ToDo:
// - split code into lines - DONE
// - split code into tokens - DONE
// - Call Evaluate for every line of code