//
// Created by luca eaton and derek zang on 8/14/25.
//

#include "BPE.h"
#include <iostream>
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

//Changes a give string to lowerCase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

//Char tokenization method
//Given a string/sentence, creates a vector of characters on the given string
std::vector<char> CTE (const std::string &s) {
    std::vector<char> v(s.begin(),s.end());
    return v;
}

//Word tokenization method
//Given a string/sentence, creates a vector of "words" on the given string/sentence
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



//substring tokenization method
//Given a string/sentence, creates a vector of "words" including prefixes and suffixes on the given string/sentence
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
            if (!currWordPre.empty()) brk.push_back("##" + currWordPre);
            std::size_t start = currWordPre.size();
            std::size_t endTrim = currWordSuff.size();
            std::size_t midLen = tgt.size() - start - endTrim;
            if (midLen > 0) brk.push_back(tgt.substr(start, midLen));
            if (!currWordSuff.empty()) brk.push_back(currWordSuff + "##");
        } else {
            brk.push_back(tgt);
        }
    }
    return brk;
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

//Notes - Case Handling in encoding
// NEXT STEP: CONVERT TOKENS INTO IDS AND REMAP THEM BACK INTO WORDS
//






//Debug Main Function
int main() {
    //std::cout << STE("", SP, prefixes, suffixes) << std::endl;

}




