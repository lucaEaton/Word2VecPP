//
// Created by luca eaton on 8/15/25.
//


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>

//Read Word by word of the text file
//Check if word exists within the outFile
//If not store within a unique ID
//Continue until we have all unique ID and words


std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// normalize our input to not consider special characters
std::set<std::string> storeVocab(const std::string &inFile)
{
    std::set<std::string> vocab;
    std::ifstream file(inFile);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
    }
    std::string word;
    while (file >> word) {
        vocab.insert(toLower(word));
    }
    file.close();
    return vocab;
}

void writeVocab(std::set<std::string> vocab, const std::string &tgtFile) {
    std::ofstream outFile(tgtFile);
    if (!outFile.is_open()) {
        std::cerr << "Could not open the output file!" << std::endl;
    }
    int id = 5;
    for (const auto& word : vocab) {
        outFile << id << ":" << word << "\n";
        id++;
    }
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
    writeVocab(storeVocab("Files/Utilitarianism"),"Files/Vocab" );
}