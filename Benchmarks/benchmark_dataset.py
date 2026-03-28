import timeit
from gensim.models.word2vec import Text8Corpus

def load():
    return list(Text8Corpus("Files/text8"))

times = timeit.repeat(load, repeat=5, number=1)
print("utilising gensim in order to benchmark my custom c++ dataset loader against an \"industry standard\" loader")
print(f"avg time: {sum(times)/len(times)*1000:.0f}ms")
# prints 1701 as it the way text8 is formated and how gensim does chunk using 10,000 words, not 1000 as we did.
print(f"# of sentences: {len(load())}")