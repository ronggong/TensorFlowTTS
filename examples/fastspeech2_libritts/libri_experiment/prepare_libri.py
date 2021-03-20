import os
import random
import shutil
import sys

libri_path = "/raid/rong.gong/data/tts/LibriTTS_360/LibriTTS" # absolute path to TensorFlowTTS.
dataset_path = "/raid/rong.gong/data/tts/Libritts_360" # Change to your paths. This is a output of re-format dataset.
subset = "train-clean-360"


with open(os.path.join(libri_path, "SPEAKERS.txt")) as f:
    data = f.readlines()
    
dataset_info = {}
max_speakers = 100  # Max number of speakers to train on
min_len = 20  # Min len of speaker narration time
max_file_len = 15  # max audio file length
min_file_len = 2  # min audio file length

possible_dataset = [i.split("|") for i in data[12:] if i.split("|")[2].strip() == subset and float(i.split("|")[3].strip()) >= min_len]

ids = [i[0].strip() for i in possible_dataset]

import soundfile as sf

possible_map = {}
subset_path = os.path.join(libri_path, subset)
for i in os.listdir(subset_path):
    if i in ids:
        id_path = os.path.join(subset_path, i)
        id_dur = 0
        id_included = []
        
        for k in os.listdir(id_path):
            for j in os.listdir(os.path.join(id_path, k)):
                if ".wav" in j:
                    f_path = os.path.join(id_path, k, j)
                    sf_file =  sf.SoundFile(f_path)
                    dur = len(sf_file) / sf_file.samplerate
                    if max_file_len < dur or dur < min_file_len:
                        #print(dur)
                        continue
                    else:
                        id_included.append(f_path)
                        id_dur += dur
        possible_map[i] = {"dur": id_dur, "included": id_included}

poss_speakers = [[k, v["included"], v["dur"]] for k, v in possible_map.items() if v["dur"]/60 >= min_len]

# find the speakers with the descending durations
poss_speakers = sorted(poss_speakers, key = lambda x: x[2])[::-1]

#for s in poss_speakers:
#    print(s[0], s[2])

to_move = [s[0] for s in poss_speakers[:max_speakers]]

print('selected speakers', len(to_move))

poss_speakers = {s[0]: s[1] for s in poss_speakers}

#print('poss_speakers dict', poss_speakers)

# old code, random select speakers
#to_move = list(poss_speakers.keys())
#random.shuffle(to_move)
#to_move = to_move[:max_speakers]


for sp_id, v in poss_speakers.items():
    if sp_id in to_move:
        for j in v:
            f_name = j.split("/")[-1]
            text_f_name = f_name.split(".wav")[0] + ".txt"
            os.makedirs(os.path.join(dataset_path, sp_id), exist_ok=True)
            shutil.copy(j, os.path.join(dataset_path, sp_id, f_name))
            shutil.copy(j.replace(".wav", ".normalized.txt"), os.path.join(dataset_path, sp_id, text_f_name))
