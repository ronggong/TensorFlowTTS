dump=/raid/rong.gong/data/tts/Libritts_360/dump_libritts_trim
CUDA_VISIBLE_DEVICES=2 python examples/fastspeech2_libritts/train_fastspeech2.py \
  --train-dir $dump/train/ \
  --dev-dir $dump/valid/ \
  --outdir ./examples/fastspeech2_libritts/outdir_libri360_fft2048_trim_punct_1gpu/ \
  --config ./examples/fastspeech2_libritts/conf/fastspeech2libritts.yaml \
  --use-norm 1 \
  --f0-stat $dump/stats_f0.npy \
  --energy-stat $dump/stats_energy.npy \
  --mixed_precision 1 \
  --dataset_config preprocess/libritts_preprocess.yaml \
  --dataset_stats $dump/stats.npy > ./examples/fastspeech2_libritts/outdir_libri360_fft2048_trim_punct_2gpu/train.log
