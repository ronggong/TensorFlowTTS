"""check the puntuations in the libri transcriptions"""

import os
import string
from nltk.tokenize import WordPunctTokenizer

dataset_path = "/raid/rong.gong/data/tts/Libritts_360" # Change to your paths. This is a output of re-format dataset.
tokenizer = WordPunctTokenizer()

dict_punct = {}
for i in os.listdir(dataset_path):
    if os.path.isfile(os.path.join(dataset_path, i)):
        continue
    for f in os.listdir(os.path.join(dataset_path, i)):
        if f.endswith('.txt'):
            f = os.path.join(dataset_path, i, f)
            with open(f, 'r') as fh:
                s = fh.read()
                tokens = tokenizer.tokenize(s)
                for t in tokens:
                    for c in t:
                        if c in string.punctuation and c != '/' and c != '_':
                            dict_punct[t] = dict_punct.get(t, 0) + 1 
                            break

print(dict_punct)