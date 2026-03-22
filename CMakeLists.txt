cmake_minimum_required(VERSION 3.20)
project(Word2VecPP)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Word2VecPP
        Word2Vec/Word2Vec.cpp
        Word2Vec/Dataset.cpp
        Word2Vec/Evaluation.cpp
        Token/Tokenizer.cpp
        Token/Tokenizer.h
        Files/FileReader.cpp
        TokenEmbeddings/SkipGramModel.cpp
        TokenEmbeddings/NegativeSampling.cpp
        TokenEmbeddings/Matrix.cpp
        TokenEmbeddings/startingEmbeddings.cpp
)
