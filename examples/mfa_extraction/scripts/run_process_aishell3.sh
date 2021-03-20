#!/bin/bash

datadir=/raid/rong.gong/data/tts/aishell/organized_data
# # MFA alignment phonemes
# ./mfa/montreal-forced-aligner/bin/mfa_train_and_align \
#    ${datadir}/wav_lab \
#    ./mfa/lexicon/baker-lexicon-phn.txt \
#    ./mfa/textgrid/aishell3 \
#    -j 8 \
#    -v \
#    --config_path examples/mfa_extraction/scripts/aishell3_train_mfa.yaml

# # Parse phn alignment
# /raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/txt_grid_parser.py \
# 	--yaml_path examples/fastspeech2_multispeakers/conf/fastspeech2.baker.v2.yaml \
# 	--dataset_path  ${datadir}/wav_lab \
# 	--text_grid_path ./mfa/textgrid/aishell3 \
# 	--output_durations_path ${datadir}/wav_lab/durations \
#    --multi_speakers 1 \
# 	--sample_rate 24000 \
#    --dataset aishell3

# phn
/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/fix_mismatch.py \
	--base_path ${datadir}/dump_trim_mfa \
	--trimmed_dur_path ${datadir}/wav_lab/trimmed-durations \
	--dur_path ${datadir}/wav_lab/durations