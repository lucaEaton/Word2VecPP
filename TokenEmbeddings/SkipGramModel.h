//
// Created by luca eaton on 8/29/25.
//

#ifndef SKIPGRAMMODEL_H
#define SKIPGRAMMODEL_H
#include "Matrix.h"
#include "NegativeSampling.h"


class SkipGramModel {
    public:

    SkipGramModel(int vocabSize, const int &dim, double learningRate, int kNegatives);

    double trainPairs(int center, int target);

    double trainOneContextWindow(int centerId, const std::vector<int> &contextIds);

    double trainOnCorpus(std::vector<std::vector<int>> sentences, int windowSize, bool shuffle);

    std::span<double> getEmbedding(int wordId);

    bool saveEmbeddings(const std::string &filePath, const std::vector<std::string> &id2word) const;

private:

    NegativeSampling negativeSampling;
    Matrix embeddingLayerIn;
    Matrix embeddingLayerOut;
    int vocabSize;
    int dim;
    double learningRate;
    int kNegatives;
};



#endif //SKIPGRAMMODEL_H

