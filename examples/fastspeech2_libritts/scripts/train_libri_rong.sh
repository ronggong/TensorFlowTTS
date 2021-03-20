#!/bin/bash

dump=/raid/rong.gong/data/tts/Libritts_360_rong_denoised/train_files_punct_0dur/dump_libritts_trim
outdir=outdir_libri360rong_fft2048_trim_punct0dur_2gpu_denoised
mkdir -p ./examples/fastspeech2_libritts/${outdir}
CUDA_VISIBLE_DEVICES= python examples/fastspeech2_libritts/train_fastspeech2.py \
  --train-dir $dump/train/ \
  --dev-dir $dump/valid/ \
  --outdir ./examples/fastspeech2_libritts/${outdir} \
  --config ./examples/fastspeech2_libritts/conf/fastspeech2libritts2.yaml \
  --use-norm 1 \
  --f0-stat $dump/stats_f0.npy \
  --energy-stat $dump/stats_energy.npy \
  --dataset_config preprocess/libritts_preprocess.yaml \
  --dataset_mapping $dump/libritts_mapper.json \
  --dataset_stats $dump/stats.npy > ./examples/fastspeech2_libritts/${outdir}/train.log
  # --mixed_precision 1 \