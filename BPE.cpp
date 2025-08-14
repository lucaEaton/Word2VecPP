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



std::vector<char> CTE (const std::string s) {
    std::vector<char> v(s.begin(),s.end());
    return v;
}

std::vector<std::string> WTE(const std::string s, std::vector<char> sp) {
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
std::vector<std::string> STE(const std::string s,
                             const std::vector<char> sp,
                             const std::vector<std::string> pre,
                             const std::vector<std::string> suff)
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
    for (size_t i = 0; i < vP.size(); i++) {
        std::string tgt = vP[i];
        std::string pref;
        bool matched = false;
        if (tgt.size() > 4) {
            for (size_t j = 0; j < tgt.size(); j++) {
                std::string currWordPre  = tgt.substr(0, 5);
                std::string currWordSuff = tgt.substr(tgt.size() - 5);

                auto it  = std::find(pre.begin(),  pre.end(),  currWordPre);
                auto it2 = std::find(suff.begin(), suff.end(), currWordSuff);

                if (it != pre.end()) {
                    const std::string& preFix = *it;

                    brk.push_back("##" + preFix);
                    brk.push_back(tgt.substr(preFix.size()));
                    pref = preFix;
                    matched = true;
                }
                if (it2 != suff.end()) {
                    const std::string& suffFix = *it2;

                    if (matched) {
                        std::string middle = tgt.substr(pref.size(),
                            tgt.size() - pref.size() - suffFix.size());
                        brk.back() = middle;
                    } else {
                        std::string stem = tgt.substr(0, tgt.size() - suffFix.size());
                        brk.push_back(stem);
                    }
                    brk.push_back(suffFix + "##");
                    matched = true;
                }
            }
        }else {
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
    std::cout << STE("Untouchable is a pretty good TV show.", SP,suffixes,prefixes) << std::endl;
}





