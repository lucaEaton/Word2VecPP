Activing working towards optimizating. This document tracks known performance 
bottlenecks and correctness issues identified during post-implementation review.

## Issue 1 — O(N) File I/O on Every Lookup✅
**Problem:** Every call to `encodeTokens` or `decodeTokens` opens and scans the 
entire `Vocab.txt` file from disk to build a local `unordered_map`, uses it once, 
then throws it away. With a ~71k token vocabulary, this means a full file scan on 
every invocation — including inside tight loops like `Dataset::vocabToID`, which 
calls `encodeTokens` once per sentence over a 17M token corpus.

**New Concept** Build the `word -> id` and `id -> word` maps once at startup and store them 
as static structures. All lookups become O(1) `unordered_map` accesses with zero I/O overhead.


## Issue 2 — O(N) File Scan Per Embedding Lookup in Evaluation
**Problem:** `vectorRetriever` scans `VocabEmbeddingsTrained.txt` line by line on 
every call to find a single token's embedding. `nearestNeighbors` calls it once per 
token in the vocabulary (~71k times), making the function effectively O(N²) in 
file, as it reads for a single nearest-neighbor query.

**New Concept** Load all embeddings into memory once — either as a flat matrix or something like an
`unordered_map<int, vector<double>>` — and serve all lookups from RAM. 
`SkipGramModel` already does this correctly with `embeddingLayerIn`; 
Hoever I believe `Evaluation` should follow the same pattern.


## Issue 3 —Incorrect ID Assignment in Negative Sampling Table
**Problem:** The sampling table is built by iterating over an `unordered_map` 
and assigning IDs sequentially via `current_id++`. Because `unordered_map` 
iteration order is not guaranteed, the word-to-ID mapping baked into the table 
is different on every run. What I've noticed is the IDs do not correspond to the actual vocab IDs 
in `Vocab.txt`, meaning negative samples are drawn from the wrong distribution 
and the model is training against incorrect targets.

**Concept** Load the vocabulary with its canonical IDs from `Vocab.txt` first, then 
build the frequency-weighted table using those IDs directly, the same IDs the 
rest of the pipeline uses.

