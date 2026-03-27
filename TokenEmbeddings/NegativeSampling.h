//
// Created by luca eaton on 8/28/25.
//

#ifndef NEGATIVESAMPLING_H
#define NEGATIVESAMPLING_H
#include <string>
#include <unordered_map>
#include <vector>
#include "../Token/Tokenizer.h"

class NegativeSampling {
    public:
    explicit NegativeSampling(Tokenizer& t, double power);
    [[nodiscard]] int getSample() const;
    [[nodiscard]] std::vector<int> vectorSample(int k) const;
private:

    std::unordered_map<std::string,double> freq;
    std::vector<int> table;
    int vocabSize;
    double power;
};



#endif //NEGATIVESAMPLING_H

