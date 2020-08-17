import os
import pybpe


with open(os.path.join('..', 'data', 'train_text.txt'), 'r') as fp:
    text = fp.read()

word_count = {}
for word in text.split():
    try:
        word_count[word] += 1
    except KeyError:
        word_count[word] = 1
word_count = sorted([(k, v) for k, v in word_count.items()], key=lambda x: x[1])

vocab = 1000
model = pybpe.BPE.train(word_count, vocab, min_token_count=100)

bpe = pybpe.BPE(model)

print('vocab size: ', bpe.vocab_size())
print('vocab', bpe.vocab())
words = ["на", "わからわからПривет", "Привет", "мир", "!", "Украина"]
for w in words:
    sw = bpe.encode(w, pybpe.OutputType.SUBWORD)
    print(sw)
    sw = bpe.encode(w, pybpe.OutputType.ID)
    print(sw)
print('=' * 80)
sw = bpe.encode(words, pybpe.OutputType.SUBWORD)
print(sw)
sw = bpe.encode(words, pybpe.OutputType.ID)
print(sw)
