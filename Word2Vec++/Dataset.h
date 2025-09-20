//
// Created by luca eaton on 9/20/25.
//

#ifndef DATASET_H
#define DATASET_H
#include <vector>


class Dataset {
public:
    static std::vector<std::vector<std::string>> loadText(const std::string &filePath);

    static std::vector<std::vector<int>> vocabToID(const std::vector<std::vector<std::string>> &tokens, const std::string &in);

    static std::vector<std::vector<std::string>> IDtoVocab(const std::vector<std::vector<int>> &tokens, const std::string &in);
};



#endif //DATASET_H
