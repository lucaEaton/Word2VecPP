//
// Created by luca eaton on 9/20/25.
//

#ifndef EVALUATION_H
#define EVALUATION_H
#include <set>
#include <unordered_map>

#include "../Token/Tokenizer.h"
#include "../TokenEmbeddings/Matrix.h"


class Evaluation {
public:
    static Matrix vectorRetriever(int tokenID);

    static double cosineSimilarity(int tokenID1, int tokenID2);

    //static std::set<std::pair<std::string, int>> nearestNeighbors(const std::string& targetToken, int k);

private:
    static std::unordered_map<std::string, double> normCache;
    Tokenizer tempTokenizer;
};



#endif //EVALUATION_H