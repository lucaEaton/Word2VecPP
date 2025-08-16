//
// Created by luca eaton and derek zang on 8/15/25.
//


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <unordered_set>

//Creates all cases to be lowercased
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

//Normalizes our input to not consider special characters
std::string removeSP(const std::string &word) {
    std::string result;
    for (char c : word) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            result.push_back(std::tolower(c));
        }
    }
    return result;
}

//Implements Substrings to be stored as tokenIDs
//Implemented from BPE.cpp
std::vector<std::string> subStrings(const std::string& word) {
    static std::vector<std::string> suff = {
        "s", "es", "ed", "ing", "ly", "er", "or", "ion", "tion", "ation", "ition",
        "ible", "able", "al", "ial", "y", "ness", "ity", "ment", "ic", "ous", "eous", "ious",
        "en", "ify", "ise", "ize", "ward", "wise", "hood", "ship", "dom", "ful", "less"
    };
    static std::vector<std::string> pre = {
        "un", "re", "in", "im", "ir", "il", "dis", "en", "em",
        "non", "over", "mis", "sub", "pre", "inter", "fore", "de",
        "trans", "super", "semi", "anti", "mid", "under", "over",
    };

    for (auto& p : pre) {
        if (word.rfind(p, 0) == 0 && word.size() > p.size()) {
            return {p, "##" + word.substr(p.size())};
        }
    }

    for (auto& s : suff) {
        if (word.size() > s.size() &&
            word.compare(word.size()-s.size(), s.size(), s) == 0) {
            return {word.substr(0, word.size()-s.size()), "##" + s};
            }
    }

    return {word};
}

//Returns a set of strings from a provided file
std::set<std::string> storeVocab(const std::string &inFile)
{
    std::set<std::string> vocab;
    std::ifstream file(inFile);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return vocab;
    }

    std::string word;
    while (file >> word) {
        std::string noSP = removeSP(word);
        if (!noSP.empty()) {
            vocab.insert(noSP);
        }
    }

    file.close();
    return vocab;
}

//Writes our set of string from storeVocab to a file
void writeVocab(std::set<std::string> v, const std::string &tgtFile) {
    std::ofstream outFile(tgtFile);
    const std::set<std::string>& vocab = v;
    if (!outFile.is_open()) {
        std::cerr << "Could not open the output file!" << std::endl;
    }
    std::unordered_set<std::string> seen;
    int id = 5;
    for (const auto& word : vocab) {
        auto sub = subStrings(word);
        for (const auto& s : sub) {
            if (seen.insert(s).second) {
                // avoid duplicates
                outFile << id << ":" << s << "\n";
                id++;
            }
        }
    }
    std::cout << "Created ID Tokens Successfully."<< std::endl;
    outFile.close();
}

std::ostream& operator<<(std::ostream &os, const std::set<std::string> &vocab) {
    for (const auto& word : vocab) {
        os << word << '\n';
    }
    return os;
}


//main function
int main() {
    writeVocab(storeVocab("Files/Words"),"Files/Vocab" );
}