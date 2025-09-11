//
// Created by luca eaton on 8/29/25.
//

#include "SkipGramModel.h"
#include "NegativeSampling.h"
#include "FileReader.h"
#include "Matrix.h"

#include <iostream>
#include <random>
#include <__ostream/basic_ostream.h>

#include "Tokenizer.h"


/**
 *
 * @param vS The vocab size (Last Token ID - 5)
 * @param d The dimension of the word2vec
 * @param lR The learning rate of the Skip Gram Model
 * @param kN Sample size of the negative samples "k"
 */
SkipGramModel::SkipGramModel(int vS, const int& d, double lR, int kN ): negativeSampling("./Files/text8", 0.75){
    embeddingLayerOut = Matrix(vS, d);
    embeddingLayerIn = FileReader::loadEmbeddingsToMatrix("./Files/VocabEmbeddings.txt",vS,d);
    if (vS < 0 || d < 0 || lR < 0 || kN < 0) {
        std::cerr << "vocabSize/dim/learningRate/kNegatives values are invalid" << std::endl;
        return;
    }

    vocabSize = vS;
    dim = d;
    learningRate = lR;
    kNegatives = kN;
    const int e_inRow = embeddingLayerIn.getRow();
    const int e_inCol = embeddingLayerIn.getCol();
    if (e_inRow != vS || e_inCol != d) {
        std::cerr << "in_embedding layer size invalid " << std::endl;
        return;
    }

    const int e_outRow = embeddingLayerOut.getRow();
    const int e_outCol = embeddingLayerOut.getCol();
    if (e_outRow != vS || e_outCol != d) {
        std::cerr << "out_embedding layer size invalid " << std::endl;
        return;
    }

    std::default_random_engine generator(std::random_device{}());
    std::uniform_real_distribution<> distribution(-0.5 / dim, 0.5 / dim);
    for (int i = 0; i < vS; i++) {
        for (int j = 0; j < dim; j++) {
            const double random_value = distribution(generator);
            embeddingLayerOut.setValue(i, j, random_value);
        }
    }
}

double sigmoid(const double x) {
    return 1.0 / (1.0 + exp(-x));
}

double SkipGramModel::trainPairs(int center, int target) {
    Matrix centerEmbeddingInLayerRow = embeddingLayerIn.getRowVector(center);
    Matrix targetEmbeddingOutLayerRow = embeddingLayerOut.getRowVector(target);

    std::vector<int> negativeSamples = negativeSampling.vectorSample(kNegatives);
    negativeSamples.erase(std::remove(negativeSamples.begin(), negativeSamples.end(), target), negativeSamples.end());

    double positiveScore = Matrix::dotRowVector(centerEmbeddingInLayerRow,targetEmbeddingOutLayerRow);
    for (const int negativeSample : negativeSamples) {
        double negativeScore = (Matrix::dotRowVector((Tokenizer::embedToken(negativeSample, "./Files/VocabEmbeddings.txt")),
                                                     centerEmbeddingInLayerRow));
    }

    double posScoreSigmod = sigmoid(positiveScore);
    const double posPair = -std::log(posScoreSigmod);
    double negPair = 0;
    for (const int negativeSample : negativeSamples) {
        negPair = std::log(sigmoid(negativeSamples[negativeSample]));
    }

    double lossPair = posPair - negPair;
    for (const int negativeSample : negativeSamples) {
        Matrix embeddingN = embeddingLayerOut.getRowVector(negativeSample);
        double scalarN = Matrix::dotRowVector(centerEmbeddingInLayerRow,embeddingN);
        Matrix gradLoss = Matrix::scaleMatrix((sigmoid(scalarN) - 1), embeddingN.getRowVector(negativeSample));
        Matrix::addMatrix(gradLoss,Matrix::scaleMatrix(scalarN,embeddingN));
        Matrix::subtractMatrix(centerEmbeddingInLayerRow,(Matrix::scaleMatrix(learningRate,gradLoss)));
    }

    //NOT FINISHED
}
