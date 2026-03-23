//
// Created by luca eaton and derek zang on 8/15/25.
// Edited by luca eaton 3/22/26
//

#include "FileReader.h"

#include <charconv>
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <unistd.h>
#include <unordered_set>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
/**
 * Normalizes our input to not consider special characters
 *
 * @param word Word
 * @return a word with no special characters
 */
std::string FileReader::removeSP(const std::string &word) {
    std::string result;
    for (const char c : word) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            result.push_back(std::tolower(c));
        }
    }
    return result;
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
        if (std::string noSP = removeSP(word); !noSP.empty()) {
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
/**
 *
 * @param inFile The file we wish to read
 * @return map of words to freq
 */
robin_hood::unordered_map<std::string, double> FileReader::buildFrequencies(const std::string &inFile) {
    robin_hood::unordered_map<std::string,double> freq;
    const int fd = open(inFile.c_str(), O_RDONLY);
    if (fd == -1) {std::cerr << "file was not accessed" << std::endl; return freq;}
    struct stat sb{}; fstat(fd, &sb);
    const auto data = static_cast<char*>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (data == MAP_FAILED) {std::cerr << "mmap failed" << std::endl; return freq;}
    const auto *end = data + sb.st_size;
    const auto *byte = data;
    while (byte < end) {
        while (byte < end && isspace(static_cast<unsigned char>(*byte))) byte++; // only for spaces
        if (byte >= end) break;
        const char* start = byte;
        while (byte < end && !isspace(static_cast<unsigned char>(*byte))) byte++;
        if (byte > start) {
            std::string wordStr(start, byte - start);
            freq[wordStr]++;
        }
    }
    return freq;
}
/*
 * run time ~400ms
 */
Matrix FileReader::loadEmbeddingsToMatrix(const int vocabSize, const int dim) {
    const std::string& in = ("../Files/VocabEmbeddings.txt");
    Matrix M(vocabSize, dim);
    const auto t_start = std::chrono::high_resolution_clock::now();
    const int fd = open(in.c_str(), O_RDONLY);
    if (fd == -1) {std::cerr << "file was not accessed" << std::endl; return M;}
    struct stat sb{}; fstat(fd, &sb);
    const auto data = static_cast<char *>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)); close(fd);
    if (data == MAP_FAILED) {std::cerr << "mmap failed" << std::endl; return M;}
    const char *end = data + sb.st_size;
    const char *byte = data;
    while (byte < end) {
        const char *begin = byte;
        while (begin < end && *begin != '>') ++begin; // find <TAB>
        const char *currLine = begin;
        while (currLine < end && *currLine != '\n') ++currLine; // find new line
        // everything after '<TAB>'
        const char *num = begin+1;
        while (num < currLine) {
            while (num < end && *num == ' ') ++num; //get each num
            char* endPtr; // reads one value, endPtr is set to where it stopped
            const double v = strtod(num, &endPtr);
            if (endPtr == num) break;
            M.addValue(v);
            num = endPtr;
        }
        byte = currLine + 1;
    }
    munmap(data, sb.st_size);
    const auto t_end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "data loaded: " << M.dataSize() << " | time taken to load embeddings: " << duration.count() << "ms" << std::endl;
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
