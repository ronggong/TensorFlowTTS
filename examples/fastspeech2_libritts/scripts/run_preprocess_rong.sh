#!/bin/bash

pathname=Libritts_360_rong_denoised
# tensorflow-tts-preprocess
# preprocess() + normalize() 
python -m tensorflow_tts.bin.preprocess \
	--rootdir /raid/rong.gong/data/tts/${pathname} \
	--outdir /raid/rong.gong/data/tts/${pathname}/dump_libritts_trim \
	--config preprocess/libritts_preprocess.yaml \
	--dataset libritts \
	--n_cpus 8

