#include <iostream>
#include <unordered_map>
#include "../../src/bpe.h"

using namespace std;
using namespace tokenizer;

size_t split(const string &txt, vector<std::string> &strs, char ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;
        pos = txt.find( ch, initialPos );
    }
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );
    return strs.size();
}

vector<WordCountStr> count_word_from_string(const string &text_utf8) {
    vector<string> words;
    split(text_utf8, words, ' ');
    std::unordered_map<string, uint32_t> wc;
    for(auto& x : words) {
        auto it = wc.find(x);
        if(it != wc.end())
            it->second++;
        else
            wc[x] = 1;
    }
    vector<WordCountStr> word_count;
    word_count.reserve((wc.size()));
    for(auto& it : wc) {
        WordCountStr x = {it.first, it.second};
        word_count.emplace_back(x);
    }
    return word_count;
}

Status fast_read_file_utf8(const string &file_name, string *file_content) {
    static const int buf_size = 1000000;
    *file_content = "";
    auto fin = fopen(file_name.data(), "rb");
    if (fin == nullptr) {
        return Status(1, "Failed to open file: " + file_name);
    }
    while (true) {
        uint64_t cur_size = file_content->size();
        file_content->resize(cur_size + buf_size);
        int buf_len = fread((void *) (file_content->data() + cur_size), 1, buf_size, fin);
        if (buf_len < buf_size) {
            file_content->resize(file_content->size() - (buf_size - buf_len));
            fclose(fin);
            return Status();
        }
    }
}

vector<WordCountStr> count_word_from_file(const string &input_path) {
    string text_utf8;
    Status status = fast_read_file_utf8(input_path, &text_utf8);
    if (!status.ok()) {
        throw;
    }
    return count_word_from_string(text_utf8);
}

void print_config(const string &input_path, const string &model_path,
                  int vocab_size, BpeConfig bpe_config) {
    std::cerr << "Training parameters" << std::endl;
    std::cerr << "  input: " << input_path << std::endl;
    std::cerr << "  model: " << model_path << std::endl;
    std::cerr << "  vocab_size: " << vocab_size << std::endl;
    std::cerr << "  n_threads: " << bpe_config.n_threads << std::endl;
    std::cerr << "  unk: " << bpe_config.special_tokens.unk_id << std::endl;
    std::cerr << std::endl;
}


int main() {

    string input_path = "../../data/train_text.txt";

    BpeConfig config;
    config.n_threads = 1;
    config.min_token_count = 10;
    config.special_tokens.unk_id = 1;
    config.special_tokens.mask_id = 2;

    cout << "reading file..." << endl;
    string data;
    Status status = fast_read_file_utf8(input_path, &data);
    if (!status.ok()) {
        std::cerr << status.message << std::endl;
        exit(0);
    }
    vector<WordCountStr> wc_str = count_word_from_string(data);

    // 1-й вариан использования: обучение модели по данным частотам токенов
    vector<int64_t > model;
    status = train_bpe(wc_str,1000, config, &model);
    if (!status.ok()) {
        cerr << status.error_message() << endl;
        exit(0);
    }

    BaseEncoder bpe = BaseEncoder(model, -1, &status);
    if (!status.ok()) {
        cerr << status.message << endl;
        exit(0);
    }
    cout << "vocab size: " << bpe.vocab_size() << endl;
    vector<string> vocab = bpe.vocabulary();
//    for (auto& v : vocab)
//        cout << v << endl;

    // 2-й вариант использование: разделение токенов
    vector<string> snt = {"на", "わからわからПривет", "Привет", "мир", "!", "Украина"};
    vector<vector<string>> subwords;
    bpe.encode_as_subwords(snt, &subwords);
    for(auto& a : subwords) {
        for(auto& b : a) {
            cout << b << " ";
        }
        cout << endl;
    }

    vector<vector<int>> ids, ends;
    bpe.encode_as_ids(snt, &ids, &ends);
    int n = std::max(ids.size(), ends.size());
    for(int i = 0; i < n; i++){
        int m = std::max(ids[i].size(), ends[i].size());
        for(int j = 0; j < m; j++){
            cout << "(" << ids[i][j] << ", " << ends[i][j] << ") ";
        }
        cout << endl;
    }

    return 0;
}