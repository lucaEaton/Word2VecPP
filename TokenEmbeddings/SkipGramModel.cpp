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
inline double log_sigmoid(double x) {
    if (x >= 0) return -std::log1p(std::exp(-x));
    return x - std::log1p(std::exp(x));
}

inline double dot(std::span<const double> a, std::span<const double> b) {
    double result = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        result += a[i] * b[i];
    }
    return result;
}

inline void axpy(const double alpha, const std::span<const double> x, std::span<double> y) {
    for (size_t i = 0; i < x.size(); i++) {
        y[i] += alpha * x[i];
    }
}

/**
 *
 * @param center Center ID
 * @param target Target ID
 * @return
 */
double SkipGramModel::trainPairs(const int center, const int target) {
    const auto centerEmbeddingInLayerRow = embeddingLayerIn.rowSpan(center-5);
    const auto targetEmbeddingOutLayerRow = embeddingLayerOut.rowSpan(target-5);
    std::vector<int> negativeSamples = negativeSampling.vectorSample(kNegatives);
    negativeSamples.erase(std::ranges::remove(negativeSamples, target).begin(), negativeSamples.end());
    const double posScore = dot(centerEmbeddingInLayerRow,targetEmbeddingOutLayerRow);
    const double posLoss = -log_sigmoid(posScore);
    double negLoss = 0.0;
    //neg score
    for (const int negativeSample : negativeSamples) {
        const double negativeScore = (dot((embeddingLayerIn.rowSpan(negativeSample-5)),centerEmbeddingInLayerRow));
        negLoss += log_sigmoid(-negativeScore);
    }

    double lossPair = posLoss + negLoss;
    std::vector g_vc(centerEmbeddingInLayerRow.size(), 0.0);
    std::vector g_uo(centerEmbeddingInLayerRow.size(), 0.0);
    axpy(((sigmoid(posScore)) - 1.0), targetEmbeddingOutLayerRow, g_vc);
    axpy(((sigmoid(posScore)) - 1.0), centerEmbeddingInLayerRow, g_uo);

    //Gradients
    for (const int negativeSample : negativeSamples) {
        auto embeddingN = embeddingLayerOut.rowSpan(negativeSample);
        const double scalarN = dot(embeddingN, centerEmbeddingInLayerRow);
        const double gradLoss = sigmoid(scalarN);

        axpy(gradLoss, embeddingN, g_vc);
        axpy(-learningRate * gradLoss, centerEmbeddingInLayerRow, embeddingN);
    }

    axpy(-learningRate, g_vc, centerEmbeddingInLayerRow);
    axpy(-learningRate, g_uo, targetEmbeddingOutLayerRow);

    return lossPair;
}
