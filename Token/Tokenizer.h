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

class Tokenizer {
public:
    void loadMap(const std::string &in);
    std::vector<int> encodeTokens(std::vector<std::string> tokenVector);
    std::vector<std::string> decodeTokens(std::vector<int> tokenIDS);

    size_t size() const;

private:
    static std::string toLower(const std::string& s);
    std::unordered_map<int,std::string> tokenIDMap;
    std::unordered_map<std::string,int> tokenMap;

    template <typename Tin, typename Tout>
    Tout lookup(const Tin& input) {
        if constexpr (std::is_same_v<Tin, std::string>) {
            // input is a string, use tokenMap (word -> id)
            return tokenMap.count(input) ? tokenMap[input] : 4;
        } else {
            // input is an int, use tokenIDMap (id -> word)
            return tokenIDMap.count(input) ? tokenIDMap[input] : tokenIDMap[4];
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
