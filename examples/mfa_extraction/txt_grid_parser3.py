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
"""Create training file and durations from textgrids for LibriTTS dataset
add punctuations with 0 duration in the train.txt"""

import os
from dataclasses import dataclass
from pathlib import Path

import click
import numpy as np
import textgrid
import yaml
from tqdm import tqdm
from nltk.tokenize import WordPunctTokenizer

import logging
import sys


logging.basicConfig(
    level=logging.DEBUG,
    stream=sys.stdout,
    format="%(asctime)s (%(module)s:%(lineno)d) %(levelname)s: %(message)s",
)


@dataclass
class TxtGridParser:
    sample_rate: int
    multi_speaker: bool
    txt_grid_path: str
    hop_size: int
    output_durations_path: str
    dataset_path: str
    training_file: str = "train.txt"
    phones_mapper = {"sil": "SIL", "sp": "SIL", "spn": "<unk>", "": "END"}
    """ '' -> is last token in every cases i encounter so u can change it for END but there is a safety check
        so it'll fail always when empty string isn't last char in ur dataset just chang it to silence then
    """
    sil_phones = set(phones_mapper.keys())
    tokenizer = WordPunctTokenizer()
    punct = "!,.:;?"
    #punct_mapper = {'!"': '!', ',"': ',', '"?': '?'}

    def parse(self):
        speakers = (
            [
                i
                for i in os.listdir(self.txt_grid_path)
                if os.path.isdir(os.path.join(self.txt_grid_path, i))
            ]
            if self.multi_speaker
            else []
        )
        data = []

        if speakers:
            for speaker in speakers:
                file_list = os.listdir(os.path.join(self.txt_grid_path, speaker))
                self.parse_text_grid(file_list, data, speaker)
        else:
            file_list = os.listdir(self.txt_grid_path)
            self.parse_text_grid(file_list, data, "")

        with open(os.path.join(self.dataset_path, self.training_file), "w") as f:
            f.writelines(data)

    def search_punct(self, tokens, text):
        """Search text in tokens list
        Returns:
            c: punct
            i+1: index of the punct in tokens
        """
        # print(tokens)
        for i in range(len(tokens) - 1):
            if text.lower() == tokens[i].lower():
                for c in tokens[i+1]:
                    if c in self.punct:
                        return c, i+1
                    else:
                        return None, i+1
        return None, None

    def parse_text_grid(self, file_list: list, data: list, speaker_name: str):
        logging.info(
            f"\n Parse: {len(file_list)} files, speaker name: {speaker_name} \n"
        )
        for f_name in tqdm(file_list):
            text_grid = textgrid.TextGrid.fromFile(
                os.path.join(self.txt_grid_path, speaker_name, f_name)
            )

            # Read transcript for search for the punct
            f_name_trans = os.path.join(self.dataset_path, speaker_name, os.path.splitext(f_name)[0]+'.txt')
            with open(f_name_trans, 'r') as fh:
                s = fh.read()
                tokens = self.tokenizer.tokenize(s)
            
            # Collect the interval end time of text tier
            # Some of the end time should be the sil begin time
            text = text_grid[0]
            dict_text = {}
            for iterator, interval in enumerate(text.intervals):
                dict_text[interval.maxTime] = interval.mark
                # print(interval.minTime, interval.maxTime, interval.mark)

            pha = text_grid[1]
            durations = []
            phs = []
            for iterator, interval in enumerate(pha.intervals):
                mark = interval.mark
                if mark in self.sil_phones:
                    # phone is a sil or its variants, we assume its begin time is the end time of a word
                    # By search for the end time, we can identify the word, then identify the next word
                    # If the next word contain a punctuation, we use it the replace the silence
                    if interval.minTime != 0 and interval.minTime in dict_text.keys():
                        t = dict_text[interval.minTime]
                        # print(t)
                        r = self.search_punct(tokens, t)
                        if r[1] is not None:
                            # remove all tokens have been searched for the next search
                            tokens = tokens[r[1]:]
                        if r[0] is not None:
                            # Punct found
                            punct = r[0]
                            durations.append(0)
                            phs.append(punct)
                    else:
                        r = (None, None)

                    # Mark is a sil, and we can't find any punct in the text
                    mark = self.phones_mapper[mark]
                    if mark == "END":
                        assert iterator == pha.intervals.__len__() - 1

                dur = interval.duration() * (self.sample_rate / self.hop_size)
                durations.append(round(dur))
                phs.append(mark)

            full_ph = " ".join(phs)

            assert full_ph.split(" ").__len__() == durations.__len__()  # safety check

            base_name = f_name.split(".TextGrid")[0]

            np.save(
                os.path.join(self.output_durations_path, f"{base_name}-durations.npy"),
                np.array(durations).astype(np.int32),
                allow_pickle=False,
            )
            data.append(f"{speaker_name}/{base_name}|{full_ph}|{speaker_name}\n")


@click.command()
@click.option(
    "--yaml_path", default="examples/fastspeech2_libritts/conf/fastspeech2libritts.yaml"
)
@click.option("--dataset_path", default="dataset", type=str, help="Dataset directory")
@click.option("--text_grid_path", default="mfa/parsed", type=str)
@click.option("--output_durations_path", default="dataset/durations")
@click.option("--sample_rate", default=24000, type=int)
@click.option("--multi_speakers", default=1, type=int, help="Use multi-speaker version")
@click.option("--train_file", default="train.txt")
def main(
    yaml_path: str,
    dataset_path: str,
    text_grid_path: str,
    output_durations_path: str,
    sample_rate: int,
    multi_speakers: int,
    train_file: str,
):

    with open(yaml_path) as file:
        attrs = yaml.load(file)
        hop_size = attrs["hop_size"]

    Path(output_durations_path).mkdir(parents=True, exist_ok=True)

    txt_grid_parser = TxtGridParser(
        sample_rate=sample_rate,
        multi_speaker=bool(multi_speakers),
        txt_grid_path=text_grid_path,
        hop_size=hop_size,
        output_durations_path=output_durations_path,
        training_file=train_file,
        dataset_path=dataset_path,
    )
    txt_grid_parser.parse()


if __name__ == "__main__":
    main()
