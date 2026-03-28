Please feel free to take a look at the REFACTORING.md file to see the optimizations I'm currently undergoing.

# Word2Vec++
I built this because I wanted to understand how computers understand the similarity between words, not just call random methods from a production library in python and call it a day.

# What does it do?
Trains vectors on the text8 corpus, being around 17 million tokens, using skip gram with negative sampling. Given a center word, the model learns to predict which words tend to appear nearby. 

Words that appear in similar context end up with, you guessed it, similar vectors. 

# Optimizations I Made

- Custom tokenizer using mmap and string_view to avoid heap allocations during vocab lookup
- Flat contiguous matrix storage for the embedding layers to keep memory access patterns cache-friendly
- Negative sampling distribution built from unigram frequencies raised to the 0.75 power, matching the original paper
- Replaced strtod(str to double) with fast_float and std::unordered_map with robin_hood after profiling showed they were bottlenecks

# Benchmarks
| Operation | Time |
|-----------|------|
| Load dataset (17M tokens) | 453ms |
| Load embeddings | 137ms |
| Load tokenizer | 4.33ms |

[Benchmark_LoadingDataset_3_27.pdf](https://github.com/user-attachments/files/26318614/Benchmark_LoadingDataset_3_27.pdf) - Output of C++ Benchmakr via Google Benchmark

Dataset loader is 1.77x faster than gensim's Text8Corpus (803ms) on the same machine.

[Benchmark_LoadingDataset_Python_3_27.txt](https://github.com/user-attachments/files/26318617/Benchmark_LoadingDataset_Python_3_27.txt) - Output of Python Benchmark



Link to the paper I used : https://arxiv.org/pdf/1310.4546
