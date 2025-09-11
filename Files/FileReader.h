#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <ostream>
#include "../TokenEmbeddings/Matrix.h"

class FileReader{
public:
    static std::string removeSP(const std::string& word);
    static std::vector<std::string> subStrings(const std::string& word);
    static std::set<std::string> storeVocab(const std::string& inFile);
    static void writeVocabSub(const std::set<std::string>& v, const std::string& tgtFile);
    static void writeVocab(const std::set<std::string>& v, const std::string& tgtFile);
    static void writeVocabFiltered(const std::set<std::string>& v,
                                   const std::string& tgtFile,
                                   const std::string& corpusFile,
                                   int min_count);

    static Matrix loadEmbeddingsToMatrix(const std::string &path, int vocabSize, int dim);

    static std::unordered_map<std::string, double> buildFrequencies(const std::string &inFile);

};

std::ostream& operator<<(std::ostream& os, const std::set<std::string>& vocab);
