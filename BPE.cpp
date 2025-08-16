//
// Created by luca eaton and derek zang on 8/14/25.
//

#include "BPE.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ranges>
#include <string>


//Special Character Dataset
static std::vector<char> SP = { '!', '.', '_', ',', '&', '@' };

//Suffix Dataset
static std::vector<std::string> suffixes = {
    "s", "es", "ed", "ing", "ly", "er", "or", "ion", "tion", "ation", "ition",
    "ible", "able", "al", "ial", "y", "ness", "ity", "ment", "ic", "ous", "eous", "ious",
    "en", "ify", "ise", "ize", "ward", "wise", "hood", "ship", "dom", "ful", "less"
};

//Prefix Dataset
static std::vector<std::string> prefixes = {
    "un", "re", "in", "im", "ir", "il", "dis", "en", "em",
    "non", "over", "mis", "sub", "pre", "inter", "fore", "de",
    "trans", "super", "semi", "anti", "mid", "under", "over",
};

/**
 * toLower Function
 *
 * @param str provided word
 * @return the word in all lower case
 */
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

/**
 * Character tokenization method
 *
 * @param s Word/Sentence
 *
 * Given a string/sentence, creates a vector of characters on the given string
 *
 * @return A vector of chars on the provided word/sentence
 */
std::vector<char> CTE (const std::string &s) {
    std::vector<char> v(s.begin(),s.end());
    return v;
}


/**
 * Word tokenization method
 *
 * @param s Word/Sentence
 * @param sp Special char case
 *
 * Given a string/sentence, creates a vector of "words" on the given string/sentence
 *
 * @return a vector of strings, primarily the "words"
 */
std::vector<std::string> WTE(const std::string& s, std::vector<char> sp) {
    std::vector<std::string> v;
    std::string pt;
    for (char c  : s) {
        if (std::find(sp.begin(), sp.end(), c) != sp.end()) {
            v.push_back(pt);
            pt.clear();
        }
        if (c == ' ') {
            if (!pt.empty()) {
                v.push_back(pt);
                pt.clear();
            }
        } else {
            pt += c;
        }
    }
    if (!pt.empty()) {
        v.push_back(pt);
    }
    return v;
}

/**
 * Substring tokenization method
 *
 * @param s Word/Sentence
 * @param sp Special char cases
 * @param pre Prefix cases
 * @param suff Suffix cases
 *
 * Given a string/sentence, creates a vector of "words" including prefixes and suffixes on the given string/sentence
 *
 * @return vector of substrings
 */
std::vector<std::string> STE(const std::string& s,
                             const std::vector<char>& sp,
                             const std::vector<std::string> &pre,
                             const std::vector<std::string> &suff)
{
    std::vector<std::string> vP = WTE(s, sp);
    std::vector<std::string> brk;
    for (const auto& tgt : vP) {
        const std::string lower = toLower(tgt);
        if (tgt.size() <= 4) {
            brk.push_back(tgt);
            continue;
        }
        bool matched = false;
        std::string currWordPre, currWordSuff;
        //For Derek -- Checks for Prefix
        auto preIt = std::find_if(pre.begin(), pre.end(), [&](const std::string& p){
            return lower.rfind(p, 0) == 0;
        });
        if (preIt != pre.end()) {
            currWordPre = *preIt;
            matched = true;
        }
        //For Derek -- Checks for Suffix
        auto suffIt = std::find_if(suff.begin(), suff.end(), [&](const std::string& x){
            if (x.size() > lower.size()) return false;
            return lower.compare(lower.size() - x.size(), x.size(), x) == 0;
        });
        if (suffIt != suff.end()) {
            currWordSuff = *suffIt;
            matched = true;
        }
        //For Derek -- Puts the pieces together
        if (matched) {
            if (!currWordPre.empty()) brk.push_back(currWordPre + "##");
            std::size_t start = currWordPre.size();
            std::size_t endTrim = currWordSuff.size();
            std::size_t midLen = tgt.size() - start - endTrim;
            if (midLen > 0) brk.push_back(tgt.substr(start, midLen));
            if (!currWordSuff.empty()) brk.push_back("##" + currWordSuff);
        } else {
            brk.push_back(tgt);
        }
    }
    return brk;
}

/*
 *Read "Vocab" file
 *Take returned vector from STE
 *convert provided words into tokens the token_ids
 *
 *------------
 *My Approach
 *------------
 *
 *Loop through file
 *Check if indexed token exists in a line from within the file.
 *Use substr() to gather the # and parse to an int
 *
 *CURRENT ERRORS:
 *
 *Doesn't fully replace the token with the given tokenID, it returns the tokenIDs sorted
 *Due to it reading the file line by line, possibly it jus takes the first one it sees in this case
 *"##ing" as it is (45) and pushes it to the top, same for the rest.
 */
std::vector<int> encodeTokens(std::vector<std::string> tokenVector, const std::string& in) {
    std::vector<std::string> tokens = tokenVector;
    std::ifstream inFile(in);
    std::vector<int> tokenID;
    if (!inFile.is_open()) {
        std::cerr << "Could not open file: " << std::endl;
        return std::vector<int>(false);
    }

    std::cout << "I passed" << std::endl;

    std::string line;
    while (std::getline(inFile, line)) {
        std::cout << "Line: [" << line << "]" << std::endl;
        std::string currentTokenID = line.substr(0, line.find(':'));
        std::string token = line.substr(line.find(':') + 1);

        for(const auto& currentToken : tokens) {
            if (currentToken == token) {
                tokenID.push_back(std::stoi(currentTokenID));
            }
        }
    }


    return tokenID;
}

//Printing vector of chars
std::ostream &operator<<(std::ostream &os, const std::vector<char> &v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << "\"" << v[i] << "\"";
        if (i + 1 < v.size()) os << ",";
    }
    os << "]";
    return os;
}

//Printing vector of strings
std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << "\"" << v[i] << "\"";
        if (i + 1 < v.size()) os << ",";
    }
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<int> &v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << "\"" << v[i] << "\"";
        if (i + 1 < v.size()) os << ",";
    }
    os << "]";
    return os;
}

//Debug Main Function
int main() {
    std::string promptedText = "Amazing show we got here! ";
    std::vector<std::string> tokenizedText =  STE(promptedText, SP, prefixes, suffixes);

    std::cout << tokenizedText << std::endl;

    std:std::vector<int> tokenIDs = encodeTokens(tokenizedText, "Files/Vocab");

    std::cout << tokenIDs << std::endl;
}




