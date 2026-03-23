//
// Created by luca eaton on 9/20/25.
// Edited by luca eaton on 3/20/26
//

#include "Evaluation.h"
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <utility>
#include <vector>
#include "Dataset.h"
#include "../Token/Tokenizer.h"

double euclideanNorm(const Matrix& matrix) {
    double sum_of_squares = 0.0;
    for (int r = 0; r < matrix.getRow(); r++) {
        for (const double element : matrix.rowSpan(r)) {
            sum_of_squares += element * element;
        }
    }
    return std::sqrt(sum_of_squares);
}

double euclideanNorm(const std::vector<double>& vec) {
    double sum_of_squares = 0.0;
    for (const double element : vec) {
        sum_of_squares += element * element;
    }

    return std::sqrt(sum_of_squares);
}

Matrix Evaluation::vectorRetriever(int tokenID) {
    std::cout << "if i am called there is still an error lying within ur code, please replace me with : matrixName.rowSpan(id - 5)" << std::endl;
    return {0,0};
}

double Evaluation::cosineSimilarity(const int tokenID1, const int tokenID2) {
    const Matrix row1 = vectorRetriever(tokenID1);
    const Matrix row2 = vectorRetriever(tokenID2);

    const double numerator = row1.dot(row2);

    const double row1Magnitude = euclideanNorm(row1);
    const double row2Magnitude = euclideanNorm(row2);

    const double denominator = row1Magnitude * row2Magnitude;

    return numerator / denominator;
}

std::unordered_map<std::string, double> Evaluation::normCache;

// std::set<std::pair<std::string, int>> Evaluation::nearestNeighbors(const std::string &targetToken, int k) {
//     if (normCache.empty()) {
//         std::cout << "Loading norms into cache (Only for once, may take a moment)" << std::endl;
//         std::ifstream inFile("../Files/VocabEmbeddingsTrained.txt");
//         if (!inFile.is_open()) {
//             std::cerr << "Could not open file: VocabEmbeddingsTrained.txt" << std::endl;
//             return {};
//         }
//
//         std::string line;
//         int count = 0;
//         while(std::getline(inFile, line)) {
//             const size_t tPos = line.find(':');
//             size_t vPos = line.find("<TAB>");
//             std::string currentTokenID = line.substr(0, tPos);
//             std::string currentTokenVectorStartingIndex = line.substr(vPos+5);
//             std::stringstream ss(currentTokenVectorStartingIndex);
//             std::vector<double> currentTokenVector;
//             double currentVector;
//             while (ss >> currentVector) {
//                 currentTokenVector.push_back(currentVector);
//             }
//
//             double currTokenNorm = euclideanNorm(currentTokenVector);
//             std::vector tokenIds = {std::stoi(currentTokenID)};
//
//             if (std::vector<std::string> decodedTokens = Tokenizer::decodeTokens(tokenIds); !decodedTokens.empty()) {
//                 normCache[decodedTokens[0]] = currTokenNorm;
//             }
//
//             count++;
//             if (count % 10000 == 0) {
//                 std::cout << "Cached " << count << " norms so far " << std::endl;
//             }
//         }
//         inFile.close();
//         std::cout << "Cache loaded with " << normCache.size() << " norms." << std::endl;
//     }
//
//     if (!normCache.contains(targetToken)) {
//         std::cerr << "Target token '" << targetToken << "' not found in vocabulary." << std::endl;
//         return {};
//     }
//
//     const std::vector<int> t_v = Tokenizer::encodeTokens({targetToken}, "../Files/Vocab.txt");
//     Matrix targetTokenVector = vectorRetriever(t_v[0]);
//     double targetNorm = normCache[targetToken];
//     std::ifstream inFile2("../Files/Vocab.txt");
//     if (!inFile2.is_open()) {
//         std::cerr << "Could not open file: Vocab.txt" << std::endl;
//         return {};
//     }
//
//     std::vector<std::pair<double, std::string>> cosineSimVec;
//     std::string line;
//     int count = 0;
//     while(std::getline(inFile2, line)) {
//         const size_t tPos = line.find(':');
//         std::string currentTokenID = line.substr(0, tPos);
//         std::string currentToken = line.substr(tPos + 1);
//
//         if (!normCache.contains(currentToken)) {
//             continue;
//         }
//
//         Matrix currentVector = vectorRetriever(std::stoi(currentTokenID));
//         double dotProduct = targetTokenVector.dot(currentVector);
//         double currentNorm = normCache[currentToken];
//
//         double currCosineSimilarity = dotProduct / (targetNorm * currentNorm);
//         cosineSimVec.emplace_back(currCosineSimilarity, currentToken);
//
//         count++;
//         if (count % 10000 == 0) {
//             std::cout << "Processed " << count << " tokens so far." << std::endl;
//         }
//     }
//     inFile2.close();
//
//     std::cout << "Sorting results..." << std::endl;
//     std::ranges::sort(cosineSimVec, [](const auto& a, const auto& b) {
//         return a.first > b.first;
//     });
//
//     std::set<std::pair<std::string, int>> result;
//     for (int i = 0; i < k && i < cosineSimVec.size(); ++i) {
//         result.insert({cosineSimVec[i].second, static_cast<int>(cosineSimVec[i].first * 1000)});
//     }
//
//     return result;
// }