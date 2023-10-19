#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <math.h>
/*
C@ Grammar for L6
Stmt:= ConfigStmt | AssgStmt | PrintStmt
ConfigStmt:= config [ dec | hex | bin ] - DONE
AssgStmt:= Variable = MathExp
PrintStmt := print MathExp - DONE
MathExp := SumExp - DONE
SumExp := ProductExp [ + ProductExp | – ProductExp ]* - DONE
ProductExp := PrimaryExp [ * PrimaryExp | / PrimaryExp ]* - DONE
PrimaryExp := Int | Variable | ( MathExp )
Variable := [a-zA-z][a-zA-z0-9]* DONE
Int := -?[0-9]+ DONE
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
    Interpreter(std::ostream& out_stream): position(0), tokens(), output_stream(out_stream), ETX("\u0003"){} // constructor

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

    int parsePrimaryExp() //debug this method
    {
        //Kolla ifall int eller variabel eller mathexp i paranteser
        int value;
        std::string next_token = peek();
        // Number
            if (is_int(next_token))
            {
                value = std::stoi(next_token);
                consume(next_token);
            }
            else if (is_variable(next_token)) {
                value = std::stoi(next_token);
                consume(next_token);
            }
            else if (next_token == "(")
            {
                consume("(");
                value = parseMathExp();
                if (peek() == ")")
                    consume(")");
                else
                    throw std::runtime_error("Expected: )\n");
            }
            // No valid PrimaryExp found, which is an error
            else 
            {
                throw std::runtime_error("expected int or ( )");
            }
        return value;
    }

    bool is_int(const std::string& token)
    {
        // Check if the string is empty or starts with a '-' sign.
        if (token.empty()) {
            return false;
        }

        // Skip the '-' sign if it's present.
        size_t start = 0;
        if (token[0] == '-') {
            start = 1;
        }

        for (size_t i = start; i < token.size(); i++) {
            if (!std::isdigit(token[i])) {
                return false;
            }
        }

        return true;
    }

    bool is_variable(const std::string& token) {
        std::regex pattern("^[a-zA-Z_][a-zA-Z0-9_]*$");

        return std::regex_match(token, pattern);
    }

    bool parseProductExp() {
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
        return (bool)result;
    }

    bool parseSumExp() {

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
        return (bool)result;
    }

    bool parseMathExp() {
        return parseSumExp();
    }

    bool parsePrintStmt() {
        return parseMathExp();
    }

    bool parseConfigStmt() {
        std::string next_token = peek();
        bool result = false;
        while (1)
        {
            if (next_token == "dec")
            {
                consume("dec");
                result = true;
                
            }
            else if (next_token == "hex")
            {
                consume("hex");
                result = true;
                
            }
            else if (next_token == "bin")
            {
                consume("bin");
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
            result = parseMathExp();
        }
        return result;
    }
    //insert all logic parse methods here!

    bool parseStmt()
    {
        bool result = false;
        std::string next_token = peek();

        while (next_token != ETX) { // check if token is of: ConfigStmt | AssgStmt | PrintStmt
            if (next_token == "print") {
                consume("print");
                result = parsePrintStmt();
            }
            else if (next_token == "config") {
                consume("config");
                result = parseConfigStmt();
            }
            else if (is_variable(next_token)) {
                consume(next_token);
                result = parseAssgStmt();
            }
            else {
                break;
            }

            next_token = peek();
        }
        if (result) {
            std::cout << "success!";
        }
        else {
            std::cout << "Fail!!";
        }
        return (bool)result;
    }

    // Evaluate & interpret one tokenized statement
    void evaluate(const std::vector<std::string>& tokens) // call for every line
    {
        this->position = 0;
        this->tokens = tokens;
        parseStmt();

        /*
        output_stream << "Evaluating" << std::endl;
        for (int i = 0; i < tokens.size(); i++)
        {
            output_stream << "'" << tokens.at(i) << "' ";
        }
        output_stream << std::endl;
        */
    }

private:
    std::vector<std::string> tokens;
    int position;
    const std::string ETX = "\u0003"; // Reserved end-of-expression string symbol (ETX = end-of-text in UNICODE)
    std::ostream& output_stream;
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

// ToDo:
// - split code into lines - DONE
// - split code into tokens - DONE
// - Call Evaluate for every line of code - DONE
// - implement Parse logic for all statements - TODO
// - store variable values in hashmap - TODO
// - get correct output from parsed code - TODO


//Tips:

// - The <regex> header provides functionality for regular expression matching –
// - see e.g.std::regex and std::regex_match.
// - Regular expressions can also be matched by checking each char of the string
// - manually.The isdigit(char) and isalpha(char) functions may be of
// - help here.
// - Stream std::hex to an ostream(using << ) to have it print integers in
// - hexadecimal base.Note that this only works for positive integers.
// - Stream std::showbase to include a base prefix, e.g. 0x.
// - Convert an int to a binary representation and then to string :
// - std::bitset<32>(int).to_string()