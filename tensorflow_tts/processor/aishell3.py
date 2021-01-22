# -*- coding: utf-8 -*-
# Copyright 2020 TensorFlowTTS Team.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Perform preprocessing and raw feature extraction for AISHELL3 dataset."""

import os
import re
from typing import Dict, List, Union, Tuple, Any

import librosa
import numpy as np
import soundfile as sf
from dataclasses import dataclass, field
from g2pM import G2pM
from tensorflow_tts.processor.baker import (
    PINYIN_DICT, is_zh, punc, _eos
)
from tensorflow_tts.processor import BaseProcessor

_punc = "＂＃＄％＆＇（）＊＋－／＜＝＞＠［＼］＾＿｀｛｜｝～｟｠｢｣､、〃》「」『』【】〔〕〖〗〘〙〚〛〜〝〞〟〰〾〿–—‘’‛“”„‟…‧﹏"
_punc_pause ="！？｡。，；：." 

@dataclass
class Aishell3Processor(BaseProcessor):

    pinyin_dict: Dict[str, Tuple[str, str]] = field(default_factory=lambda: PINYIN_DICT)
    cleaner_names: str = None
    target_rate: int = 24000

    def __post_init__(self):
        super().__post_init__()
        self.pinyin_parser = self.get_pinyin_parser()

    def setup_eos_token(self):
        return _eos[0]

    def _create_item_mfa(self):
        with open(
            os.path.join(self.data_dir, self.train_f_name), mode="r", encoding="utf-8"
            ) as f:
            for line in f:
                parts = line.strip().split(self.delimiter)
                utt_id = parts[self.positions["file"]].split("/")[1]
                wav_path = os.path.join(self.data_dir, parts[self.positions["file"]])
                wav_path = (
                    wav_path + self.f_extension
                    if wav_path[-len(self.f_extension) :] != self.f_extension
                    else wav_path
                )
                text = parts[self.positions["text"]]

                # # remove the eos token
                # if text.endswith(_eos[0]):
                #     text = text.replace(_eos[0], '').strip()
                    
                speaker_name = parts[self.positions["speaker_name"]]
                self.items.append([text, wav_path, utt_id, speaker_name])

    def create_items(self):
        if self.mfa:
            print("Use MFA aligned text")
            self._create_item_mfa()
        else:
            raise NotImplementedError("Create item method has only implemented for MFA case" + 
                " for AISHELL3 dataset.")

    def get_phoneme_from_char_and_pinyin(self, chn_char, pinyin):
        # we do not need #4, use sil to replace it
        chn_char = chn_char.replace("#4", "")
        char_len = len(chn_char)
        i, j = 0, 0
        result = ["sil"]
        while i < char_len:
            cur_char = chn_char[i]
            if is_zh(cur_char):
                if pinyin[j][:-1] not in self.pinyin_dict:
                    assert chn_char[i + 1] == "儿"
                    assert pinyin[j][-2] == "r"
                    tone = pinyin[j][-1]
                    a = pinyin[j][:-2]
                    a1, a2 = self.pinyin_dict[a]
                    result += [a1, a2 + tone, "er5"]
                    # if i + 2 < char_len and chn_char[i + 2] != "#":
                    #     result.append("#0")

                    i += 2
                    j += 1
                else:
                    tone = pinyin[j][-1]
                    a = pinyin[j][:-1]
                    a1, a2 = self.pinyin_dict[a]
                    result += [a1, a2 + tone]

                    # if i + 1 < char_len and chn_char[i + 1] != "#":
                    #     result.append("#0")

                    i += 1
                    j += 1
            elif cur_char == "#":
                result.append(chn_char[i : i + 2])
                i += 2
            else:
                # ignore the unknown char and punctuation
                # result.append(chn_char[i])
                i += 1
        if result[-1] == "#0":
            result = result[:-1]
        result.append("sil")
        assert j == len(pinyin)
        return result

    def get_one_sample(self, item):
        text, wav_file, utt_id, speaker_name = item

        try:
            # normalize audio signal to be [-1, 1], soundfile already norm.
            audio, rate = sf.read(wav_file)
            audio = audio.astype(np.float32)
            if rate != self.target_rate:
                assert rate > self.target_rate
                audio = librosa.resample(audio, rate, self.target_rate)

            # convert text to ids
            text_ids = np.asarray(self.text_to_sequence(text), np.int32)
        except Exception as e:
            print(e, utt_id, text)
            return {
                "raw_text": text,
                "text_ids": None,
                "audio": None,
                "utt_id": utt_id,
                "speaker_name": speaker_name,
                "rate": self.target_rate,
            }

        # return None
        sample = {
            "raw_text": text,
            "text_ids": text_ids,
            "audio": audio,
            "utt_id": utt_id,
            "speaker_name": speaker_name,
            "rate": self.target_rate,
        }

        return sample

    # def get_pinyin_parser(self):
    #     my_pinyin = Pinyin(MyConverter())
    #     pinyin = my_pinyin.pinyin
    #     return pinyin

    def get_pinyin_parser(self):
        model = G2pM()
        return model

    def text_to_sequence(self, text, inference=False):
        if inference:
            # Rong: found some punc e.g. "”，" not split if char_split=False
            pinyin = self.pinyin_parser(text, tone=True, char_split=True)
            # pinyin = self.pinyin_parser(text, style=Style.TONE3, errors="ignore")
            new_pinyin = []
            for x in pinyin:
                x = "".join(x)
                if "#" not in x and x not in punc:
                    new_pinyin.append(x)
            phonemes = self.get_phoneme_from_char_and_pinyin(text, new_pinyin)
            text = " ".join(phonemes)
            print(f"phoneme seq: {text}")

        sequence = []
        for symbol in text.split():
            idx = self.symbol_to_id[symbol]
            sequence.append(idx)
        
        if not self.mfa:
            # add eos tokens
            sequence += [self.eos_id]
        return sequence
