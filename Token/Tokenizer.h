//
// Created by luca eaton and derek zang on 8/14/25.
//


#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

#include "../TokenEmbeddings/Matrix.h"

#include
#include

class Tokenizer {
public:
    // Public datasets so you can pass them explicitly like before if you want
    static const std::vector<char> SP;
    static const std::vector<std::string> suffixes;
    static const std::vector<std::string> prefixes;

    // Tokenization utilities
    static std::vector<char> CTE(const std::string& s);
    static std::vector<std::string> WTE(const std::string& s, std::vector<char> sp);
    static std::vector<std::string> STE(const std::string& s,
                                        const std::vector<char>& sp,
                                        const std::vector<std::string>& pre,
                                        const std::vector<std::string>& suff);

    // Encoding/decoding/embedding
    static std::vector<int> encodeTokens(std::vector<std::string> tokenVector, const std::string& inFilePath);
    static std::vector<std::string> decodeTokens(std::vector<int> tID, const std::string& inFilePath);
    static std::vector<std::vector<double>> embedTokenVector(std::vector<int> tID, const std::string& inFilePath);
    static std::vector<std::string> decodeEmbedToken(const std::vector<std::vector<double> >& tEmbed, const std::string &in);

    static Matrix embedToken(int tID, const std::string &in);

private:
    static std::string toLower(const std::string& s);
};

/// Stream operators (same behavior as your originals)
std::ostream& operator<<(std::ostream& os, const std::vector<char>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<int>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<double>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<double>>& vv);

#endif // TOKENIZER_H
