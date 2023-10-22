#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <unordered_map>
#include<bitset>
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

class Interpreter
{
public:
    Interpreter(std::ostream& out_stream): position(0), tokens(), output_stream(out_stream), ETX("\u0003"){} // constructor
    enum printModes
    {
        dec = 1,
        hex = 2,
        bin = 3
    };
    void print() const
    {
        switch (printmode) {
        case dec:
            output_stream << symbolTable.at(tempSymbol) << std::endl;
            break;
        case hex:
            output_stream << std::hex << "0x" << symbolTable.at(tempSymbol) << std::endl;
            break;
        case bin:
            output_stream << std::bitset<16>(symbolTable.at(tempSymbol)).to_string() << std::endl;
            break;
        default:
            output_stream << symbolTable.at(tempSymbol) << std::endl;
        }
    }
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
        bool test = true;
        // Check if the string is empty or starts with a '-' sign.
        if (token.empty()) {
            test = false;
        }
        // Skip the '-' sign if it's present.
        size_t start = 0;
        if (token[0] == '-') {
            start = 1;
        }
        for (size_t i = start; i < token.size(); i++) {
            if (!std::isdigit(token[i])) {
                test = false;
            }
        }
        return test;
    }
    bool is_variable(const std::string& token) {
        std::regex pattern("^[a-zA-Z_][a-zA-Z0-9_]*$");

        return std::regex_match(token, pattern);
    }
    int parsePrimaryExp()
    {
        int value = 0;
        std::string next_token = peek();
        if (is_int(next_token))
        {
            value = std::stoi(next_token);
            consume(next_token);
        }
        else if (is_variable(next_token)) {
            
            value = symbolTable.at(next_token);
            consume(next_token);
        }
        else if (next_token == "(")
        {
            consume("(");
            value = parseMathExp();
            if (peek() == ")")
            {
                consume(")");
            }
            else
            {
                throw std::runtime_error("Expected: )\n");
            }            
        }
        // No valid PrimaryExp found, which is an error
        else 
        {
            throw std::runtime_error("expected int or ( )");
        }
        return value;
    }
    int parseProductExp() {
        int result = parsePrimaryExp();
        std::string next_token = peek();
        while (1)
        {
            if (next_token == "*")
            {
                consume("*");
                result = (result * parsePrimaryExp());
            }
            else if (next_token == "/")
            {
                consume("/");
                result = (result / parsePrimaryExp());
            }
            else
            {
                break;
            }
            next_token = peek();
        }
        return result;
    }
    int parseSumExp() {
        int result = parseProductExp();
        std::string next_token = peek();
        while (1)
        {
            if (next_token == "+")
            {
                consume("+");
                result = (result + parseProductExp());
            }
            else if (next_token == "-")
            {
                consume("-");
                result = (result - parseProductExp());
            }
            else
            {
                break;
            }
            next_token = peek();
        }
        return result;
    }
    int parseMathExp() 
    {
        int result = parseSumExp();
        if (result == 0)
        {
            symbolTable[tempSymbol] = result;
            return (bool)!result;
        }
        if ((bool)result)
        {
            symbolTable[tempSymbol] = result;
        }
        return result;
    }
    bool parsePrintStmt() 
    {
        return (bool)parseMathExp();
    }
    bool parseConfigStmt() 
    {
        std::string next_token = peek();
        bool result = false;
        while (1)
        {
            if (next_token == "dec")
            {
                consume("dec");
                printmode = dec;
                result = true;
                
            }
            else if (next_token == "hex")
            {
                consume("hex");
                printmode = hex;
                result = true;
            }
            else if (next_token == "bin")
            {
                consume("bin");
                printmode = bin;
                result = true;
            }
            else {
                break;
            }
            next_token = peek();
        }
        return result;
   }
    bool parseAssgStmt()
    {
        std::string next_token = peek();
        bool result = false;
        if (next_token == "=")
        {
            consume("=");
            result = (bool)parseMathExp();
        }
        return result;
    }
    bool parseStmt()
    {
        bool result = false;
        std::string next_token = peek();
        while (next_token != ETX) { // check if token is of: ConfigStmt | AssgStmt | PrintStmt
            tempSymbol = "";
            if (next_token == "print") {
                consume("print");
                result = parsePrintStmt();
                if (result)
                {
                    print();
                }
            }
            else if (next_token == "config") {
                consume("config");
                result = parseConfigStmt();
            }
            else if (is_variable(next_token)) {
                tempSymbol = next_token;
                consume(next_token);
                result = parseAssgStmt();
            }
            else {
                break;
            }
            next_token = peek();
        }
        return (bool)result;
    }
    // Evaluate & interpret one tokenized statement
    void evaluate(const std::vector<std::string>& tokens) // call for every line
    {
        this->position = 0;
        this->tokens = tokens;
        parseStmt();
    }
private:
    std::vector<std::string> tokens;
    int position;
    const std::string ETX = "\u0003"; // Reserved end-of-expression string symbol (ETX = end-of-text in UNICODE)
    std::ostream& output_stream;
    std::unordered_map<std::string, int> symbolTable;
    std::string tempSymbol;
    printModes printmode = dec;
};
int main()
{   
    Interpreter catParser(std::cout);
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
            while (ss >> token) // store tokens based on whitespace
            {
                tokens.push_back(token); // store a line of tokens
            }
            catParser.evaluate(tokens); //parse tokens for every line
            tokens.clear(); // clear tokens for next line
        }
        codeFile.close();
    }
    return 0;
}