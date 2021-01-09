#!/bin/bash

# tensorflow-tts-preprocess --rootdir /raid/rong.gong/data/tts/Libritts_360 \
# 	--outdir /raid/rong.gong/data/tts/Libritts_360/dump_libritts_trim \
# 	--config preprocess/libritts_preprocess.yaml \
# 	--dataset libritts

tensorflow-tts-normalize --rootdir /raid/rong.gong/data/tts/Libritts_360/dump_libritts_trim \
	  --outdir /raid/rong.gong/data/tts/Libritts_360/dump_libritts_trim \
	  --config preprocess/libritts_preprocess.yaml \
	  --dataset libritts
