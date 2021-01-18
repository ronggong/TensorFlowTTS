#!/bin/bash

datadir=/raid/rong.gong/data/tts/baker_train_phn

# #tensorflow-tts-preprocess \
python -m tensorflow_tts.bin.preprocess \
	--rootdir ${datadir} \
	--outdir ${datadir}/dump_trim_mfa \
	--config preprocess/baker_preprocess_mfa.yaml \
	--dataset baker \
	--n_cpus 4 \
	--mfa

# # #tensorflow-tts-normalize 
# python -m tensorflow_tts.bin.preprocess \
# 	  --rootdir ${datadir}/dump_trim_mfa \
# 	  --outdir ${datadir}/dump_trim_mfa \
# 	  --config preprocess/baker_preprocess_mfa.yaml \
# 	  --dataset baker