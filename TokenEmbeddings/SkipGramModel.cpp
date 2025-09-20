//
// Created by luca eaton on 8/29/25.
//

#include "SkipGramModel.h"

#include <fstream>

#include "NegativeSampling.h"
#include "FileReader.h"
#include "Matrix.h"

#include <iostream>
#include <random>
#include <__ostream/basic_ostream.h>

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
 * @param vS The vocab size (Last Token ID - 5)
 * @param d The dimension of the word2vec
 * @param lR The learning rate of the Skip Gram Model
 * @param kN Sample size of the negative samples "k"
 */
SkipGramModel::SkipGramModel(int vS, const int& d, double lR, int kN ): negativeSampling("../Files/text8", 0.75){
    embeddingLayerOut = Matrix(vS, d);
    embeddingLayerIn = FileReader::loadEmbeddingsToMatrix("../Files/VocabEmbeddings.txt",vS,d);
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
/**
 *
 * @param center Center ID
 * @param target Target ID
 * @return
 */
double SkipGramModel::trainPairs(const int center, const int target) {
    const int c = center - 5;
    const int t = target - 5;
    if (c < 0 || c >= vocabSize || t < 0 || t >= vocabSize) {
        return 0.0;
    }

    const auto centerEmbeddingInLayerRow = embeddingLayerIn.rowSpan(c);
    const auto targetEmbeddingOutLayerRow = embeddingLayerOut.rowSpan(t);
    std::vector<int> negativeSamples = negativeSampling.vectorSample(kNegatives);
    negativeSamples.erase(std::ranges::remove(negativeSamples, t).begin(), negativeSamples.end());
    const double posScore = dot(centerEmbeddingInLayerRow,targetEmbeddingOutLayerRow);
    const double posLoss = -log_sigmoid(posScore);
    double negLoss = 0.0;
    //neg score
    for (const int negativeSample : negativeSamples) {
        if (negativeSample < 0 || negativeSample >= vocabSize) continue;
        const double negativeScore = (dot((embeddingLayerIn.rowSpan(negativeSample)),centerEmbeddingInLayerRow));
        negLoss += log_sigmoid(-negativeScore);
    }

    const double lossPair = posLoss + negLoss;
    std::vector gradientVC(centerEmbeddingInLayerRow.size(), 0.0);
    std::vector gradientUO(centerEmbeddingInLayerRow.size(), 0.0);
    axpy(((sigmoid(posScore)) - 1.0), targetEmbeddingOutLayerRow, gradientVC);
    axpy(((sigmoid(posScore)) - 1.0), centerEmbeddingInLayerRow, gradientUO);

    //Gradients
    for (const int negativeSample : negativeSamples) {
        if (negativeSample < 0 || negativeSample >= vocabSize) continue;
        auto embeddingN = embeddingLayerOut.rowSpan(negativeSample);
        const double scalarN = dot(embeddingN, centerEmbeddingInLayerRow);
        const double gradLoss = sigmoid(scalarN);

        axpy(gradLoss, embeddingN, gradientVC);
        axpy(-learningRate * gradLoss, centerEmbeddingInLayerRow, embeddingN);
    }

    axpy(-learningRate, gradientVC, centerEmbeddingInLayerRow);
    axpy(-learningRate, gradientUO, targetEmbeddingOutLayerRow);

    return lossPair;
}
/**
 * @brief Trains on one Skip-gram context window for a given center word.
 *
 *  L_window = (1 / |C|) * Σ_{o ∈ C} [ -log σ(v_c · u_o) - Σ_{k=1...K} log σ(- v_c · u_{n_k}) ]
 *
 * @param centerId     Vocabulary ID of the center word (raw vocab ID; no index offset).
 * @param contextIds   Vocabulary IDs of the context words within the window (raw IDs).
 *                     The vector may be empty; any occurrences equal to centerId are ignored upstream.
 * @return Average Skip-Gram with Negative Sampling loss over all valid pairs in this window. Returns 0.0 if
 *                                                      @param contextIds is empty (i.e., no training performed).
 */
double SkipGramModel::trainOneContextWindow(int centerId, const std::vector<int>& contextIds){
    if (contextIds.empty()) return 0.0;

    double totalLoss = 0.0;
    double numPairsUsed = 0.0;
    for (const int contextId : contextIds) {
        if (contextId == centerId) {continue;}
        totalLoss += trainPairs(centerId, contextId);
        numPairsUsed++;
    }

    if (numPairsUsed == 0) return 0.0;
    return totalLoss / numPairsUsed;
}

double SkipGramModel::trainOnCorpus( std::vector<std::vector<int>> sentences, const int windowSize, const bool shuffle) {
    if (sentences.empty()) return 0.0;
    if (shuffle) {std::mt19937 rng(std::random_device{}()); std::ranges::shuffle(sentences, rng);}

    double totalLoss = 0.0;
    double numWindowsUsed = 0.0;
    for (const auto & sentence : sentences) {
        if (sentence.empty()) continue;
        for (int j = 0; j < sentence.size(); j++) {
            const int centerId = sentence[j];
            const int left = std::max(0, j - windowSize);
            const int right = std::min(static_cast<int>(sentence.size()) - 1, j + windowSize);
            std::vector<int> contextIds;
            contextIds.reserve(2 * windowSize);
            for (int k = left; k <= right; k++) {
                if (k == j) continue;
                contextIds.push_back(sentence[k]);
            }

            if (!contextIds.empty()) {
                totalLoss += trainOneContextWindow(centerId, contextIds);
                numWindowsUsed++;
            }
        }
    }

    if (numWindowsUsed == 0) return 0.0;
    return totalLoss / numWindowsUsed;
}

std::span<double> SkipGramModel::getEmbedding(int wordId) {
    return embeddingLayerIn.rowSpan(wordId);
}

bool SkipGramModel::saveEmbeddings(const std::string &filePath, const std::vector<std::string> &id2word) const {
    std::ofstream out(filePath);
    if (!out.is_open()) {
        std::cerr << "Could not open output file: " << filePath << "\n";
        return false;
    }

    const int rows = embeddingLayerIn.getRow();
    const int cols = embeddingLayerIn.getCol();
    out.setf(0, std::ios::floatfield);
    out << std::setprecision(6);
    for (int r = 0; r < rows; ++r) {
        const int wordId = 5 + r;
        const auto row = embeddingLayerIn.rowSpan(r);
        std::string token;
        if (wordId >= 0 && wordId < static_cast<int>(id2word.size())) {
            token = id2word[wordId];
        }

        out << wordId << ":" << token << "<TAB>";
        for (int d = 0; d < cols; ++d) {
            out << row[d];
            if (d + 1 < cols) out << ' ';
        }
        out << '\n';
    }

    std::cout << "Saved Tokens Embeddings" << "\n";
    return true;
}