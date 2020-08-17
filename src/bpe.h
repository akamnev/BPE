#pragma once

#include <map>
#include <string>
#include <unordered_set>
#include "../third_party/flat_hash_map.h"

#include "utils.h"

namespace tokenizer {

enum OutputType { ID, SUBWORD };

struct WordCountStr {
    std::string word;
    uint64_t count;
};


Status train_bpe(const std::vector<WordCountStr> &word_count, int vocab_size, BpeConfig config, std::vector<int64_t>* model);

class BaseEncoder {
 public:
  BPEState bpe_state;
  flat_hash_map<uint32_t, uint32_t> id2char;
  flat_hash_map<uint32_t, std::vector<uint32_t>> recipe;
  flat_hash_map<std::string, uint32_t> reversed_recipe;
  flat_hash_map<uint64_t, int> rule2id;
  int n_threads;

  explicit BaseEncoder(BPEState bpe_state, int _n_threads);

  explicit BaseEncoder(const std::string &model_path, int n_threads, Status *ret_status);
  explicit BaseEncoder(const std::vector<int64_t> &model, int n_threads, Status *ret_status);

  void fill_from_state();

  Status encode_as_ids(const std::vector<std::string> &words,
          std::vector<std::vector<int>> *ids,
          std::vector<std::vector<int>> *ends=nullptr, double dropout_prob=0) const;

  Status encode_as_subwords(const std::vector<std::string> &words,
      std::vector<std::vector<std::string>> *subwords, double dropout_prob=0) const;

  Status id_to_subword(int id, std::string *subword) const;

  int subword_to_id(const std::string &token) const;

  Status decode(const std::vector<std::vector<int>> &ids,
                std::vector<std::string> *sentences,
                const std::unordered_set<int> *ignore_ids) const;

  Status decode(const std::vector<int> &ids, std::string *sentence, const std::unordered_set<int> *ignore_ids) const;

  int vocab_size() const;

  std::vector<std::string> vocabulary() const;

  void dump(const std::string &file_name);

 private:
    DecodeResult encode_token(const std::string &word_utf8,
                              const EncodingConfig &encoding_config,
                              OutputType output_type) const;

    Status encode_parallel(
      const std::vector<std::string> &words,
      const EncodingConfig &encoding_config, OutputType output_type,
      std::vector<DecodeResult> *decoder_results
  ) const;
};

}  // namespace tokenizer
