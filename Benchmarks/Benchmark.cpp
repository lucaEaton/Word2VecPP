//
// Created by luca eaton on 3/27/26.
//

#include "Benchmark.h"
#include <benchmark/benchmark.h>
#include "../Word2Vec/Dataset.h"
#include "../Token/Tokenizer.h"
#include "../Files/FileReader.h"

static void BM_LoadDataset(benchmark::State& state) {
    Tokenizer t;
    // initialize your tokenizer here the same way you do in main
    for (auto _ : state) {
        auto dataset = Dataset::loadDataset("../Files/text8", t);
        benchmark::DoNotOptimize(dataset);
    }
}
static void BM_LoadEmbeddings(benchmark::State& state) {
    for (auto _ : state) {
        auto M = FileReader::loadEmbeddingsToMatrix(71294, 300);
        benchmark::DoNotOptimize(M);
    }
}

static void BM_LoadTokenizer(benchmark::State& state) {
    Tokenizer t;
    for (auto _ : state) {
        t.loadMapV2();
    }
}

BENCHMARK(BM_LoadDataset)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_LoadEmbeddings)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_LoadTokenizer)->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();