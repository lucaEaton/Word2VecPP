# Word2Vec++

> Custom C++20 word‑embedding library that tokenizes raw text and trains word vectors with **Skip‑Gram + Negative Sampling** — no external ML frameworks.

## ✨ Highlights

* **From‑scratch C++** implementation (header‑only utilities where possible), focused on clarity and speed.
* **Tokenizer/BPE pipeline** to turn raw text into token IDs.
* **Skip‑Gram + Negative Sampling** training loop with fast vector ops.
* **No 3rd‑party ML deps** — just the C++ standard library, CMake, and a compiler.

## Repo structure

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

> Note on IDs: special/reserved tokens occupy **0–4**, so *your first vocab row is ID 5*.

## Build

### Requirements

* CMake ≥ 3.20
* A C++20 compiler (Clang 15+/Apple Clang on macOS, or GCC 11+)
* (Optional) CLion / VS Code CMake Tools

### macOS / Linux

```bash
# from repo root
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

This will generate one or more executables (see *Targets* printed by CMake). Common targets you’ll see in this repo include tokenization (e.g., `Tokenizer_BPE`) and training/evaluation mains (under `Word2Vec/`).

### Windows (MSVC)

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

## Quickstart

1. **Prepare data**

   * Put your corpus in `Files/` (example: `text8`).
   * Ensure `Files/Vocab.txt` exists: one token per line matching the tokenizer’s output. (IDs begin at 5.)

2. **Tokenize** (BPE or your chosen tokenizer)

   * Build and run the tokenizer target (often named `Tokenizer_BPE`).
   * Output is an integer‑ID sequence used by training.

3. **Train Skip‑Gram + Negative Sampling**

   * Build and run the training main under `Word2Vec/` (names can vary; look for a `main()` near `SkipGramModel.*`).
   * Typical hyper‑params you’ll see in this project:

     * `embedding_dim = 300`
     * `learning_rate ≈ 0.025`
     * `window_size ≈ 5–7`
     * `k_negatives ≈ 5–10`
     * `ns_exponent = 0.75` (unigram^0.75)

4. **Evaluate**

   * Build/run the `Evaluation` target to compute cosine similarity between pairs (e.g., *king* vs *queen*).

## How it works

* **Tokenizer/BPE** maps raw text ➜ tokens ➜ integer IDs (saved to `Files/`).
* **Model** (`SkipGramModel`) learns two embedding matrices: **input** and **output**.
* **Loss** = `−log σ(u·v)` for positive (center, context) pairs + `−Σ log σ(−u·v_neg)` for negatives.
* **Negative sampling** (`NegativeSampling`) draws k negatives from a unigram distribution raised to 0.75.
* **Optimization**: simple SGD on dot products using fast row‑spans from `Matrix`.

## Example code tips (from this repo)

* Use numerically stable helpers: `log_sigmoid`, `sigmoid`, `std::log1p` forms.
* Prefer **row spans** of the matrix to avoid copies (e.g., `rowSpan(id)`).
* If your vocab IDs start at 5, index the correct row with `id-5` wherever matrices are 0‑based.
* Keep negative samples **distinct** from the positive target; filter duplicates before scoring.

## Large files & Git LFS

GitHub warns about big files in `Files/` (e.g., `text8` ~95MB and `VocabEmbeddings.txt` ~66MB). Consider moving these to **Git LFS** or excluding them from history:

```bash
# track large artifacts
git lfs install
git lfs track "Files/text8" "Files/*.txt"

# (optional) rewrite history to purge large blobs
# ⚠️ Force‑push will rewrite the remote
pipx install git-filter-repo  # or python -m pip install git-filter-repo
git filter-repo --path Files/text8 --invert-paths
```

## Reproducing a simple run (pseudo‑CLI)

> Exact binaries/flags may differ — check the built targets printed by CMake.

```bash
# 1) tokenize corpus
./build/Tokenizer_BPE \
  --input Files/text8 \
  --out   Files/tokens.txt \
  --vocab Files/Vocab.txt

# 2) train skip‑gram
./build/word2vec_train \
  --tokens Files/tokens.txt \
  --vocab  Files/Vocab.txt \
  --dim 300 --window 7 --neg 7 --lr 0.025 \
  --epochs 15 --save Files/VocabEmbeddings.txt

# 3) evaluate
./build/evaluation --vocab Files/Vocab.txt --emb Files/VocabEmbeddings.txt \
  --pair king queen
```

## Troubleshooting

* **Cosine similarity seems capped (~0.3–0.4)**

  * Train longer; raise `epochs` or use dynamic LR decay.
  * Increase corpus size / quality; ensure subsampling and min‑count aren’t discarding too much.
  * Verify negatives are **not** including the true target; ensure gradients are applied to both input & output rows.
* **Span/row access compile errors**

  * Ensure `rowSpan` returns `std::span<double>` over contiguous storage and matches `const`/non‑`const` correctly.
* **Index off‑by‑5**

  * If vocab starts at ID 5, convert to 0‑based indices when addressing matrix rows.
* **Giant repo warnings**

  * Use Git LFS or `git filter-repo` to remove historical blobs (see *Large files & Git LFS* above).

## Roadmap

* [ ] Clean CLI for tokenization/training/eval with consistent flags
* [ ] Save/load binary model format; support gensim compatibility
* [ ] CBOW and/or Hierarchical Softmax option
* [ ] Checkpointing + LR schedule (warmup/decay)
* [ ] Unit tests for `Matrix`, sampling, and gradient updates

## License

MIT (see `LICENSE`)

## Citation

If this project helps your work, please star the repo and cite it as:

```
Eaton, L. (2025). Word2VecPP: From‑scratch Skip‑Gram + Negative Sampling in C++.
GitHub repository: https://github.com/lucaEaton/Word2VecPP
```
