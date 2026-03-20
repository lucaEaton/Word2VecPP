//
// Created by luca eaton on 9/20/25.
// Edited by luca eaton on 3/20/25
//
#ifndef DATASET_H
#define DATASET_H
#include <functional>
#include <vector>
#include "../Token/Tokenizer.h"


class Dataset {
    public:
        static std::vector<std::vector<std::string>> loadText(const std::string &filePath);
        static std::vector<std::vector<int>> vocabToID(const std::vector<std::vector<std::string>>& tokens, Tokenizer& tokenizer);
        static std::vector<std::vector<std::string>> IDtoVocab(const std::vector<std::vector<int>>& tokens, Tokenizer& tokenizer);

    private:
        template <typename TIn, typename TOut>
        static std::vector<std::vector<TOut>> convertSentences(
        const std::vector<std::vector<TIn>>& tokens,
        std::function<std::vector<TOut>(const std::vector<TIn>&)> converter)
        {
            std::vector<std::vector<TOut>> result;
            for (const auto& sentence : tokens) {
                if (auto converted = converter(sentence); !converted.empty()) {
                    result.push_back(std::move(converted));
                }
            }
            return result;
        }
};



#endif //DATASET_H
