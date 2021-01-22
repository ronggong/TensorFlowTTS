import os
import shutil
from tqdm import tqdm
from pathlib import Path
from mandarin_utils import (
    _pause,
    _initials,
    _finals,
    _tones,
    PINYIN_DICT,
    is_zh
)


baker_path = "/raid/rong.gong/data/tts/baker" # absolute path to TensorFlowTTS.
Path(os.path.join(baker_path, "wav_lab")).mkdir(parents=True, exist_ok=True)
lexicon_path = "/raid/rong.gong/work/tts/TensorFlowTTS/mfa/lexicon/baker-lexicon.txt"

# Transcription word level
# with open(
#     os.path.join(baker_path, "ProsodyLabeling/000001-010000.txt"),
#     encoding="utf-8",
#     ) as ttf:
#     lines = ttf.readlines()
#     for idx in tqdm(range(0, len(lines), 2)):
#         utt_id, chn_char = lines[idx].strip().split()
#         pinyin = lines[idx + 1].strip()
#         if "IY1" in pinyin or "Ｂ" in chn_char:
#             print(f"Skip this: {utt_id} {chn_char} {pinyin}")
#             continue
#         # copy wav
#         shutil.copy(os.path.join(baker_path, "Wave", utt_id+".wav"), 
#             os.path.join(baker_path, "wav_lab", utt_id+".wav"))
#         with open(os.path.join(baker_path, "wav_lab", utt_id+".lab"), "w") as lab:
#             lab.write(pinyin)

# Dictionary word level
# with open(lexicon_path, "w") as lf:
#     for pinyin, init_final in PINYIN_DICT.items():
#         for tone in _tones:
#             lf.write(f"{pinyin}{tone}\t{init_final[0]} {init_final[1]}{tone}\n")

Path(os.path.join(baker_path, "wav_lab_phn")).mkdir(parents=True, exist_ok=True)
lexicon_path = "/raid/rong.gong/work/tts/TensorFlowTTS/mfa/lexicon/baker-lexicon-phn.txt"

pinyin_dict = PINYIN_DICT
def get_phoneme_from_char_and_pinyin(chn_char, pinyin):
    # we do not need #4, use sil to replace it
    chn_char = chn_char.replace("#4", "")
    char_len = len(chn_char)
    i, j = 0, 0
    # result = ["sil"]
    result = []
    while i < char_len:
        cur_char = chn_char[i]
        if is_zh(cur_char):
            if pinyin[j][:-1] not in pinyin_dict:
                # print(chn_char[j], pinyin)
                assert chn_char[i + 1] == "儿"
                assert pinyin[j][-2] == "r"
                tone = pinyin[j][-1]
                a = pinyin[j][:-2]
                a1, a2 = pinyin_dict[a]
                result += [a1, a2 + tone, "er5"]
                if i + 2 < char_len and chn_char[i + 2] != "#":
                    result.append("p0")

                i += 2
                j += 1
            else:
                tone = pinyin[j][-1]
                a = pinyin[j][:-1]
                a1, a2 = pinyin_dict[a]
                result += [a1, a2 + tone]

                if i + 1 < char_len and chn_char[i + 1] != "#":
                    result.append("p0")

                i += 1
                j += 1
        elif cur_char == "#":
            result.append("p" + chn_char[i + 1 : i + 2])
            i += 2
        else:
            # ignore the unknown char and punctuation
            # result.append(chn_char[i])
            i += 1
    if result[-1] == "p0":
        result = result[:-1]
    # result.append("sil")
    assert j == len(pinyin)
    return result

# Transcription phn level
with open(
    os.path.join(baker_path, "ProsodyLabeling/000001-010000.txt"),
    encoding="utf-8",
    ) as ttf:
    lines = ttf.readlines()
    for idx in tqdm(range(0, len(lines), 2)):
        utt_id, chn_char = lines[idx].strip().split()
        pinyin = lines[idx + 1].strip().split()
        if "IY1" in pinyin or "Ｂ" in chn_char:
            print(f"Skip this: {utt_id} {chn_char} {pinyin}")
            continue
        phonemes = get_phoneme_from_char_and_pinyin(chn_char, pinyin)
        text = " ".join(phonemes)
        # copy wav
        shutil.copy(os.path.join(baker_path, "Wave", utt_id+".wav"), 
            os.path.join(baker_path, "wav_lab_phn", utt_id+".wav"))
        with open(os.path.join(baker_path, "wav_lab_phn", utt_id+".lab"), "w") as lab:
            lab.write(text)

# Dictionary phn level
with open(lexicon_path, "w") as lf:
    for c in _pause:
        lf.write(f"{c}\t{c}\n")
    for c in _initials:
        lf.write(f"{c}\t{c}\n")
    for c in _finals:
        for t in _tones:
            lf.write(f"{c}{t}\t{c}{t}\n")
