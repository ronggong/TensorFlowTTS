#!/bin/bash

dump=/raid/rong.gong/data/tts/aishell/organized_data/dump_trim_mfa
outdir=./examples/fastspeech2_multispeakers/outdir_aishell3_trim_end_1gpu_150k_16batch_size_phn_large
mkdir -p ${outdir}

CUDA_VISIBLE_DEVICES=3 python3 examples/fastspeech2_multispeakers/train_fastspeech2.py \
  --train-dir $dump/train/ \
  --dev-dir $dump/valid/ \
  --outdir  ${outdir}\
  --config ./examples/fastspeech2_multispeakers/conf/fastspeech2.aishell3.v1.yaml \
  --use-norm 1 \
  --f0-stat $dump/stats_f0.npy \
  --energy-stat $dump/stats_energy.npy \
  --mixed_precision 1 \
  --dataset_config preprocess/aishell3_preprocess_mfa.yaml \
  --dataset_mapping ${dump}/aishell3_mapper.json \
  --dataset_stats $dump/stats.npy \
  --dataset aishell3 > ${outdir}/train.log
