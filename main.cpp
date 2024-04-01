#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>

using namespace std;
enum class TokenType {
    Keyword,
    Identifier,
    Operator,
    Delimiter,
    Literal,
    Unknown
};

map<TokenType, string> tokenTypeNames = {
    {TokenType::Keyword, "Keywords"},
    {TokenType::Identifier, "Identifiers"},
    {TokenType::Operator, "Operators"},
    {TokenType::Delimiter, "Delimiters"},
    {TokenType::Literal, "Literals"},
    {TokenType::Unknown, "Unknown"}
};

const string whitespace = " \t\n";
const string delimiters = ",;(){}\"";
const vector<string> keyWords = { "def", "return", "print", "int", "cout", "endl","std","using", "#include","iostream"};
const vector<string> operators = { "+", "-", "*", "/", "%", "=", "<", ">",
                                   "+=", "-=", "*=", "/=", "%=", "==", "!=", "<=", ">=",
                                   "&&", "||", "++", "--", "<<", ">>" };

bool isKeyword(const string& word) {
    return find(keyWords.begin(), keyWords.end(), word) != keyWords.end();
}

bool isDelimiter(char ch) {
    return delimiters.find(ch) != string::npos;
}

bool isOperator(const string& op) {
    return find(operators.begin(), operators.end(), op) != operators.end();
}

TokenType identifyTokenType(const string& token) {
    if (isKeyword(token)) return TokenType::Keyword;
    if (isOperator(token)) return TokenType::Operator;
    if (token.length() == 1 && isDelimiter(token[0]) && token[0] != '\"') return TokenType::Delimiter;
    if (isdigit(token[0]) || (token.length() > 1 && token[0] == '\"' && token[token.length() - 1] == '\"')) return TokenType::Literal;
    return TokenType::Identifier;
}

// Tokenize input string
vector<string> tokenize(const string& input) {
    vector<string> tokens;
    string token;
    bool inStringLiteral = false; // Flag to indicate we're inside a string literal

    for (size_t i = 0; i < input.length(); ++i) {
        char ch = input[i];

        if (inStringLiteral) {
            token += ch; // Add character to token
            // Check for end of string literal, considering escape character to allow " inside literals
            if (ch == '"' && (i == 0 || input[i - 1] != '\\')) {
                inStringLiteral = false;
                tokens.push_back(token); // Add the complete string literal as a token
                token.clear();
            }
        }
        else {
            if (whitespace.find(ch) != string::npos) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            }
            else if (isDelimiter(ch) && ch != '"') { // Check for delimiters that are not double quotes
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                tokens.push_back(string(1, ch));
            }
            else if (ch == '"') { // Start of string literal
                if (!token.empty()) {
                    tokens.push_back(token); // Add token before the string literal starts
                    token.clear();
                }
                token += ch;
                inStringLiteral = true; // Indicate that we're now inside a string literal
            }
            else {
                string potentialOperator(1, ch);
                if (i + 1 < input.length()) {
                    potentialOperator += input[i + 1];
                    if (isOperator(potentialOperator)) {
                        if (!token.empty()) {
                            tokens.push_back(token);
                            token.clear();
                        }
                        tokens.push_back(potentialOperator);
                        ++i;
                        continue;
                    }
                }
                if (isOperator(string(1, ch))) {
                    if (!token.empty()) {
                        tokens.push_back(token);
                        token.clear();
                    }
                    tokens.push_back(string(1, ch));
                }
                else {
                    token += ch;
                }
            }
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

// Remove comments and excessive whitespace from a line
string cleanLine(const string& line) {
    size_t commentPos = line.find("//");
    if (commentPos != string::npos) {
        // If there's a comment, only take the substring before it
        return line.substr(0, commentPos);
    }
    return line; // Return the line directly if there's no comment
}

void printCleanedFile(const string& filename) {
    ifstream inFile(filename);
    string line;

    if (!inFile.is_open()) {
        cout << "Unable to open file to print cleaned content." << endl;
        return;
    }

    cout << "Cleaned File Content:" << endl;
    while (getline(inFile, line)) {
        string cleaned = cleanLine(line); // Remove comments first

        // Now remove all whitespace from the cleaned line
        cleaned.erase(remove_if(cleaned.begin(), cleaned.end(), ::isspace), cleaned.end());

        // Print the cleaned line only if it's not empty
        if (!cleaned.empty()) {
            cout << cleaned << endl;
        }
    }

   
    inFile.close();
}



int main() {
    printCleanedFile("file.txt");
    ifstream inFile("file.txt");
    map<TokenType, set<string>> tokenMap; // Use set to avoid duplicates

    string line;
    if (!inFile.is_open()) {
        cout << "Unable to open input file." << endl;
        return 1;
    }

    while (getline(inFile, line)) {
        string cleanedLine = cleanLine(line);
        if (!cleanedLine.empty()) {
            vector<string> tokens = tokenize(cleanedLine);
            for (const string& token : tokens) {
                TokenType type = identifyTokenType(token);
                tokenMap[type].insert(token); // Insert token into the appropriate category
            }
        }
    }

    inFile.close();

    // Print token table, properly categorized and without duplicates
    cout << "Category\t\tTokens\n";
    cout << "------------------------------------------------\n";
    for (const auto& pair : tokenMap) {
        cout << tokenTypeNames[pair.first] << "\t\t";
        auto& tokens = pair.second;
        for (auto it = tokens.begin(); it != tokens.end(); ++it) {
            cout << *it;
            if (next(it) != tokens.end()) cout << ", ";
        }
        cout << "\n";
    }

    return 0;
}
