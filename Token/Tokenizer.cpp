//
// Created by luca eaton and derek zang on 8/14/25.
//

#include "Tokenizer.h"
#include <iostream>
#include <fstream>
#include <map>
#include <utility>
#include <vector>
#include <sstream>
#include <string>


std::string toLower(const std::string& s);


//Special Character Dataset
static std::vector<char> SP = { '!', '.', '_', ',', '&', '@' };

//Suffix Dataset
static std::vector<std::string> suffixes = {
    "s", "es", "ed", "ing", "ly", "er", "or", "ion", "tion", "ation", "ition",
    "ible", "able", "al", "ial", "y", "ness", "ity", "ment", "ic", "ous", "eous", "ious",
    "en", "ify", "ise", "ize", "ward", "wise", "hood", "ship", "dom", "ful", "less"
};

//Prefix Dataset
static std::vector<std::string> prefixes = {
    "un", "re", "in", "im", "ir", "il", "dis", "en", "em",
    "non", "over", "mis", "sub", "pre", "inter", "fore", "de",
    "trans", "super", "semi", "anti", "mid", "under", "over",
};

/**
 * toLower Function
 *
 * @param str provided word
 * @return the word in all lower case
 */
std::string Tokenizer::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

/**
 * Character tokenization method
 *
 * @param s Word/Sentence
 *
 * Given a string/sentence, creates a vector of characters on the given string
 *
 * @return A vector of chars on the provided word/sentence
 */
std::vector<char> Tokenizer::CTE (const std::string &s) {
    std::vector<char> v(s.begin(),s.end());
    return v;
}
/**
 * Word tokenization method
 *
 * @param s Word/Sentence
 * @param sp Special char case
 *
 * Given a string/sentence, creates a vector of "words" on the given string/sentence
 *
 * @return a vector of strings, primarily the "words"
 */
std::vector<std::string> Tokenizer::WTE(const std::string& s, std::vector<char> sp) {
    std::vector<std::string> v;
    std::string pt;
    for (char c  : s) {
        if (std::find(sp.begin(), sp.end(), c) != sp.end()) {
            v.push_back(pt);
            pt.clear();
        }
        if (c == ' ') {
            if (!pt.empty()) {
                v.push_back(pt);
                pt.clear();
            }
        } else {
            pt += c;
        }
    }
    if (!pt.empty()) {
        v.push_back(pt);
    }
    return v;
}
/**
 * Substring tokenization method
 *
 * @param s Word/Sentence
 * @param sp Special char cases
 * @param pre Prefix cases
 * @param suff Suffix cases
 *
 * Given a string/sentence, creates a vector of "words" including prefixes and suffixes on the given string/sentence
 *
 * @return vector of substrings
 */
std::vector<std::string> Tokenizer::STE(const std::string& s,
                             const std::vector<char>& sp,
                             const std::vector<std::string> &pre,
                             const std::vector<std::string> &suff)
{
    std::vector<std::string> vP = WTE(s, sp);
    std::vector<std::string> brk;
    for (const auto& tgt : vP) {
        const std::string lower = toLower(tgt);
        if (tgt.size() <= 4) {
            brk.push_back(tgt);
            continue;
        }
        bool matched = false;
        std::string currWordPre, currWordSuff;
        //For Derek -- Checks for Prefix
        auto preIt = std::find_if(pre.begin(), pre.end(), [&](const std::string& p){
            return lower.rfind(p, 0) == 0;
        });
        if (preIt != pre.end()) {
            currWordPre = *preIt;
            matched = true;
        }
        //For Derek -- Checks for Suffix
        auto suffIt = std::find_if(suff.begin(), suff.end(), [&](const std::string& x){
            if (x.size() > lower.size()) return false;
            return lower.compare(lower.size() - x.size(), x.size(), x) == 0;
        });
        if (suffIt != suff.end()) {
            currWordSuff = *suffIt;
            matched = true;
        }
        //For Derek -- Puts the pieces together
        if (matched) {
            if (!currWordPre.empty()) brk.push_back(currWordPre + "##");
            std::size_t start = currWordPre.size();
            std::size_t endTrim = currWordSuff.size();
            std::size_t midLen = tgt.size() - start - endTrim;
            if (midLen > 0) brk.push_back(tgt.substr(start, midLen));
            if (!currWordSuff.empty()) brk.push_back("##" + currWordSuff);
        } else {
            brk.push_back(tgt);
        }
    }
    return brk;
}
/**
 *
 * @param tokenVector Our tokenized sentence/words
 * @param in Provided dictionary of tokens/token ids/token embeddings
 *
 *Read dictionary file
 *Take returned vector, usually from our STE() method
 *Convert provided words into tokens the tokenIDS
 *
 * @return tokenIDs : The token IDs of each provided token.
 */
std::vector<int> Tokenizer::encodeTokens(std::vector<std::string> tokenVector, const std::string& in) {
    std::vector<std::string> tokens = std::move(tokenVector);
    std::unordered_map<std::string, int> tMap;
    std::ifstream inFile(in);
    if (!inFile.is_open()) {
        std::cerr << "Could not open file: " << std::endl;
        return {};
    }

    std::string line;
    while (std::getline(inFile, line)) {
        size_t pos = line.find(':');
        size_t vPos = line.find("<TAB>");
        std::string currentTokenID = line.substr(0, pos);
        std::string currentToken = line.substr(pos+1, vPos - pos - 1);
        tMap[toLower(currentToken)] = std::stoi(currentTokenID);
    }

    std::vector<int> tokenIDs;
    for(const auto& token : tokens) {
        if (tMap.count(toLower(token))) {
            tokenIDs.push_back(tMap[toLower(token)]);
        }
    }
    return tokenIDs;
}
/**
 *
 * @param tID A vector of token IDs
 * @param in Provided dictionary of tokens/token ids/token embeddings
 *
 * Reads our dictionary
 * Takes the vector of Token IDs
 * Converts IDs into tokens/words
 *
 * @return A decoded vector of tokens from the provided token vector
 */
std::vector<std::string> Tokenizer::decodeTokens(std::vector<int> tID, const std::string& in) {
    std::vector<int> tokenIDs= std::move(tID);
    std::unordered_map<int, std::string> tMap;
    std::ifstream inFile(in);
    if (!inFile.is_open()) {
        std::cerr << "Could not open file: " << std::endl;
        return{};
    }

    std::string line;
    while (std::getline(inFile, line)) {
        size_t pos = line.find(':');
        size_t vPos = line.find("<TAB>");
        std::string currentTokenID = line.substr(0, pos);
        std::string currentToken = line.substr(pos+1, vPos - pos - 1);
        tMap[std::stoi(currentTokenID)] = toLower(currentToken);
    }

    std::vector<std::string> tokens;
    for (const auto& currentTokenIDs : tokenIDs) {
        if (tMap.count(currentTokenIDs)) {
            tokens.push_back(tMap[currentTokenIDs]);
        }
    }
    return tokens;
}
/**
 *
*  @param tID A vector of token IDs
 * @param in Provided dictionary of tokens/token ids/token embeddings
 *
** Reads our dictionary
 * Takes the vector of Token IDs
 * Converts IDs into token embeddings
 *
 * @return Word to Vectors
 */
std::vector<std::vector<double>> Tokenizer::embedTokenVector(std::vector<int> tID, const std::string& in) {
    std::vector<int> tokenIDs = std::move(tID);
    std::unordered_map<int, std::vector<double>> tMap;

    std::ifstream inFile(in);
    if(!inFile.is_open()) {
        std::cerr << "Could not open file: " << std::endl;
        return {};
    }

    std::string line;
    std::vector<double> emedded;
    while(std::getline(inFile, line)){
        size_t vPos = line.find("<TAB>");
        size_t tPos = line.find(':');
        std::string currentTokenID = line.substr(0, tPos);
        std::string currentTokenVectorStartingIndex = line.substr(vPos+5);
        std::stringstream ss(currentTokenVectorStartingIndex);
        std::vector<double> currentTokenVector;
        double currentVector;
        while (ss >> currentVector) {
            currentTokenVector.push_back(currentVector);
        }
        tMap[std::stoi(currentTokenID)] = std::move(currentTokenVector);
    }

    std::vector<std::vector<double>> tokensEmbeddings;
    for (int currentTokenID : tokenIDs) {

        auto it = tMap.find(currentTokenID);
        if (it != tMap.end()) {
            tokensEmbeddings.push_back(it -> second);
        }
    }
    return tokensEmbeddings;
}
/**
 *
*  @param tEmbed  A vector of token embeddings
 * @param in Provided dictionary of tokens/token ids/token embeddings
 *
** Reads our dictionary
 * Takes the vector of Token IDs
 * Converts token embeddings to token IDs to then tokens
 *
 * @return Vector to Words
 */
std::vector<std::string> Tokenizer::decodeEmbedToken(const std::vector<std::vector<double>>& tEmbed, const std::string &in){
    std::vector<std::vector<double>> tokenEmbedded = tEmbed;
    std::map<std::vector<double>,int> tMap;
    std::ifstream inFile(in);
    if(!inFile.is_open()) {
        std::cerr << "Could not open file: " << std::endl;
        return {};
    }

    std::string line;
    std::vector<double> embedded;
    while(std::getline(inFile, line)){
        size_t vPos = line.find('<TAB>');
        size_t tPos = line.find(':');
        std::string currentTokenID = line.substr(0, tPos);
        std::string currentTokenVectorStartingIndex = line.substr(vPos+1);
        std::stringstream ss(currentTokenVectorStartingIndex);
        std::vector<double> currentTokenVector;
        double currentVector;
        while (ss >> currentVector) {
            currentTokenVector.push_back(currentVector);
        }
        tMap[currentTokenVector] = std::stoi(currentTokenID);
    }


    //basically reversed embeddings, reverse the hashmap and search
    std::vector<int> tokensID;
    for (const std::vector<double>& currentTokenVec : tokenEmbedded) {
        auto it = tMap.find(currentTokenVec);
        if (it != tMap.end()) {
            tokensID.push_back(it -> second);
        }
    }
    std::vector<std::string> tokens = decodeTokens(tokensID, in); //decode the IDs easier to search
    return tokens;
}
/**
 *
 * @param tID A token ID
 * @param in Provided dictionary of tokens/token ids/token embeddings
 *
 * Reads our dictionary
 * Takes the Token ID
 * Converts ID into token embeddings
 *
 * @return Word to Vector
 */

Matrix Tokenizer::embedToken(int tID, const std::string &in) {
    std::ifstream inFile(in);
    if (!inFile.is_open()) {
        std::cerr << "Could not open file: " << in << std::endl;
        return {0, 0};
    }

    std::string line;
    while (std::getline(inFile, line)) {
        size_t tPos = line.find(':');
        size_t vPos = line.find("<TAB>");
        if (tPos == std::string::npos || vPos == std::string::npos) continue;

        int id = std::stoi(line.substr(0, tPos));
        if (id != tID) continue;

        std::stringstream ss(line.substr(vPos + 5));
        std::vector<double> vec;
        double x;
        while (ss >> x) vec.push_back(x);

        Matrix row(1, static_cast<int>(vec.size()));
        for (int j = 0; j < static_cast<int>(vec.size()); ++j) {
            row.setValue(0, j, vec[j]);
        }

        return row;
    }

    return {0, 0};
}
//Stream Operators
std::ostream &operator<<(std::ostream &os, const std::vector<char> &v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << "\"" << v[i] << "\"";
        if (i + 1 < v.size()) os << ",";
    }
    os << "]";
    return os;
}
std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << "\"" << v[i] << "\"";
        if (i + 1 < v.size()) os << ",";
    }
    os << "]";
    return os;
}
std::ostream &operator<<(std::ostream &os, const std::vector<int> &v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << "\"" << v[i] << "\"";
        if (i + 1 < v.size()) os << ",";
    }
    os << "]";
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::vector<double>& v) {
    os << "[ ";
    for (double d : v) os << d << " ";
    os << "]";
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<double>>& vv) {
    os << "{\n";
    for (const auto& v : vv) {
        os << "," << v << "\n";  // uses the vector<double> overload above
    }
    os << "}";
    return os;
}

//Debug Main Function

/*
int main() {
    std::string promptedText = "What up future king over here";
    std::vector<std::string> tokenizedText =  Tokenizer::WTE(promptedText, SP);
    std::vector<int> tokenIDs = Tokenizer::encodeTokens(tokenizedText, "../Files/VocabEmbeddings.txt");
    std::vector<std::string> decodedTokens = Tokenizer::decodeTokens(tokenIDs, "../Files/VocabEmbeddings.txt");
    std::vector<std::vector<double>> embeddedTokens = Tokenizer::embedToken(tokenIDs, "../Files/VocabEmbeddings.txt");
    std::vector<std::string> decodedTokensEmbed = Tokenizer::decodeEmbedToken(embeddedTokens, "../Files/VocabEmbeddings.txt");


    std::cout << "Tokens            : " << tokenizedText << std::endl;
    std::cout << "Encoded Token IDS : " << tokenIDs << std::endl;
    std::cout << "Embedded Token    : "  << embeddedTokens << std::endl;
    std::cout << "Current Embedding Size : 100 DIM" << std::endl;
    std::cout << "Decoded Token IDS From Embedding  : " << decodedTokensEmbed << std::endl;
    std::cout << "Decoded Token IDS From Tokens     : " << decodedTokens << std::endl;
}
*/




