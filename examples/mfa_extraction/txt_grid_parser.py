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
"""Create training file and durations from textgrids."""

import os
from dataclasses import dataclass
from pathlib import Path

import click
import numpy as np
import textgrid
import yaml
from tqdm import tqdm

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
    txt_grid_tier: int
    hop_size: int
    output_durations_path: str
    dataset_path: str
    dataset: str = "libritts"
    training_file: str = "train.txt"
    phone_set_file: str = "phones.txt"

    def __post_init__(self):
        if self.dataset=='baker' or self.dataset=='aishell3':
            #self.phones_mapper = {"sil": "sil", "sp": "#0", "spn": "#1", "": "eos"}
            self.phones_mapper = {
                "sil": "sil",
                "sp": "sil", 
                "spn": "sil",
                "p0": "#0",
                "p1": "#1",
                "p2": "#2",
                "p3": "#3",
                "nc": "^",
                "": "eos",
                "sp1": "sil"}

            # er phns in baker native annotation
            self.phones_mapper2 = {
                "io": "iou",
                "air": "ai",
                "anr": "an",
                "aor": "ao",
                "ar": "a",
                "enr": "en",
                "ianr": "ian",
                "iangr": "iang",
                "iar": "ia",
                "iiir": "iii",
                "ingr": "ing",
                "inr": "in",
                "iour": "iou",
                "ir": "i",
                "ongr": "ong",
                "our": "ou",
                "uair": "uai",
                "uanr": "uan",
                "ueir": "uei",
                "uenr": "uen",
                "uor": "uo",
                "ur": "u",
                "vanr": "van",
                "ng": "en"
            }
        else:
            self.phones_mapper = {"sil": "SIL", "sp": "SIL", "spn": "SIL", "": "END"}
            self.phones_mapper2 = {}
        """ '' -> is last token in every cases i encounter so u can change it for END but there is a safety check
            so it'll fail always when empty string isn't last char in ur dataset just chang it to silence then
        """
        self.sil_phones = set(self.phones_mapper.keys())
        self.phones2 = set(self.phones_mapper2.keys())

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
        phn_set = []

        if speakers:
            for speaker in speakers:
                file_list = os.listdir(os.path.join(self.txt_grid_path, speaker))
                self.parse_text_grid(file_list, data, phn_set, speaker)
        else:
            file_list = os.listdir(self.txt_grid_path)
            self.parse_text_grid(file_list, data, phn_set, "")

        with open(os.path.join(self.dataset_path, self.training_file), "w") as f:
            f.writelines(data)

        phn_set = list(set(phn_set))
        phn_set.sort()
        with open(os.path.join(self.dataset_path, self.phone_set_file), "w") as f:
            f.writelines(phn_set)

    def parse_text_grid(
        self, 
        file_list: list, 
        data: list, 
        phn_set: list,
        speaker_name: str
        ):

        logging.info(
            f"\n Parse: {len(file_list)} files, speaker name: {speaker_name} \n"
        )
        
        for f_name in tqdm(file_list):
            # try:
            text_grid = textgrid.TextGrid.fromFile(
                os.path.join(self.txt_grid_path, speaker_name, f_name)
            )
            # except Exception as e:
            #     print(f"{e}, {f_name}")
            #     continue
            pha = text_grid[self.txt_grid_tier]
            durations = []
            phs = []
            write_this = True
            # print_full_ph = False
            for iterator, interval in enumerate(pha.intervals):
                mark = interval.mark
                if mark is None:
                    continue

                mark_nodigit = ''.join(i for i in mark if not i.isdigit())
                
                if self.dataset == 'baker' and mark_nodigit in ['iyl', 'pl']:
                    write_this = False
                    break

                if mark in self.sil_phones:
                    mark = self.phones_mapper[mark]
                    if mark == "END":
                        assert iterator == pha.intervals.__len__() - 1
                        # check if empty ph is always last example in your dataset if not fix it
                elif mark_nodigit in self.phones2:
                    # remove er phns in baker native labels
                    if not mark[-1].isdigit():
                        raise ValueError(f"{mark} last char is not digit.")
                    mark = self.phones_mapper2[mark_nodigit] + mark[-1]

                dur = interval.duration() * (self.sample_rate / self.hop_size)
                durations.append(round(dur))
                phs.append(mark)

                # export phone set
                mark_nodigit = ''.join(i for i in mark if not i.isdigit())
                phn_set.append(mark_nodigit+"\n")

            if not write_this:
                # not write for this file, because something's wrong earlier
                continue

            full_ph = " ".join(phs)

            # if print_full_ph:
            #     with open(os.path.join(self.dataset_path, f_name+'.txt'), "w") as f:
            #         f.write(full_ph)

            assert full_ph.split(" ").__len__() == durations.__len__()  # safety check

            if self.txt_grid_tier == 0:
                base_name = f_name.split(".interval")[0]
            else:
                base_name = f_name.split(".TextGrid")[0]
            data.append(f"{speaker_name}/{base_name}|{full_ph}|{speaker_name}\n")

            if self.dataset=='baker':
                base_name=str(int(base_name))
            np.save(
                os.path.join(self.output_durations_path, f"{base_name}-durations.npy"),
                np.array(durations).astype(np.int32),
                allow_pickle=False,
            )


@click.command()
@click.option(
    "--yaml_path", default="examples/fastspeech2_libritts/conf/fastspeech2libritts.yaml"
)
@click.option("--dataset_path", default="dataset", type=str, help="Dataset directory")
@click.option("--text_grid_path", default="mfa/parsed", type=str)
@click.option("--text_grid_tier", default=1, type=int)
@click.option("--output_durations_path", default="dataset/durations")
@click.option("--sample_rate", default=24000, type=int)
@click.option("--multi_speakers", default=1, type=int, help="Use multi-speaker version")
@click.option("--train_file", default="train.txt")
@click.option("--dataset", type=str, default="librtts")
def main(
    yaml_path: str,
    dataset_path: str,
    text_grid_path: str,
    text_grid_tier: int,
    output_durations_path: str,
    sample_rate: int,
    multi_speakers: int,
    train_file: str,
    dataset: str
):

    with open(yaml_path) as file:
        attrs = yaml.load(file)
        hop_size = attrs["hop_size"]

    Path(output_durations_path).mkdir(parents=True, exist_ok=True)

    txt_grid_parser = TxtGridParser(
        sample_rate=sample_rate,
        multi_speaker=bool(multi_speakers),
        txt_grid_path=text_grid_path,
        txt_grid_tier=text_grid_tier,
        hop_size=hop_size,
        output_durations_path=output_durations_path,
        training_file=train_file,
        dataset_path=dataset_path,
        dataset=dataset
    )
    txt_grid_parser.parse()


if __name__ == "__main__":
    main()
