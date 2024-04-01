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

const string whitespace = " \t\n";
const string delimiters = ",;(){}";
const vector<string> keyWords = { "def", "return", "print", "int" };
const vector<string> operators = { "+", "-", "*", "/", "%", "=", "<", ">", 
"+=", "-=", "*=", "/=", "%=", "==", "!=", "<=", ">=", "&&", "||", "++", "--", "<<", ">>" };

bool isKeyword(const string& word) {
    return find(keyWords.begin(), keyWords.end(), word) != keyWords.end();
}

bool isDelimiter(char ch) {
    return delimiters.find(ch) != string::npos;
}

// Updated to handle string input
bool isOperator(const string& op) {
    return find(operators.begin(), operators.end(), op) != operators.end();
}

TokenType identifyTokenType(const string& token) {
    if (isKeyword(token)) return TokenType::Keyword;
    if (isOperator(token)) return TokenType::Operator; 
    if (token.length() == 1 && isDelimiter(token[0])) return TokenType::Delimiter;
    if (isdigit(token[0])) return TokenType::Literal;
    return TokenType::Identifier;
}
// Tokenize input string
vector<string> tokenize(const string& input) {
    vector<string> tokens;
    string token;
    for (size_t i = 0; i < input.length(); ++i) {
        char ch = input[i];
        if (whitespace.find(ch) != string::npos) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if (isDelimiter(ch)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(string(1, ch));
        }
        else {
            string potentialOperator(1, ch);
            if (i + 1 < input.length()) {
                potentialOperator += input[i + 1];
                // Check for operator that have 2 char
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
