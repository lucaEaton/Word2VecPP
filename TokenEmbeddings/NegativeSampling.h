//
// Created by luca eaton on 8/28/25.
//

#ifndef NEGATIVESAMPLING_H
#define NEGATIVESAMPLING_H
#include <string>
#include <unordered_map>
#include <unordered_set>

class NegativeSampling {
    public:

    NegativeSampling(const std::string &inFile, double power);
    int getSample() const;
    std::vector<int> vectorSample(int k) const;
private:

    std::unordered_map<std::string,double> freq;
    std::vector<int> table;
    int vocabSize;
    double power;
};



#endif //NEGATIVESAMPLING_H

