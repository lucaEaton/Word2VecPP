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

