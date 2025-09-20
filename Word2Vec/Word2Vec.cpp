//
// Created by luca eaton on 9/20/25.
//

#include "Word2Vec.h"

#include <fstream>
#include <iostream>

#include "Dataset.h"
#include "../TokenEmbeddings/SkipGramModel.h"

int main() {
    constexpr int numEpochs = 4;
    constexpr int vocabSize = 71294;
    const auto tokens = Dataset::loadText("../Files/text8");
    const auto tokenIDs = Dataset::vocabToID(tokens,"../Files/Vocab.txt");

    SkipGramModel skipGram(vocabSize - 5 + 1,100,0.0025,3);

    std::cout << "Starting Training!"<<std::endl;
    for (int epoch = 0; epoch < numEpochs; epoch++) {

        const double loss = skipGram.trainOnCorpus(tokenIDs,3,true);
        std::cout << "Round: " << epoch+1 << " Loss: " << loss <<std::endl;
    }
    std::vector<std::string> id2word;
    {
        std::ifstream in("Vocab.txt");
        std::string line;
        while (std::getline(in, line)) {
            auto pos = line.find(':');
            if (pos == std::string::npos) continue;
            int id = std::stoi(line.substr(0, pos));
            std::string word = line.substr(pos + 1);
            if (id >= static_cast<int>(id2word.size())) id2word.resize(id + 1);
            id2word[id] = word;
        }
    }

    if (const bool ok = skipGram.saveEmbeddings("../Files/VocabEmbeddingsTrained.txt", id2word ); !ok) {
        std::cerr << "Failed to save embeddings\n";
    }
    else {
        std::cout << "Saved to VocabEmbeddingsTrained.txt\n";
    }

}
