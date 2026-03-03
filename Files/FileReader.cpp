//
// Created by luca eaton and derek zang on 8/15/25.
//

#include "FileReader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <unordered_set>
/**
 * Normalizes our input to not consider special characters
 *
 * @param word Word
 * @return a word with no special characters
 */
std::string FileReader::removeSP(const std::string &word) {
    std::string result;
    for (char c : word) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            result.push_back(std::tolower(c));
        }
    }
    return result;
}
/**
 * @breif storing a substring
 *
 * @param word Word
 *
 * Implements Substrings to be stored as tokenIDs
 *
 *
 * @return a vector of words based on their substrings
 */
std::vector<std::string> FileReader::subStrings(const std::string& word) {
    static std::vector<std::string> suff = {
        "s", "es", "ed", "ing", "ly", "er", "or", "ion", "tion", "ation", "ition",
        "ible", "able", "al", "ial", "y", "ness", "ity", "ment", "ic", "ous", "eous", "ious",
        "en", "ify", "ise", "ize", "ward", "wise", "hood", "ship", "dom", "ful", "less"
    };
    static std::vector<std::string> pre = {
        "un", "re", "in", "im", "ir", "il", "dis", "en", "em",
        "non", "over", "mis", "sub", "pre", "inter", "fore", "de",
        "trans", "super", "semi", "anti", "mid", "under", "over",
    };

    for (auto& p : pre) {
        if (word.rfind(p, 0) == 0 && word.size() > p.size()) {
            return {p, "##" + word.substr(p.size())};
        }
    }

    for (auto& s : suff) {
        if (word.size() > s.size() &&
            word.compare(word.size()-s.size(), s.size(), s) == 0) {
            return {word.substr(0, word.size()-s.size()), "##" + s};
            }
    }

    return {word};
}
/**
 * Store Vocabulary Method
 *
 * @param inFile The file we wish to "read"
 *
 * @return our set vocabulary in a set of strings
 */
std::set<std::string> FileReader::storeVocab(const std::string &inFile)
{
    std::set<std::string> vocab;
    std::ifstream file(inFile);
    if (!file.is_open()) {
        std::cerr << "Could not open the file! : Source FileReader.cpp Line:76" << std::endl;
        return vocab;
    }

    std::string word;
    while (file >> word) {
        std::string noSP = removeSP(word);
        if (!noSP.empty()) {
            vocab.insert(noSP);
        }
    }

    file.close();
    return vocab;
}
/**
 *
 * @param v A set of strings, provided by "storeVocab()"
 * @param tgtFile The file we wish to write to
 *
 * @breif Writes our set of string including substrings from storeVocab to a file
 */
void FileReader::writeVocabSub(const std::set<std::string>& v, const std::string &tgtFile) {
    std::ofstream outFile(tgtFile);
    const std::set<std::string>& vocab = v;
    std::unordered_set<std::string> seen;
    int id = 5;
    for (const auto& word : vocab) {

        if (seen.insert(word).second) {
            outFile << id << ":" << word << "\n";
            id++;
        }
    }
    std::cout << "Created ID Tokens Successfully."<< std::endl;
    outFile.close();
}
/**
 *
 * @param v A set of strings, provided by "storeVocab()"
 * @param tgtFile The file we wish to write to
 *
 * @brief Writes our set of string from to a file
 */
void FileReader::writeVocab(const std::set<std::string>& v, const std::string &tgtFile) {
    std::ofstream outFile(tgtFile);
    const std::set<std::string>& vocab = v;
    if (!outFile.is_open()) {
        std::cerr << "Could not open the output file!" << std::endl;
    }
    std::unordered_set<std::string> seen;
    int id = 5;
    for (const auto& word : vocab) {

            if (seen.insert(word).second) {
                outFile << id << ":" << word << "\n";
                id++;
            }
    }
    std::cout << "Created ID Tokens Successfully Without Frequencies."<< std::endl;
    outFile.close();
}
/**
 *
 * @param inFile The file we wish to read
 * @return map of words to freq
 */
std::unordered_map<std::string, double> FileReader::buildFrequencies(const std::string &inFile) {
    std::unordered_map<std::string,double> freq;
    std::ifstream file(inFile);
    if (!file.is_open()) {
        std::cerr << "Could not open the file! : Source FileReader.cpp Line:147" << std::endl;
        return freq;
    }
    std::string word;
    while (file >> word) {
        std::string w = removeSP(word);
        if (!w.empty()) ++freq[w];
    }
    return freq;
}
/**
 *
 * @param v the set of strings from "StoreVocab()"
 * @param tgtFile The file to write to
 * @param corpusFile The file to read from
 * @param min_count Condition of which words to choose from depending on how much they see
 */
void FileReader::writeVocabFiltered(const std::set<std::string>& v,const std::string& tgtFile,const std::string& corpusFile, const int min_count = 5) {
    std::ofstream outFile(tgtFile);
    if (!outFile.is_open()) {
        std::cerr << "Could not open the output file!" << std::endl;
        return;
    }

    auto freq = buildFrequencies(corpusFile);
    std::unordered_set<std::string> seen;
    int id = 5;
    for (const auto& word : v) {
        auto it = freq.find(word);
        if (it == freq.end() || it->second < min_count) continue;

        if (seen.insert(word).second) {
            outFile << id << ":" << word << "\n";
            ++id;
        }
    }
    std::cout << "Created ID Tokens Successfully (Min Count Being : " << min_count << ").\n";
    outFile.close();
}

static bool parseDoubles(const std::string& s, std::vector<double>& out, int expectedDim) {
    std::istringstream iss(s);
    double x;
    out.clear();
    while (iss >> x) out.push_back(x);
    return static_cast<int>(out.size()) == expectedDim;
}
/**
 *
 * @param path File to retrieve Embeddings from
 * @param vocabSize The vocabulary size of tokens (71k-72K)
 * @param dim The Dimension of the matrix embedding
 * @return the embeddings
 */
Matrix FileReader::loadEmbeddingsToMatrix(const std::string& path, int vocabSize, int dim) {
    Matrix M(vocabSize, dim);
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("FileReader::loadEmbeddingsToMatrix: could not open " + path);
    }

    std::string line;
    int lineNo = 0, loaded = 0;
    std::vector<double> buf; buf.reserve(dim);

    while (std::getline(in, line)) {
        constexpr int kBaseId = 5;
        ++lineNo;
        if (line.empty()) continue;

        const std::string sep = "<TAB>";
        auto tabPos = line.find(sep);
        if (tabPos == std::string::npos) {
            std::cerr << "line " << lineNo << ": missing <TAB>; skipping\n";
            continue;
        }

        std::string left  = line.substr(0, tabPos);
        std::string right = line.substr(tabPos + static_cast<int>(sep.size()));  // <-- skip all 5 chars

        auto colonPos = left.find(':');
        if (colonPos == std::string::npos) {
            std::cerr << "[warn] line " << lineNo << ": missing ':'; skipping\n";
            continue;
        }

        std::string idStr = left.substr(0, colonPos);
        int fileId;
        try {
            fileId = std::stoi(idStr);
        } catch (...) {
            std::cerr << "[warn] line " << lineNo << ": bad id '" << idStr << "'; skipping\n";
            continue;
        }

        // map absolute file ID -> zero-based row index
        const int row = fileId - kBaseId;      // <-- critical fix
        if (row < 0 || row >= vocabSize) {
            std::cerr << "[warn] line " << lineNo << ": id " << fileId
                      << " out of range [" << kBaseId << "," << (kBaseId + vocabSize - 1) << "]; skipping\n";
            continue;
        }

        if (!parseDoubles(right, buf, dim)) {
            std::cerr << "[warn] line " << lineNo << ": expected " << dim
                      << " floats; got different count; skipping\n";
            continue;
        }

        for (int j = 0; j < dim; ++j) {
            M.setValue(row, j, buf[j]);        // <-- write into zero-based row
        }
        ++loaded;
    }

    if (loaded == 0) {
        std::cerr << "loaded 0 rows from " << path << "\n";
    }
    return M;
}


//Print Ops
std::ostream& operator<<(std::ostream &os, const std::set<std::string> &vocab) {
    for (const auto& word : vocab) {
        os << word << '\n';
    }
    return os;
}

//Main Function
/*
int main() {
    auto vocab = storeVocab("Files/text8");
    writeVocabFiltered(vocab, "Files/Vocab.txt", "Files/text8", 5);
}
*/
