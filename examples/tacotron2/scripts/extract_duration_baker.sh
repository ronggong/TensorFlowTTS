dump=/raid/rong.gong/data/tts/baker_train/dump_trim
CUDA_VISIBLE_DEVICES=3 python examples/tacotron2/extract_duration.py \
  --rootdir ${dump}/valid/ \
  --outdir ${dump}/valid/durations/ \
  --checkpoint ./examples/tacotron2/exp/train.tacotron2.baker.v1_batch32/checkpoints/model-100000.h5 \
  --use-norm 1 \
  --config ./examples/tacotron2/conf/tacotron2.baker.v1.yaml \
  --batch-size 32 \
  --win-front 3 \
  --win-back 3

CUDA_VISIBLE_DEVICES=3 python examples/tacotron2/extract_duration.py \
  --rootdir ${dump}/train/ \
  --outdir ${dump}/train/durations/ \
  --checkpoint ./examples/tacotron2/exp/train.tacotron2.baker.v1_batch32/checkpoints/model-100000.h5 \
  --use-norm 1 \
  --config ./examples/tacotron2/conf/tacotron2.baker.v1.yaml \
  --batch-size 32 \
  --win-front 3 \
  --win-back 3