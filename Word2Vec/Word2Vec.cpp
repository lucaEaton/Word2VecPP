//
// Created by luca eaton on 9/20/25.
//

#include "Word2Vec.h"

#include <fstream>
#include <iostream>

#include "Dataset.h"
#include "../Files/FileReader.h"
#include "../Token/Tokenizer.h"
#include "../TokenEmbeddings/SkipGramModel.h"

int main() {
    // std::cout << "test" << std::endl;
    // Tokenizer t;
    // t.loapMapV2();
    // Matrix m = FileReader::loadEmbeddingsToMatrix(static_cast<int>(t.size()),300);
    // return 0;

    Tokenizer tokenizer;
    tokenizer.loapMapV2();
    constexpr int numEpochs = 40;
    const size_t vocabSize = tokenizer.size();

    const auto tokens = Dataset::loadDataset("../Files/text8",tokenizer); // text 8 tokens

    SkipGramModel skipGram(vocabSize,100,0.0025,3, tokenizer);

    std::cout << "starting training"<<std::endl;
    for (int epoch = 0; epoch < numEpochs; epoch++) {

        const double loss = skipGram.trainOnCorpus(tokens,3,true);
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
