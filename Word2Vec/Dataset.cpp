//
// Created by luca eaton on 9/20/25.
// Edited by luca eaton on 3/20/25
//

#include "Dataset.h"
#include "../Token/Tokenizer.h"
#include <fstream>
#include <string>
#include <iostream>

std::vector<std::vector<std::string>> Dataset::loadText(const std::string &filePath) {
    std::vector<std::vector<std::string>> dataSet;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << filePath << std::endl;
        return {};
    }
    std::vector<std::string> tokens;
    std::string word;
    while (file >> word) tokens.push_back(word);
    dataSet.push_back(std::move(tokens));
    std::cout << "Loaded tokens to dataset"<< std::endl;
    return dataSet;
}

std::vector<std::vector<int>> Dataset::vocabToID(const std::vector<std::vector<std::string>>& tokens, Tokenizer& tokenizer) {
    auto result = convertSentences<std::string, int>(tokens,
        [&](const std::vector<std::string>& s) { return tokenizer.encodeTokens(s); });
    std::cout << "loaded dataset from tokens to token IDs" << std::endl;
    return result;
}

std::vector<std::vector<std::string>> Dataset::IDtoVocab(const std::vector<std::vector<int>>& tokens, Tokenizer& tokenizer) {
    auto result = convertSentences<int, std::string>(tokens,
        [&](const std::vector<int>& s) { return tokenizer.decodeTokens(s); });
    std::cout << "loaded dataset from token IDs to tokens" << std::endl;
    return result;
}




