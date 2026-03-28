cmake_minimum_required(VERSION 3.20)
project(Word2VecPP)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(fast_float GIT_REPOSITORY https://github.com/fastfloat/fast_float.git GIT_TAG main)
FetchContent_MakeAvailable(fast_float)

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

target_link_libraries(Word2VecPP fast_float)

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
FetchContent_Declare(Benchmarks GIT_REPOSITORY https://github.com/google/benchmark.git GIT_TAG main)
FetchContent_MakeAvailable(Benchmarks)

add_executable(Word2VecBenchmark
        Benchmarks/benchmark.cpp
        Word2Vec/Dataset.cpp
        Token/Tokenizer.cpp
        Files/FileReader.cpp
        TokenEmbeddings/Matrix.cpp
)
target_link_libraries(Word2VecBenchmark benchmark::benchmark fast_float)