#!/bin/bash

# This doesn't work...
# ./mfa/montreal-forced-aligner/bin/mfa_generate_dictionary \
#    ./mfa/montreal-forced-aligner/pretrained_models/mandarin_pinyin_g2p.zip \
#    /raid/rong.gong/data/tts/baker/wav_lab \
#    ./mfa/lexicon/baker-lexicon.txt

# ./mfa/montreal-forced-aligner/bin/mfa_validate_dataset \
#    /raid/rong.gong/data/tts/baker_train \
#    ./mfa/lexicon/baker-lexicon.txt \
#    -j 8

# Get MFA alignment
# ./mfa/montreal-forced-aligner/bin/mfa_train_and_align \
#    /raid/rong.gong/data/tts/baker_train \
#    ./mfa/lexicon/baker-lexicon.txt \
#    ./mfa/textgrid/baker \
#    -j 8

# MFA alignment phonemes
# ./mfa/montreal-forced-aligner/bin/mfa_train_and_align \
#    /raid/rong.gong/data/tts/baker_train_phn \
#    ./mfa/lexicon/baker-lexicon-phn.txt \
#    ./mfa/textgrid/baker_phn \
#    -j 8

# Parse word alignment
# /raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/txt_grid_parser.py \
# 	  --yaml_path examples/fastspeech2_multispeakers/conf/fastspeech2.baker.v2.yaml \
# 	  --dataset_path  /raid/rong.gong/data/tts/baker_train \
# 	  --text_grid_path ./mfa/textgrid/baker \
# 	  --output_durations_path /raid/rong.gong/data/tts/baker_train/durations \
# 	  --sample_rate 24000 \
#     --dataset baker

# # Parse phn alignment
# /raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/txt_grid_parser.py \
# 	  --yaml_path examples/fastspeech2_multispeakers/conf/fastspeech2.baker.v2.yaml \
# 	  --dataset_path  /raid/rong.gong/data/tts/baker_train_phn \
# 	  --text_grid_path ./mfa/textgrid/baker_phn \
# 	  --output_durations_path /raid/rong.gong/data/tts/baker_train_phn/durations \
# 	  --sample_rate 24000 \
#     --dataset baker

# # Parse baker native textgrid alignment
# /raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/txt_grid_parser.py \
# 	--yaml_path examples/fastspeech2_multispeakers/conf/fastspeech2.baker.v2.yaml \
# 	--dataset_path  /raid/rong.gong/data/tts/baker_train_phn \
# 	--text_grid_path /raid/rong.gong/data/tts/baker/PhoneLabeling \
# 	--text_grid_tier 0 \
# 	--output_durations_path /raid/rong.gong/data/tts/baker_train_phn/durations \
# 	--sample_rate 24000 \
#     --dataset baker

# fix mismatch word
# /raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/fix_mismatch.py \
# 	  --base_path /raid/rong.gong/data/tts/baker_train/dump_trim_mfa \
# 	  --trimmed_dur_path /raid/rong.gong/data/tts/baker_train/trimmed-durations \
# 	  --dur_path /raid/rong.gong/data/tts/baker_train/durations

# phn
/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/fix_mismatch.py \
	  --base_path /raid/rong.gong/data/tts/baker_train_phn/dump_trim_mfa \
	  --trimmed_dur_path /raid/rong.gong/data/tts/baker_train_phn/trimmed-durations \
	  --dur_path /raid/rong.gong/data/tts/baker_train_phn/durations

# # phn native textgrid alignment
# /raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/fix_mismatch.py \
# 	  --base_path /raid/rong.gong/data/tts/baker_train_phn/dump_trim_native_align \
# 	  --trimmed_dur_path /raid/rong.gong/data/tts/baker_train_phn/trimmed-durations \
# 	  --dur_path /raid/rong.gong/data/tts/baker_train_phn/durations