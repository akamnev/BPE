from libcpp.vector cimport vector
from libcpp.unordered_set cimport unordered_set
from libcpp.string cimport string
from libcpp cimport bool
from typing import Collection
from enum import IntEnum, auto


cdef extern from "../src/bpe.h" namespace "tokenizer":

    cdef cppclass Status:
        int code
        string message
        bool ok() const

    cdef cppclass SpecialTokens:
        int pad_id
        int mask_id
        int unk_id
        int bos_id
        int eos_id

    cdef cppclass BpeConfig:
        int min_token_count
        int n_threads
        SpecialTokens special_tokens

    cdef cppclass WordCountStr:
        string word
        int count

    Status train_bpe(const vector[WordCountStr]& data, int vocab_size, const BpeConfig& bpe_config, vector[long int]* model)

    cdef cppclass BaseEncoder:
        BaseEncoder(const vector[long int]& model, int n_threads, Status* status)

        Status encode_as_ids(const vector[string] &words, vector[vector[int]]* ids, vector[vector[int]]* ends, double dropout_prob) const
        Status encode_as_subwords(const vector[string]& words, vector[vector[string]]* subwords, double dropout_prob) const

        Status id_to_subword(int id, string* subword) const
        int subword_to_id(const string &subword) const

        Status decode(const vector[vector[int]]& ids, vector[string]* output, const unordered_set[int]* ignore_ids) const
        int vocab_size() const
        vector[string] vocabulary() const
        void dump(string file_name)


class OutputType(IntEnum):
    ID = auto()
    SUBWORD = auto()


cdef class BPE:
    cdef BaseEncoder* encoder

    def __dealloc__(self):
        del self.encoder

    def __init__(self, model, n_threads=-1):
        cdef Status status
        self.encoder = new BaseEncoder(model, n_threads, &status)
        if not status.ok():
            raise ValueError(status.message.decode())

    @staticmethod
    def train(data, vocab_size, min_token_count=0, n_threads=-1,
              pad_id=0, mask_id=1, unk_id=2, bos_id=3, eos_id=4):
        cdef BpeConfig bpe_config
        bpe_config.min_token_count = min_token_count
        bpe_config.n_threads = n_threads
        bpe_config.special_tokens.pad_id = pad_id
        bpe_config.special_tokens.mask_id = mask_id
        bpe_config.special_tokens.unk_id = unk_id
        bpe_config.special_tokens.bos_id = bos_id
        bpe_config.special_tokens.eos_id = eos_id

        cdef vector[WordCountStr] word_count
        cdef WordCountStr wc
        for d in data:
            wc.word = d[0].encode()
            wc.count = d[1]
            word_count.push_back(wc)

        cdef vector[long int] model
        cdef Status status = train_bpe(word_count, vocab_size, bpe_config, &model)
        if not status.ok():
            raise ValueError(status.message.decode())
        return model

    def dump(self, file_name):
        return self.encoder.dump(file_name.encode())

    def encode(self, words, output_type, dropout_prob=0.0):
        cdef vector[string] w
        cdef vector[vector[string]] ret_subwords
        cdef vector[vector[int]] ret_ids
        cdef vector[vector[int]] ret_ends
        cdef Status status
        if dropout_prob < 0 or dropout_prob > 1:
            raise ValueError("dropout_prob value must be in the range [0, 1]. Current value of dropout_prob = " + str(dropout_prob))
        if isinstance(words, str):
            w = [words.encode()]
        elif isinstance(words, list) or isinstance(words, tuple):
            w = [x.encode() for x in words]
        else:
            raise TypeError(f"type of variable words must be str, list, or tuple but {type(words)} type is given")
        if output_type == OutputType.ID:
            status = self.encoder.encode_as_ids(w, &ret_ids, &ret_ends, dropout_prob)
            if not status.ok():
                raise ValueError(status.message.decode())
            assert len(ret_ids) == len(ret_ends)
            if len(ret_ids) == 1:
                return ret_ids[0], ret_ends[0]
            return ret_ids, ret_ends
        elif output_type == OutputType.SUBWORD:
            status = self.encoder.encode_as_subwords(w, &ret_subwords, dropout_prob)
            if not status.ok():
                raise ValueError(status.message.decode())
            if len(ret_subwords) == 1:
                return [piece.decode() for piece in ret_subwords[0]]
            return [[piece.decode() for piece in word] for word in ret_subwords]
        else:
            raise ValueError('output_type must be equal to "OutputType.ID" or '
                             '"OutputType.SUBWORD"')

    def subword_to_id(self, subword):
        return self.encoder.subword_to_id(subword.encode())

    def id_to_subword(self, id):
        cdef string subword
        cdef Status status = self.encoder.id_to_subword(id, &subword)
        if not status.ok():
            raise ValueError(status.message.decode())
        return subword.decode()

    def decode(self, ids, ignore_ids):

        if not isinstance(ids, list):
            raise TypeError(
                "{} is not a list instance".format(type(ids))
            )

        if not isinstance(ignore_ids, Collection) and ignore_ids is not None:
            raise TypeError(
                "{} is not a Collection instance".format(type(ignore_ids))
            )

        if len(ids) > 0 and isinstance(ids[0], int):
            ids = [ids]
        if ignore_ids is None:
            ignore_ids = set()

        cdef vector[string] sentences
        cdef unordered_set[int] c_ignore_ids = unordered_set[int](ignore_ids)
        cdef Status status = self.encoder.decode(ids, &sentences, &c_ignore_ids)
        if not status.ok():
            raise ValueError(status.message.decode())
        return [sentence.decode() for sentence in sentences]

    def vocab_size(self):
        return self.encoder.vocab_size()

    def vocab(self):
        cdef vector[string] vocab = self.encoder.vocabulary()
        return [token.decode() for token in vocab]
