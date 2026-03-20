//
// Created by luca eaton and derek zang on 8/14/25.
// edited by luca eaton on 3/20/26
//

#include "Tokenizer.h"
#include <iostream>
#include <fstream>
#include <map>
#include <utility>
#include <vector>
#include <sstream>
#include <string>
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
 * @param in tokenID to token reference file
 *
 * the first run counts the amount of tokens within the reference file
 * in order to allocate memory towards the maps themselves
 *
 * the second run is what sets the key -> values for O(1) look up of tokens
 * @relates Tokenizer::tokenMap
 * @relates Tokenizer::tokenIDMap
 */
void Tokenizer::loadMap(const std::string& in) {
    std::ifstream ifs(in);
    if (!ifs.is_open()) {
        std::cerr << "file cannot be read. no tokens stored " << std::endl;
        return;
    }
    const long long count = std::count(std::istreambuf_iterator<char>(ifs),std::istreambuf_iterator<char>(), '\n'); // count lines
    //allocate memory
    tokenIDMap.reserve(count);
    tokenMap.reserve(count);
    std::string line;
    while (std::getline(ifs, line)) {
        const size_t pos = line.find(':'); // find the token id
        const size_t vPos = line.find("<TAB>"); // find the token itself
        std::string currentTokenID = line.substr(0, pos); // everything before the ":"
        std::string currentToken = line.substr(pos+1, vPos - pos - 1); // everything between the ":" and "<TAB>
        tokenIDMap[std::stoi(currentTokenID)] = currentToken;
        tokenMap[toLower(currentToken)] = std::stoi(currentTokenID);
    }
    std::cout << "total tokens loaded : " << tokenIDMap.size() << std::endl;
}
/**
 *
 * @param tV Our tokenized sentence/words\
 *
 * Loops through each object within our tV vector
 * finds each token though our map
 * pushes it within our result vector
 * if it doesn't find it will push in the token id "4", which means unknown <UNK>
 *
 * @relates Tokenizer::tokenMap
 * @see template lookup in "Tokenizer.h"
 * @timecomplx O(T) T = tV size;
 * @return tokenIDs : The token IDs of each provided token.
 */
std::vector<int> Tokenizer::encodeTokens(std::vector<std::string> tV) {
    const std::vector<std::string> tokens = std::move(tV);
    std::vector<int> res;
    for (const auto& t : tokens) {
        res.push_back(lookup<std::string, int>(t));
    }
    return res;
}
/**
 *
 * @param tID A vector of token IDs
 *
 * Loops through each object within our tV vector
 * finds each token though our map
 * pushes it within our result vector
 * if it doesn't find it will push in the token representing id "4", which means unknown <UNK>
 *
 * @relates Tokenizer::tokenIDMap
 * @see template lookup in "Tokenizer.h"
 * @timecomplx O(I) I = tID size
 * @return A decoded vector of tokens from the provided token vector
 */
std::vector<std::string> Tokenizer::decodeTokens(std::vector<int> tID) {
    const std::vector<int> tokenIDs = std::move(tID);
    std::vector<std::string> res;
    for (const auto& id : tokenIDs) {
        res.push_back(lookup<int, std::string>(id));
    }
    return res;
}

/**
 *
 * @return the amount of tokens we hold
 */
size_t Tokenizer::size() const {
    return tokenIDMap.size();
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




