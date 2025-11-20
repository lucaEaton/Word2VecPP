# Word2Vec++

 Custom C++word‑embedding library that tokenizes raw text and trains word vectors with **Skip‑Gram + Negative Sampling** —, all from scratch, no external ML frameworks.

```
Word2VecPP/
├─ Files/                     # Corpora, vocab, and (optional) pretrained embeddings
│  ├─ text8                   # Example training corpus (95MB)
│  ├─ Vocab.txt               # One token per line (IDs start at 5)
│  └─ VocabEmbeddings.txt     # (Optional) embeddings dump/checkpoint (~66MB)
├─ Token/                     # Tokenization & BPE utilities
│  ├─ Tokenizer.*
│  └─ ...
├─ TokenEmbeddings/           # Training algorithms & sampling
│  ├─ NegativeSampling.*
│  ├─ Matrix.*                # Lightweight matrix/row-span helpers
│  └─ ...
├─ Word2Vec/                  # Model, training loop, evaluation, CLI mains
│  ├─ SkipGramModel.*
│  ├─ Evaluation.*
│  └─ ...
├─ CMakeLists.txt             # Project build configuration
└─ README.md
```

Link to the paper I used : https://arxiv.org/pdf/1310.4546
