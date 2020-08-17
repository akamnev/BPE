#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "../third_party/flat_hash_map.h"

namespace tokenizer {

struct BPE_Rule {
  // x + y -> z
  uint32_t x{0};
  uint32_t y{0};
  uint32_t z{0};

  BPE_Rule() = default;

  BPE_Rule(uint32_t x, uint32_t y, uint32_t z);

  bool operator==(const BPE_Rule &other) const;
};

struct Status {
    int code{0};
    std::string message;
    Status() = default;
    Status(int code, std::string message);

    const std::string &error_message() const;
    bool ok() const;
};

struct SpecialTokens {
    static const int max_id_value = 9;

    static const std::string PAD_TOKEN;
    static const std::string MASK_TOKEN;
    static const std::string UNK_TOKEN;
    static const std::string BOS_TOKEN;
    static const std::string EOS_TOKEN;
    static const std::string RESERVED_TOKEN_5;
    static const std::string RESERVED_TOKEN_6;
    static const std::string RESERVED_TOKEN_7;
    static const std::string RESERVED_TOKEN_8;
    static const std::string RESERVED_TOKEN_9;

    int pad_id = 0;
    int mask_id = 1;
    int unk_id = 2;
    int bos_id = 3;
    int eos_id = 4;
    int reserved_5 = -1;
    int reserved_6 = -1;
    int reserved_7 = -1;
    int reserved_8 = -1;
    int reserved_9 = -1;

    SpecialTokens() = default;
    explicit SpecialTokens(int unk_id, int pad_id=0, int mask_id=1, int bos_id=3, int eos_id=4,
          int reserved_5=-1, int reserved_6=-1, int reserved_7=-1, int reserved_8=-1, int reserved_9=-1);

    void dump(std::ofstream &fout) const;
    void load(std::ifstream &fin);

    std::vector<int64_t> serialize() const;
    Status deserialize(std::vector<int64_t>::const_iterator begin, std::vector<int64_t>::const_iterator end);
    Status deserialize(const std::vector<int64_t>& data);

    Status check() const;

    uint32_t max_id() const;

    bool taken_id(int id) const;

    uint64_t n_special_tokens() const;

    Status id_to_subword(int id, std::string* subword) const;
    int subword_to_id(const std::string& word) const;
};

struct BpeConfig {
  int min_token_count = 0;
  int n_threads = 0;
  SpecialTokens special_tokens;

  BpeConfig() = default;

  BpeConfig(int n_threads, int min_token_count, const SpecialTokens &special_tokens);
};

struct BPEState {
  tokenizer::flat_hash_map<uint32_t, uint32_t> char2id;
  std::vector<BPE_Rule> rules;
  tokenizer::flat_hash_set<uint32_t> vocab;
  SpecialTokens special_tokens;

  void dump(const std::string &file_name);

  std::vector<int64_t> serialize();

  Status load(const std::string &file_name);

  Status deserialize(std::vector<int64_t> data);
};

struct DecodeResult {
  std::vector<int> ids;
  std::vector<std::string> pieces;
  std::vector<int> ends;
};

struct EncodingConfig {
  double dropout_prob;
};

bool is_space(uint32_t ch);

}  // namespace tokenizer
