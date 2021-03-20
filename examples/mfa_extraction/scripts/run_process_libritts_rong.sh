#!/bin/bash

pathname=Libritts_360_rong_denoised

#/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/run_mfa_validation.py \
#    --corpus_directory /raid/rong.gong/data/tts/Libritts

#/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/run_mfa.py \
#    --corpus_directory /raid/rong.gong/data/tts/${pathname} \
#    --output_directory ./mfa/textgrid/${pathname} \
#    --jobs 8

# Textgrid parsing no include puncts
#/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/txt_grid_parser.py \
#	  --yaml_path examples/fastspeech2_libritts/conf/fastspeech2libritts.yaml \
#	  --dataset_path  /raid/rong.gong/data/tts/Libritts_360_rong \
#	  --text_grid_path ./mfa/textgrid/libritts_360_rong \
#	  --output_durations_path /raid/rong.gong/data/tts/Libritts_360_rong/durations \
#	  --sample_rate 24000

# The textgrid parsing including punctuations
#/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/txt_grid_parser2.py \
# 	  --yaml_path examples/fastspeech2_libritts/conf/fastspeech2libritts.yaml \
# 	  --dataset_path  /raid/rong.gong/data/tts/Libritts_360_rong \
# 	  --text_grid_path ./mfa/textgrid/libritts_360_rong \
# 	  --output_durations_path /raid/rong.gong/data/tts/Libritts_360_rong/durations \
# 	  --sample_rate 24000

## The textgrid parsing including punctuations but with 0 durations
#/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/txt_grid_parser3.py \
# 	  --yaml_path examples/fastspeech2_libritts/conf/fastspeech2libritts.yaml \
# 	  --dataset_path  /raid/rong.gong/data/tts/${pathname} \
# 	  --text_grid_path ./mfa/textgrid/${pathname} \
# 	  --output_durations_path /raid/rong.gong/data/tts/${pathname}/durations \
# 	  --sample_rate 24000

/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/fix_mismatch.py \
	  --base_path /raid/rong.gong/data/tts/${pathname}/dump_libritts_trim \
	  --trimmed_dur_path /raid/rong.gong/data/tts/${pathname}/trimmed-durations \
	  --dur_path /raid/rong.gong/data/tts/${pathname}/durations
