//
// Created by luca eaton and derek zang on 8/21/25.
//

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <algorithm>

class startingEmbeddings {
public:
    startingEmbeddings(std::string  inFile,
                       std::string  outFile,
                       int dim = 128,
                       double lo = -1.0,
                       double hi = 1.0,
                       unsigned long long seed = 0)
        : inFile(std::move(inFile)), outFile(std::move(outFile)), dim(dim), lo(lo), hi(hi), seed(seed) {}

    void generate() {
        std::ifstream in(inFile);
        if (!in.is_open()) {
            std::cerr << "Could not open input file: " << inFile << "\n";
            return;
        }
        std::ofstream out(outFile);
        if (!out.is_open()) {
            std::cerr << "Could not open output file: " << outFile << "\n";
            return;
        }
        std::random_device rd;
        unsigned long long s = (seed == 0) ? ((static_cast<unsigned long long>(rd()) << 32) ^ rd()) : seed;
        std::mt19937_64 gen(s);
        std::uniform_real_distribution<double> dist(lo, hi);
        std::string line;
        while (std::getline(in, line)) {
            auto pos = line.find(':');
            if (pos == std::string::npos) continue;
            std::string idStr = line.substr(0, pos);
            std::string word  = line.substr(pos + 1);
            out << idStr << ":" << word << "<TAB>";
            for (int d = 0; d < dim; ++d) {
                out << dist(gen);
                if (d + 1 < dim) out << ' ';
            }
            out << '\n';
        }
        std::cout << "Created Random Tokens Embeddings" << "\n";
    }

private:
    std::string inFile, outFile;
    int dim;
    double lo, hi;
    unsigned long long seed;
};


//debug main statement

int main() {
    startingEmbeddings emb("./Files/Vocab.txt","./Files/VocabEmbeddings.txt", 100, -1.0, 1.0, 1353);
    emb.generate();
    return 0;
}
