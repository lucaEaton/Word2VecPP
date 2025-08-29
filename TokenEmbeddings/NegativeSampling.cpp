//
// Created by luca eaton on 8/28/25.
//
#include "NegativeSampling.h"
#include "FileReader.h"
/**
 *
 * @param inFile Corpus (text8)
 * @param power usually "0.75"
 *
 * Creates a probability distribution of most common words
 * that makes the contrastive training both efficient and effective.
 *
 * Creates a table in order
 *
 */
NegativeSampling::NegativeSampling(const std::string& inFile, const double power) : power(power){
    std::unordered_map<std::string, double> newFreq = FileReader::buildFrequencies(inFile);
    double z = 0;
    //Formula
    for ( auto& word : newFreq) {
        word.second = std::pow(word.second, power);
    }
    //Sum
    vocabSize = newFreq.size();
    for (const auto& word : newFreq) {
        z += word.second;
    }
    //Normalization
    for (const auto& word : newFreq) {
        newFreq[word.first] = word.second / z;
    }
    //Creates a table of IDs
    constexpr int tableSize = 1e6;
    table.reserve(tableSize);
    int id = 0;
    for (const auto& kv : freq) {
        const double p = kv.second;
        constexpr int count = static_cast<int>(p * tableSize);

        for (int i = 0; i < count; ++i) {
            table.push_back(id);
        }
        ++id;
    }

    freq = std::move(newFreq);
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


