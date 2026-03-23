//
// Created by luca eaton on 8/28/25.
//
#include "NegativeSampling.h"
#include "../Files/FileReader.h"
/**
 *
 * @param power usually "0.75"
 *
 * Creates a probability distribution of most common words
 * that makes the contrastive training both efficient and effective.
 *
 * Creates a table in order
 *
 */
NegativeSampling::NegativeSampling(Tokenizer& t, const double power) : power(power){
    robin_hood::unordered_map<std::string, double> wFreq = FileReader::buildFrequencies("../Files/text8");
    double z = 0;
    robin_hood::unordered_map<int, double> idFreq;
    for (const auto& [word, freq] : wFreq) {
        const auto id = t.encodeTokens({word});
        if (id[0] == 4) continue;
        double val = std::pow(freq, power);
        idFreq[id[0]] = val;
        z += val;
    }
    constexpr int tSize = 1e6;
    table.clear();
    table.reserve(tSize);
    for (const auto& [id, val] : idFreq) {
        int count = static_cast<int>(std::lround((val / z) * tSize)); // slot table
        count = std::max(0, count);
        for (int i = 0; i < count; ++i) {
            table.push_back(id); //more common words get picked
        }
    }
    vocabSize = idFreq.size();
}
/**
 *
 * Grabs a random ID from the table
 *
 * @return Random Sample
 */
int NegativeSampling::getSample() const {
    int r = rand() % table.size();
    return table[r];
}
/**
 *
 * @param k # of samples
 * @return a vector of sample IDs
 */
std::vector<int> NegativeSampling::vectorSample(const int k) const {
    std::vector<int> vectorOfSamples;
    vectorOfSamples.reserve(k);
    for (int i = 0; i < k; ++i) {
        vectorOfSamples.push_back(getSample());
    }

    return vectorOfSamples;
}



