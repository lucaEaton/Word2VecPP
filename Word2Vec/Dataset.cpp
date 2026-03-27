//
// Created by luca eaton on 9/20/25.
// Edited by luca eaton on 3/20/25
//

#include "Dataset.h"

#include <chrono>

#include "../Token/Tokenizer.h"
#include <fstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <chrono>

std::vector<std::vector<std::string>> Dataset::loadText(const std::string &filePath) {
    const auto t_start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<std::string>> dataSet;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << filePath << std::endl;
        return {};
    }
    std::vector<std::string> tokens;
    std::string word;
    while (file >> word) tokens.push_back(word);
    dataSet.push_back(std::move(tokens));
    const auto t_end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "loaded tokens to dataset | time taken:  "<< duration <<std::endl;
    return dataSet;
}

/**
 *
 * @param filePath corpus
 * @param t tokenizer
 * @return dataset broken in chunks of 1000 words each
 *
 * run time to beat -> ~1100ms √ -> ~3800ms -> ~2500ms
 */
std::vector<std::vector<int>> Dataset::loadDataset(const std::string &filePath, Tokenizer &t) {
    const auto t_start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<int>> dataSet;
    const std::string& in = filePath;
    const int fd = open(in.c_str(), O_RDONLY);
    if (fd == -1) {std::cerr << "could not open file " << in << std::endl; return {};}
    struct stat sb{}; fstat(fd, &sb);
    const auto data = static_cast<char *>(mmap(nullptr, sb.st_size,PROT_READ, MAP_PRIVATE, fd, 0));
    close(fd);
    if (data == MAP_FAILED) {std::cerr << "mmap failed " << std::endl; return {};}
    const char *end = data + sb.st_size;
    const char *byte = data;
    long long count = 0;
    while (byte < end) {
        while (byte < end && *byte == ' ') byte++;
        if (byte >= end) break;
        while (byte < end && *byte != ' ') byte++;
        count++;
    }
    dataSet.reserve((count/1000)+1); // chunks of 1000
    byte = data;
    long long wordCount = 0;
    while (byte < end) {
        while (byte < end && *byte == ' ') byte++;
        if (byte >= end) break;
        const char* endPtr = byte;
        while (endPtr < end && *endPtr != ' ') endPtr++;
        if (wordCount % 1000 == 0) dataSet.emplace_back();
        dataSet.back().emplace_back(t.encodeSingleToken(std::string_view(byte, endPtr)));
        wordCount++;
        byte = endPtr;
    }
    munmap(data, sb.st_size);
    const auto t_end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "loaded tokens to dataset, # of sentences: " << dataSet.size()  <<  " | time taken:  " << duration <<std::endl;
    return dataSet;
}

std::vector<std::vector<int>> Dataset::vocabToID(const std::vector<std::vector<std::string>>& tokens, Tokenizer& tokenizer) {
    const auto t_start = std::chrono::high_resolution_clock::now();
    auto result = convertSentences<std::string, int>(tokens,
        [&](const std::vector<std::string>& s) { return tokenizer.encodeTokens(s); });
    const auto t_end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "loaded dataset from tokens to ids | time taken:  "<< duration <<std::endl;
    return result;
}

std::vector<std::vector<std::string>> Dataset::IDtoVocab(const std::vector<std::vector<int>>& tokens, Tokenizer& tokenizer) {
    auto result = convertSentences<int, std::string>(tokens,
        [&](const std::vector<int>& s) { return tokenizer.decodeTokens(s); });
    std::cout << "loaded dataset from token IDs to tokens" << std::endl;
    return result;
}




