//
// Created by luca eaton on 9/20/25.
// Edited by luca eaton on 3/20/25
//

#include "Dataset.h"

#include <chrono>

#include "../Token/Tokenizer.h"
#include <fstream>
#include <string>
#include <iostream>

std::vector<std::vector<std::string>> Dataset::loadText(const std::string &filePath) {
    const auto t_start = std::chrono::high_resolution_clock::now();
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
    const auto t_end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "loaded tokens to dataset | time taken:  "<< duration <<std::endl;
    return dataSet;
}

std::vector<std::vector<int>> Dataset::vocabToID(const std::vector<std::vector<std::string>>& tokens, Tokenizer& tokenizer) {
    const auto t_start = std::chrono::high_resolution_clock::now();
    auto result = convertSentences<std::string, int>(tokens,
        [&](const std::vector<std::string>& s) { return tokenizer.encodeTokens(s); });
    const auto t_end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "loaded dataset from tokens to ids | time taken:  "<< duration <<std::endl;
    return result;
}

std::vector<std::vector<std::string>> Dataset::IDtoVocab(const std::vector<std::vector<int>>& tokens, Tokenizer& tokenizer) {
    auto result = convertSentences<int, std::string>(tokens,
        [&](const std::vector<int>& s) { return tokenizer.decodeTokens(s); });
    std::cout << "loaded dataset from token IDs to tokens" << std::endl;
    return result;
}




