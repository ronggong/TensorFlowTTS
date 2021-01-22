#!/bin/bash

datadir=/raid/rong.gong/data/tts/aishell

# # select speakers, copy audio, parse text
# python3 examples/fastspeech2_multispeakers/scripts/prepare_aishell3.py \
#     --dataset_path ${datadir}/AISHELL-3/train/wav/ \
#     --output_path ${datadir}/organized_data/wav_lab \
#     --transcript ${datadir}/AISHELL-3/train/label_train-set.txt


datadir=/raid/rong.gong/data/tts/aishell/organized_data

# ## #tensorflow-tts-preprocess \
# python -m tensorflow_tts.bin.preprocess \
# 	--rootdir ${datadir}/wav_lab \
# 	--outdir ${datadir}/dump_trim_mfa \
# 	--config preprocess/aishell3_preprocess_mfa.yaml \
# 	--dataset aishell3 \
# 	--n_cpus 4 \
# 	--mfa

# #tensorflow-tts-normalize 
python -m tensorflow_tts.bin.preprocess \
 	  --rootdir ${datadir}/dump_trim_mfa \
 	  --outdir ${datadir}/dump_trim_mfa \
 	  --config preprocess/aishell3_preprocess_mfa.yaml \
 	  --dataset aishell3