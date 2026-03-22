//
// Created by luca eaton and derek zang on 8/14/25.
// edited by luca eaton on 3/20/26
//
#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <string>
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <sys/mman.h>
#include "robin_hood.h"

class Tokenizer {
public:
    void loadMap(const std::string &in);
    void loapMapV2(const std::string &in);
    std::vector<int> encodeTokens(std::vector<std::string> tokenVector);
    std::vector<std::string> decodeTokens(std::vector<int> tokenIDS);
    [[nodiscard]] size_t size() const;
    ~Tokenizer() {
        if (mappedData) munmap(mappedData, mappedSize);
    }

private:
    static std::string toLower(const std::string& s);
    robin_hood::unordered_map<int, std::string_view> tokenIDMap;
    robin_hood::unordered_map<std::string_view, int> tokenMap;

    char* mappedData = nullptr;
    size_t mappedSize = 0;

    template <typename Tin, typename Tout>
    Tout lookup(const Tin& input) {
        if constexpr (std::is_same_v<Tin, std::string>) {
            // word -> id
            const std::string_view sv(input);
            const auto it = tokenMap.find(sv);
            return it != tokenMap.end() ? it->second : 4;
        } else {
            // id -> word
            auto it = tokenIDMap.find(input);
            const std::string_view sv = (it != tokenIDMap.end()) ? it->second : tokenIDMap.at(4);
            return std::string(sv);
        }
    }
};

/// Stream operators (same behavior as your originals)
std::ostream& operator<<(std::ostream& os, const std::vector<char>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<int>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<double>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<double>>& vv);

#endif // TOKENIZER_H
