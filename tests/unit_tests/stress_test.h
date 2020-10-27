#pragma once

#include "../../third_party/flat_hash_map.h"
#include "../../src/utils.h"

namespace tokenizer {
Status learn_bpe_from_string(std::string &text_utf8,
                             int n_tokens,
                             const std::string &output_file,
                             BpeConfig bpe_config,
                             BPEState *bpe_state);

void utf8_to_chars(uint32_t x, std::back_insert_iterator<std::string> it);

uint32_t chars_to_utf8(const char *begin, size_t size, size_t *utf8_len);

}
