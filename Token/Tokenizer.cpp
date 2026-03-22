//
// Created by luca eaton and derek zang on 8/14/25.
// edited by luca eaton on 3/20/26
//
#include "Tokenizer.h"

#include <charconv>
#include <chrono>
#include <iostream>
#include <fstream>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
/**
 * toLower Function
 *
 * @param str provided word
 * @return the word in all lower case
 */
std::string Tokenizer::toLower(const std::string& str) {
    std::string result = str;
    std::ranges::transform(result, result.begin(),
                           [](const unsigned char c) { return std::tolower(c); });
    return result;
}
/**
 * @brief Loads a token file into memory and builds two lookup tables:
 *        one for looking up a word by its ID, and one for looking up an ID by its word.
 *
 * The file should be formatted where each line looks like:
 *        5:hello
 *        6:joe
 * Where the number on the left is the token ID and the word on the right is the token.
 *
 * @param in the path to the token file (e.g. "Vocab.txt")
 *
 * @note  First pass, scans the file once just to count how many tokens there are,
 *                      so we can reserve the exact memory needed upfront (faster).
 * @note  Second pass, actually reads each line, splits it at the ':', and stores:
 *                      - tokenIDMap[id]   = word  (give me an ID, I'll give you the word)
 *                      - tokenMap[word]   = id    (give me a word, I'll give you the ID)
 *
 * @note  Uses mmap instead of ifstream to read the file directly from memory,
 *        which avoids unnecessary data copies and speeds up parsing.
 *
 * @note  string_view is used to avoid copying strings during parsing
 *        each word just points directly into the mapped file memory.
 *
 * @warning  if the file can't be opened or mmap fails, an error is printed, function fails.
 *
 * @relates Tokenizer::tokenMap      (word → id  lookup table)
 * @relates Tokenizer::tokenIDMap    (id   → word lookup table)
 *
 * @timecomplex O(n), n = # of bytes
 * @performance  runs in avg ~22ms for 71,294 tokens, which is around ~3.24M tokens/sec
 */
void Tokenizer::loapMapV2(const std::string& in) {
    const auto t_start = std::chrono::high_resolution_clock::now();
    const int fd = open(in.c_str(), O_RDONLY);
    if (fd == -1) {std::cerr << "file cannot be opened" << std::endl; return;}
    struct stat sb{}; fstat(fd, &sb); // struct stat is defined by the OS that holds the metadat
    const auto data = static_cast<char *>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)); close(fd); // closes the file descriptor, alr mapped into memory
    if (data == MAP_FAILED) std::cerr << "mmap failed" << std::endl;
    const char *end = data + sb.st_size; // 'data' points to the start of the file, adds the file size in bytes, so this points to one byte past the last character of the file
    const char *line = data; // moving pointer that starts at the beginning of the file
    long long count = 0;
    while (line < end) {
        const char *tokens = line;
        while (tokens < end && *tokens != '\n') tokens++;
        count++;
        line = tokens + 1;
    }
    // pre-allocate that much space
    tokenIDMap.reserve(count);
    tokenMap.reserve(count);

    line = data;
    while (line < end) {
        const char *currColon = line;
        while (currColon < end && *currColon != ':') currColon++;
        const char *currLine = currColon;
        while (currLine < end && *currLine != '\n') currLine++;
        int id = 0;
        std::from_chars(line, currColon, id); // read chars from the first line to currColon.
        std::string_view word(currColon + 1, currLine - currColon - 1); //stores the word itself
        tokenIDMap[id] = word;
        tokenMap[word] = id; //stores pointer
        line = currLine + 1;
    }
    //keeping the map alive for std::string_view
    mappedData = data;
    mappedSize = sb.st_size;
    const auto t_end = std::chrono::high_resolution_clock::now();
    std::cout << "total tokens loaded : " << tokenIDMap.size() << std::endl;
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "time taken to map tokens: " << duration.count() << " ms\n";
}
/**
 * @param in tokenID to token reference file
 *
 * the first run counts the amount of tokens within the reference file
 * in order to allocate memory towards the maps themselves
 *
 * the second run is what sets the key -> values for O(1) look up of tokens
 * @relates Tokenizer::tokenMap
 * @relates Tokenizer::tokenIDMap
 */
void Tokenizer::loadMap(const std::string& in) {
    const auto t_start = std::chrono::high_resolution_clock::now();
    std::ifstream ifs(in);
    if (!ifs.is_open()) {
        std::cerr << "file cannot be read. no tokens stored " << std::endl;
        return;
    }
    const long long count = std::count(std::istreambuf_iterator<char>(ifs),std::istreambuf_iterator<char>(), '\n'); // count lines
    //allocate memory
    tokenIDMap.reserve(count);
    tokenMap.reserve(count);
    std::string line;
    while (std::getline(ifs, line)) {
        const size_t pos = line.find(':'); // find the token id
        const size_t vPos = line.find("<TAB>"); // find the token itself
        std::string currentTokenID = line.substr(0, pos); // everything before the ":"
        std::string currentToken = line.substr(pos+1, vPos - pos - 1); // everything between the ":" and "<TAB>
        tokenIDMap[std::stoi(currentTokenID)] = currentToken;
        tokenMap[toLower(currentToken)] = std::stoi(currentTokenID);
    }
    const auto t_end = std::chrono::high_resolution_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "total tokens loaded : " << tokenIDMap.size() << std::endl;
    std::cout << "time taken to map tokens: " << duration.count() << " ms\n";
}
/**
 *
 * @param tV Our tokenized sentence/words\
 *
 * Loops through each object within our tV vector
 * finds each token though our map
 * pushes it within our result vector
 * if it doesn't find it will push in the token id "4", which means unknown <UNK>
 *
 * @relates Tokenizer::tokenMap
 * @see template lookup in "Tokenizer.h"
 * @timecomplx O(T) T = tV size;
 * @return tokenIDs : The token IDs of each provided token.
 */
std::vector<int> Tokenizer::encodeTokens(std::vector<std::string> tV) {
    const std::vector<std::string> tokens = std::move(tV);
    std::vector<int> res;
    for (const auto& t : tokens) {
        res.push_back(lookup<std::string, int>(t));
    }
    return res;
}
/**
 *
 * @param tID A vector of token IDs
 *
 * Loops through each object within our tV vector
 * finds each token though our map
 * pushes it within our result vector
 * if it doesn't find it will push in the token representing id "4", which means unknown <UNK>
 *
 * @relates Tokenizer::tokenIDMap
 * @see template lookup in "Tokenizer.h"
 * @timecomplx O(I) I = tID size
 * @return A decoded vector of tokens from the provided token vector
 */
std::vector<std::string> Tokenizer::decodeTokens(std::vector<int> tID) {
    const std::vector<int> tokenIDs = std::move(tID);
    std::vector<std::string> res;
    for (const auto& id : tokenIDs) {
        res.push_back(lookup<int, std::string>(id));
    }
    return res;
}

/**
 *
 * @return the amount of tokens we hold
 */
size_t Tokenizer::size() const {
    return tokenIDMap.size();
}
