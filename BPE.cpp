//
// Created by luca eaton on 8/14/25.
//

#include "BPE.h"
#include <iostream>
#include <vector>
#include <ranges>
#include <string>


static std::vector<char> SP = { '!', '.', '_', ',', '&', '@' };

static std::vector<std::string> suffixes = {
    "s", "es", "ed", "ing", "ly", "er", "or", "ion", "tion", "ation", "ition",
    "ible", "able", "al", "ial", "y", "ness", "ity", "ment", "ic", "ous", "eous", "ious",
    "en", "ify", "ise", "ize", "ward", "wise", "hood", "ship", "dom", "ful", "less"
};

static std::vector<std::string> prefixes = {
    "un", "re", "in", "im", "ir", "il", "dis", "en", "em",
    "non", "over", "mis", "sub", "pre", "inter", "fore", "de",
    "trans", "super", "semi", "anti", "mid", "under", "over",
};

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::vector<char> CTE (const std::string &s) {
    std::vector<char> v(s.begin(),s.end());
    return v;
}

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
std::vector<std::string> STE(const std::string& s,
                             const std::vector<char>& sp,
                             const std::vector<std::string> &pre,
                             const std::vector<std::string> &suff)
{
    // "Untouchable is a pretty good TV show."
    // WTE(sentence) -> ["Untouchable", "is", ...]
    // Untouchable is a word that has a prefix and suffix. (Loop through all entries in WTE(sentence)_)->
    // "Untouchable", ["un##" ,"touch", "##able] gets swapped with "Untouchable" in the original WTE output.
    // We're essentially modifying the entries of WTE
    // WTE is O(n). We have another loop so O(n). We're at "untouchable."

    std::vector<std::string> vP = WTE(s, sp);
    std::vector<std::string> brk;
    // vP = ["Untouchable", "is", "a", "pretty", "good", "TV", "show", "."]
    // vP[0] = "Untouchable" -- check first 4, includes pre, use substring to catch the pre, add "##" and push
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


std::ostream &operator<<(std::ostream &os, const std::vector<char> &v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << "\"" << v[i] << "\"";
        if (i + 1 < v.size()) os << ",";
    }
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << "\"" << v[i] << "\"";
        if (i + 1 < v.size()) os << ",";
    }
    os << "]";
    return os;
}


int main() {
    std::cout << STE("Untouchable is a great tv show! ", SP, prefixes, suffixes) << std::endl;
}




