#!/bin/bash


#/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/run_mfa_validation.py \
#    --corpus_directory /raid/rong.gong/data/tts/Libritts

#/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/run_mfa.py \
#    --corpus_directory /raid/rong.gong/data/tts/Libritts_360 \
#    --output_directory ./mfa/parsed \
#    --jobs 8
#
#/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/txt_grid_parser.py \
#	  --yaml_path examples/fastspeech2_libritts/conf/fastspeech2libritts.yaml \
#	  --dataset_path  /raid/rong.gong/data/tts/Libritts_360 \
#	  --text_grid_path ./mfa/parsed \
#	  --output_durations_path /raid/rong.gong/data/tts/Libritts_360/durations \
#	  --sample_rate 24000

# The textgrid parsing including punctuations
# /raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/txt_grid_parser2.py \
# 	  --yaml_path examples/fastspeech2_libritts/conf/fastspeech2libritts.yaml \
# 	  --dataset_path  /raid/rong.gong/data/tts/Libritts_360 \
# 	  --text_grid_path ./mfa/parsed \
# 	  --output_durations_path /raid/rong.gong/data/tts/Libritts_360/durations \
# 	  --sample_rate 24000

/raid/rong.gong/venvs/tf-23/bin/python examples/mfa_extraction/fix_mismatch.py \
	  --base_path /raid/rong.gong/data/tts/Libritts_360/dump_libritts_trim \
	  --trimmed_dur_path /raid/rong.gong/data/tts/Libritts_360/trimmed-durations \
	  --dur_path /raid/rong.gong/data/tts/Libritts_360/durations
