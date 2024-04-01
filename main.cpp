#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

// Token categories
enum class TokenType {
    Keyword,
    Identifier,
    Operator,
    Delimiter,
    Literal,
    Unknown
};

// Mapping from TokenType to a human-readable string for printing
map<TokenType, string> tokenTypeNames = {
    {TokenType::Keyword, "Keywords"},
    {TokenType::Identifier, "Identifiers"},
    {TokenType::Operator, "Operators"},
    {TokenType::Delimiter, "Delimiters"},
    {TokenType::Literal, "Literals"},
    {TokenType::Unknown, "Unknown"}
};

// Separator, keyword, and operator configurations
const string whitespace = " \t\n";
const string delimiters = ",;(){}";
const vector<string> keyWords = { "def", "return", "print", "int"};
const string operators = "+-*/%=";

// Check if a string is a keyword
bool isKeyword(const string& word) {
    return find(keyWords.begin(), keyWords.end(), word) != keyWords.end();
}

// Check if a character is a delimiter
bool isDelimiter(char ch) {
    return delimiters.find(ch) != string::npos;
}

// Check if a character is an operator
bool isOperator(char ch) {
    return operators.find(ch) != string::npos;
}

// Identify the token type
TokenType identifyTokenType(const string& token) {
    if (isKeyword(token)) return TokenType::Keyword;
    if (token.length() == 1 && isOperator(token[0])) return TokenType::Operator;
    if (token.length() == 1 && isDelimiter(token[0])) return TokenType::Delimiter;
    if (isdigit(token[0])) return TokenType::Literal;
    return TokenType::Identifier; // Everything else that is not a single char operator/delimiter or digit is an identifier
}

// Tokenize input string
vector<string> tokenize(const string& input) {
    vector<string> tokens;
    string token;
    for (char ch : input) {
        if (whitespace.find(ch) != string::npos) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if (isDelimiter(ch) || isOperator(ch)) {
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
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

// Remove comments and excessive whitespace from a line
string cleanLine(const string& line) {
    stringstream result;
    bool inComment = false;
    for (char ch : line) {
        if (ch == '#') inComment = true;
        if (!inComment) result << ch;
    }
    return result.str();
}

int main() {
    ifstream inFile("file.txt");
    ifstream cleanFile("file.txt");
    ofstream outFile("cleaned_code.txt");
    map<TokenType, set<string>> tokenMap; // Use set to avoid duplicates

    string line;
    if (!inFile.is_open()) {
        cout << "Unable to open input file." << endl;
        return 1;
    }

    while (getline(cleanFile, line)) {
        // Check if the line starts with " ", "#", or "//"
        if (!line.empty() && (line[0] == ' ' || line[0] == '#' || line.substr(0, 2) == "//")) {
            continue;
        }

        cout << line << "\n"; // Print the line
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
    outFile.close();

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
