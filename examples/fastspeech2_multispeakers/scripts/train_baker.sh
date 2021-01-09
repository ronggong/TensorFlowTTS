# dump=/raid/rong.gong/data/tts/baker_train/dump_trim_mfa
# outdir=./examples/fastspeech2_multispeakers/outdir_baker_trim_1gpu_200k_16batch_size
# mkdir -p ${outdir}
# CUDA_VISIBLE_DEVICES=3 python examples/fastspeech2_multispeakers/train_fastspeech2.py \
#   --train-dir $dump/train/ \
#   --dev-dir $dump/valid/ \
#   --outdir  ${outdir}\
#   --config ./examples/fastspeech2_multispeakers/conf/fastspeech2.baker.v2.yaml \
#   --use-norm 1 \
#   --f0-stat $dump/stats_f0.npy \
#   --energy-stat $dump/stats_energy.npy \
#   --mixed_precision 1 \
#   --dataset_config preprocess/baker_preprocess_mfa.yaml \
#   --dataset_mapping ${dump}/baker_mapper.json \
#   --dataset_stats $dump/stats.npy \
#   --dataset baker > ${outdir}/train.log

dump=/raid/rong.gong/data/tts/baker_train/dump_trim
outdir=./examples/fastspeech2_multispeakers/outdir_baker_trim_1gpu_200k_16batch_size_tacotron2dur
mkdir -p ${outdir}
CUDA_VISIBLE_DEVICES=3 python examples/fastspeech2_multispeakers/train_fastspeech2.py \
  --train-dir $dump/train/ \
  --dev-dir $dump/valid/ \
  --outdir  ${outdir}\
  --config ./examples/fastspeech2_multispeakers/conf/fastspeech2.baker.v2.yaml \
  --use-norm 1 \
  --f0-stat $dump/stats_f0.npy \
  --energy-stat $dump/stats_energy.npy \
  --mixed_precision 1 \
  --dataset_config preprocess/baker_preprocess.yaml \
  --dataset_mapping ${dump}/baker_mapper.json \
  --dataset_stats $dump/stats.npy \
  --dataset baker > ${outdir}/train.log
