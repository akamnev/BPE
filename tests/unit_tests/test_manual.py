import os
import pybpe


def count_words(text):
    word_count = {}
    for word in text.split():
        try:
            word_count[word] += 1
        except KeyError:
            word_count[word] = 1
    word_count = sorted([(k, v) for k, v in word_count.items()],
                        key=lambda x: x[1])
    return word_count


def test_russian():
    train_text = "собирать cборник сборище отобранный сборщица"
    test_text = "собранный собрание прибор"
    wc = count_words(train_text)
    model = pybpe.BPE.train(wc, 50)
    bpe = pybpe.BPE(model)
    tokenized_text = bpe.encode(test_text.split(),
                                output_type=pybpe.OutputType.SUBWORD)
    expected_result = [["с", "обранный"], ["с", "об", "ран", "и", "е"],
                       ["п", "р", "и", "бор"]]
    assert tokenized_text == expected_result


def test_english():
    train_text = """
        anachronism
        synchronous  
        chronology
        chronic
        chronophilia
        chronoecological
        chronocoulometry
        """

    test_text = "chronocline synchroscope "

    wc = count_words(train_text)
    model = pybpe.BPE.train(wc, 200)
    bpe = pybpe.BPE(model)
    tokenized_text = bpe.encode(test_text.split(), output_type=pybpe.OutputType.SUBWORD)
    expected_result = [['chrono', 'c', 'l', 'i', 'n', 'e'], ['s', 'y', 'n', 'ch', 'r', 'o', 's', 'co', 'p', 'e']]
    assert tokenized_text == expected_result


def test_japanese():
    train_text = """
        むかし、 むかし、 ある ところ に
        おじいさん と おばあさん が いました。
        おじいさん が 山（やま） へ 木（き） を きり に いけば、
        おばあさん は 川（かわ） へ せんたく に でかけます。
        「おじいさん、 はよう もどって きなされ。」
        「おばあさん も き を つけて な。」
        まい日（にち） やさしく いい あって でかけます 
    """
    test_text = " おばあさん が  川 で せん "
    wc = count_words(train_text)
    model = pybpe.BPE.train(wc, 100)
    bpe = pybpe.BPE(model)
    tokenized_text = bpe.encode(test_text.split(), output_type=pybpe.OutputType.SUBWORD)
    expected_result = [["おばあさん"], ["が"],  ["川"], ["で"], ["せ", "ん"]]
    assert tokenized_text == expected_result
