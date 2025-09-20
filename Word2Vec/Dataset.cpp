//
// Created by luca eaton on 9/20/25.
//

#include "Dataset.h"
#include "Tokenizer.h"

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
    while (file >> word) {
        tokens.push_back(word);
    }

    dataSet.push_back(std::move(tokens));

    std::cout << "Loaded tokens to dataset."<< std::endl;
    return dataSet;
}

std::vector<std::vector<int>> Dataset::vocabToID(const std::vector<std::vector<std::string>>& tokens,const std::string& in) {
    std::vector<std::vector<int>> vocabIDs;
    for (const auto& sentence : tokens) {
        if (std::vector<int> encoded = Tokenizer::encodeTokens(sentence, in); !encoded.empty()) {
            vocabIDs.push_back(std::move(encoded));

        }

    }

    std::cout << "Loaded dataset from Tokens to Token IDs."<< std::endl;
    return vocabIDs;
}

std::vector<std::vector<std::string>> Dataset::IDtoVocab(const std::vector<std::vector<int>>& tokens,const std::string& in) {
    std::vector<std::vector<std::string>> vocab;
    for (const auto& sentence : tokens) {
        if (std::vector<std::string> encoded = Tokenizer::decodeTokens(sentence, in); !encoded.empty()) {
            vocab.push_back(std::move(encoded));

        }

    }

    std::cout << "Loaded dataset from Token IDs to Tokens."<< std::endl;
    return vocab;
}




