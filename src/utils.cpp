#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

namespace tokenizer {
using std::string;
using std::vector;

const std::string SpecialTokens::PAD_TOKEN = "<pad>";
const std::string SpecialTokens::MASK_TOKEN = "<mask>";
const std::string SpecialTokens::UNK_TOKEN = "<unk>";
const std::string SpecialTokens::BOS_TOKEN = "<bos>";
const std::string SpecialTokens::EOS_TOKEN = "<eos>";
const std::string SpecialTokens::RESERVED_TOKEN_5 = "<res_5>";
const std::string SpecialTokens::RESERVED_TOKEN_6 = "<res_6>";
const std::string SpecialTokens::RESERVED_TOKEN_7 = "<res_7>";
const std::string SpecialTokens::RESERVED_TOKEN_8 = "<res_8>";
const std::string SpecialTokens::RESERVED_TOKEN_9 = "<res_9>";


SpecialTokens::SpecialTokens(int unk_id, int pad_id, int mask_id, int bos_id, int eos_id,
                             int reserved_5, int reserved_6, int reserved_7, int reserved_8, int reserved_9)
        : pad_id(pad_id), mask_id(mask_id), unk_id(unk_id), bos_id(bos_id), eos_id(eos_id),
          reserved_5(reserved_5), reserved_6(reserved_6), reserved_7(reserved_7), reserved_8(reserved_8),
          reserved_9(reserved_9) {}

void SpecialTokens::dump(std::ofstream &fout) const {
    fout << pad_id << std::endl;
    fout << mask_id << std::endl;
    fout << unk_id << std::endl;
    fout << bos_id << std::endl;
    fout << eos_id << std::endl;
    fout << reserved_5 << std::endl;
    fout << reserved_6 << std::endl;
    fout << reserved_7 << std::endl;
    fout << reserved_8 << std::endl;
    fout << reserved_9 << std::endl;
}

void SpecialTokens::load(std::ifstream &fin) {
    fin >> pad_id;
    fin >> mask_id;
    fin >> unk_id;
    fin >> bos_id;
    fin >> eos_id;
    fin >> reserved_5;
    fin >> reserved_6;
    fin >> reserved_7;
    fin >> reserved_8;
    fin >> reserved_9;
}

vector<int64_t> SpecialTokens::serialize() const {
    vector<int64_t> data = {pad_id, mask_id, unk_id, bos_id, eos_id,
                            reserved_5, reserved_6, reserved_7,
                            reserved_8, reserved_9};
    return data;
}

Status SpecialTokens::deserialize(std::vector<int64_t>::const_iterator begin,
        std::vector<int64_t>::const_iterator end) {
    if(begin != end) {
        pad_id = *begin;
        ++begin;
    }
    if(begin != end) {
        mask_id = *begin;
        ++begin;
    }
    if(begin != end) {
        unk_id = *begin;
        ++begin;
    }
    if(begin != end) {
        bos_id = *begin;
        ++begin;
    }
    if(begin != end) {
        eos_id = *begin;
        ++begin;
    }
    if(begin != end) {
        reserved_5 = *begin;
        ++begin;
    }
    if(begin != end) {
        reserved_6 = *begin;
        ++begin;
    }
    if(begin != end) {
        reserved_7 = *begin;
        ++begin;
    }
    if(begin != end) {
        reserved_8 = *begin;
        ++begin;
    }
    if(begin != end) {
        reserved_9 = *begin;
        ++begin;
    }
    return Status();
}


Status SpecialTokens::deserialize(const std::vector<int64_t>& data) {
    return deserialize(data.begin(), data.end());
}

Status SpecialTokens::check() const {
    if (pad_id < -1 || pad_id > max_id_value) {
        return Status(1, "pad_id: must be in the range [-1, max_id_value]. Current value of pad_id = "
        + std::to_string(unk_id));
    }
    if (mask_id < -1 || mask_id > max_id_value) {
        return Status(1, "mask_id: must be in the range [-1, max_id_value]. Current value of mask_id = "
                         + std::to_string(unk_id));
    }
    if (unk_id < 0 || unk_id > max_id_value) {
        return Status(1, "unk_id: must be in the range [0, max_id_value]. Current value of unk_id = "
                         + std::to_string(unk_id));
    }
    if (bos_id < -1 || bos_id > max_id_value) {
        return Status(1, "bos_id: must be in the range [-1, max_id_value]. Current value of bos_id = "
                         + std::to_string(unk_id));
    }
    if (eos_id < -1 || eos_id > max_id_value) {
        return Status(1, "eos_id: must be in the range [-1, max_id_value]. Current value of eos_id = "
                         + std::to_string(unk_id));
    }
    if (reserved_5 < -1 || reserved_5 > max_id_value) {
        return Status(1, "reserved_5: must be in the range [-1, max_id_value]. Current value of reserved_5 = "
                         + std::to_string(unk_id));
    }
    if (reserved_6 < -1 || reserved_6 > max_id_value) {
        return Status(1, "reserved_6: must be in the range [-1, max_id_value]. Current value of reserved_6 = "
                         + std::to_string(unk_id));
    }
    if (reserved_7 < -1 || reserved_7 > max_id_value) {
        return Status(1, "reserved_7: must be in the range [-1, max_id_value]. Current value of reserved_7 = "
                         + std::to_string(unk_id));
    }
    if (reserved_8 < -1 || reserved_8 > max_id_value) {
        return Status(1, "reserved_8: must be in the range [-1, max_id_value]. Current value of reserved_8 = "
                         + std::to_string(unk_id));
    }
    if (reserved_9 < -1 || reserved_9 > max_id_value) {
        return Status(1, "reserved_9: must be in the range [-1, max_id_value]. Current value of reserved_9 = "
                         + std::to_string(unk_id));
    }

    flat_hash_set<int> ids;
    uint64_t cnt_add = 0;
    if (pad_id != -1) {
        ids.insert(pad_id);
        cnt_add++;
    }
    if (mask_id != -1) {
        ids.insert(mask_id);
        cnt_add++;
    }
    if (bos_id != -1) {
        ids.insert(bos_id);
        cnt_add++;
    }
    if (eos_id != -1) {
        ids.insert(eos_id);
        cnt_add++;
    }
    if (reserved_5 != -1) {
        ids.insert(reserved_5);
        cnt_add++;
    }
    if (reserved_6 != -1) {
        ids.insert(reserved_6);
        cnt_add++;
    }
    if (reserved_7 != -1) {
        ids.insert(reserved_7);
        cnt_add++;
    }
    if (reserved_8 != -1) {
        ids.insert(reserved_8);
        cnt_add++;
    }
    if (reserved_9 != -1) {
        ids.insert(reserved_9);
        cnt_add++;
    }
    ids.insert(unk_id);
    cnt_add++;
    if (ids.size() != cnt_add) {
        return Status(1, "All ids of special tokens must be different.");
    }
    return Status();
}


uint32_t SpecialTokens::max_id() const {
  int ret = max_id_value;
  ret = std::max(ret, pad_id);
  ret = std::max(ret, mask_id);
  ret = std::max(ret, unk_id);
  ret = std::max(ret, bos_id);
  ret = std::max(ret, eos_id);
  ret = std::max(ret, reserved_5);
  ret = std::max(ret, reserved_6);
  ret = std::max(ret, reserved_7);
  ret = std::max(ret, reserved_8);
  ret = std::max(ret, reserved_9);
  return ret;
}

bool SpecialTokens::taken_id(int id) const {
  return id == pad_id || id == mask_id || id == unk_id || id == bos_id || id == eos_id ||
    id == reserved_5 || id == reserved_6 || id == reserved_7 || id == reserved_8 || id == reserved_9;
}

uint64_t SpecialTokens::n_special_tokens() const {
    uint64_t cnt = 0;
    cnt += (pad_id != -1);
    cnt += (mask_id != -1);
    cnt += (unk_id != -1);
    cnt += (bos_id != -1);
    cnt += (eos_id != -1);
    cnt += (reserved_5 != -1);
    cnt += (reserved_6 != -1);
    cnt += (reserved_7 != -1);
    cnt += (reserved_8 != -1);
    cnt += (reserved_9 != -1);
  return cnt;
}

Status SpecialTokens::id_to_subword(int id, std::string* subword) const {
    if (id < 0)
        return Status(1, "The value of id must be greater or equal 0. "
                         "Current value of id = " + std::to_string(id));
    if (id > max_id_value)
        return Status(1, "The value of id must be less or equal " + std::to_string(max_id_value) +
                         ". Current value of id = " + std::to_string(id));
    if (id == pad_id)
        *subword = PAD_TOKEN;
    else if (id == mask_id)
        *subword = MASK_TOKEN;
    else if (id == unk_id)
        *subword = UNK_TOKEN;
    else if (id == bos_id)
        *subword = BOS_TOKEN;
    else if (id == eos_id)
        *subword = EOS_TOKEN;
    else if (id == reserved_5)
        *subword = RESERVED_TOKEN_5;
    else if (id == reserved_6)
        *subword = RESERVED_TOKEN_6;
    else if (id == reserved_7)
        *subword = RESERVED_TOKEN_7;
    else if (id == reserved_8)
        *subword = RESERVED_TOKEN_8;
    else if (id == reserved_9)
        *subword = RESERVED_TOKEN_9;
    return Status();
}

int SpecialTokens::subword_to_id(const std::string& word) const {
    int id = -1;
    if (word == PAD_TOKEN)
        id = pad_id;
    else if (word == MASK_TOKEN)
        id = mask_id;
    else if (word == UNK_TOKEN)
        id = unk_id;
    else if (word == BOS_TOKEN)
        id = bos_id;
    else if (word == EOS_TOKEN)
        id = eos_id;
    else if (word == RESERVED_TOKEN_5)
        id = reserved_5;
    else if (word == RESERVED_TOKEN_6)
        id = reserved_6;
    else if (word == RESERVED_TOKEN_7)
        id = reserved_7;
    else if (word == RESERVED_TOKEN_8)
        id = reserved_8;
    else if (word == RESERVED_TOKEN_9)
        id = reserved_9;
    return id;
}

    bool BPE_Rule::operator==(const BPE_Rule &other) const {
  return x == other.x && y == other.y && z == other.z;
}

BPE_Rule::BPE_Rule(uint32_t x, uint32_t y, uint32_t z) : x(x), y(y), z(z) {}

void BPEState::dump(const string &file_name) {
  std::ofstream fout(file_name, std::ios::out);
  if (fout.fail()) {
    std::cerr << "Can't open file: " << file_name << std::endl;
    assert(false);
  }
  fout << char2id.size() << " " << rules.size() << std::endl;
  for (auto s : char2id) {
    fout << s.first << " " << s.second << std::endl;
  }

  for (auto rule : rules) {
    fout << rule.x << " " << rule.y << " " << rule.z << std::endl;
  }
  special_tokens.dump(fout);
  fout.close();
}

std::vector<int64_t> BPEState::serialize() {
    vector<int64_t> data;
    data.push_back(char2id.size());
    for(const auto &s : char2id) {
        data.push_back(s.first);
        data.push_back(s.second);
    }
    data.push_back(rules.size());
    for(const auto& it : rules) {
        data.push_back(it.x);
        data.push_back(it.y);
        data.push_back(it.z);
    }
    data.push_back(vocab.size());
    for(const auto& it : vocab)
        data.push_back(it);
    vector<int64_t> sp = special_tokens.serialize();
    data.push_back(sp.size());
    for(const auto& it : sp)
        data.push_back(it);
    return data;
}

Status BPEState::load(const string &file_name) {
  char2id.clear();
  rules.clear();
  std::ifstream fin(file_name, std::ios::in);
  if (fin.fail()) {
    return Status(1, "Can not open file with model: " + file_name);
  }
  int n, m;
  fin >> n >> m;
  for (int i = 0; i < n; i++) {
    uint32_t inner_id;
    uint32_t utf32_id;
    fin >> inner_id >> utf32_id;
    char2id[inner_id] = utf32_id;
  }
  for (int i = 0; i < m; i++) {
    uint32_t x, y, z;
    fin >> x >> y >> z;
    rules.emplace_back(x, y, z);
  }
  special_tokens.load(fin);
  fin.close();
  return Status();
}

Status BPEState::deserialize(std::vector<int64_t> data) {
    char2id.clear();
    rules.clear();
    vocab.clear();
    uint32_t idx = 0;
    uint32_t char2id_size = data[idx++];
    while(char2id.size() < char2id_size) {
        uint32_t inner_id = data[idx++];
        uint32_t utf32_id = data[idx++];
        char2id[inner_id] = utf32_id;
    }
    uint32_t rules_size = data[idx++];
    while(rules.size() < rules_size) {
        uint32_t x = data[idx++];
        uint32_t y = data[idx++];
        uint32_t z = data[idx++];
        rules.emplace_back(x, y, z);
    }
    uint32_t vocab_size = data[idx++];
    while(vocab.size() < vocab_size) {
        vocab.insert(data[idx++]);
    }
    int64_t sp_size = data[idx++];
    Status status = special_tokens.deserialize(data.begin() + idx, data.begin() + idx + sp_size);
    if(!status.ok())
        return status;
    return Status();
}

BpeConfig::BpeConfig(int _n_threads, int _min_token_count, const SpecialTokens &_special_tokens)
    : n_threads(_n_threads),
      min_token_count(_min_token_count),
      special_tokens(_special_tokens) {}

Status::Status(int code, std::string message) : code(code), message(std::move(message)) {}

const std::string &Status::error_message() const {
  return message;
}
bool Status::ok() const {
  return code == 0;
}
}  // namespace tokenizer
